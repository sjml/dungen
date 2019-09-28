#include "stdafx.h"
#include "rendering.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "util.h"
#include "world.h"
#include "text.h"
#include "outline.h"
#include "game.h"
#include  "../ui/banner.h"
#include  "../ui/choice.h"
#include  "../ui/tile_choice.h"

static bool renderingInitialized = false;

static Vec2i windowDimensions;
static Vec2i framebufferDimensions;
static Vec2i orthoDimensions;

static GLubyte* screenShotBuffer;

static CameraData MainCamera;
static gbMat4 projectionMatrix;
static gbMat4 modelViewMatrix;
static gbMat4 perspectiveMatrix;
static gbMat4 orthoMatrix;

static GLfloat hexVerts[] = {
    -0.8660254f,  -0.5f,  // 1
     0.0000000f,  -1.0f,  // 2
    -0.8660254f,   0.5f,  // 0
     0.8660254f,  -0.5f,  // 3
     0.0f,         1.0f,  // 5
     0.8660254f,   0.5f,  // 4
};

static GLfloat squareVerts[] = {
    -0.5f, -0.5f,
    -0.5f,  0.5f,
     0.5f,  0.5f,
     0.5f, -0.5f,
};

static gbMat4 hexModelMatrix;
static GLuint hexVAO;
static GLuint hexVBO;
static GLuint attribVBO;
static GLuint hexProgram;
static GLint  hexVPMatLocation;

static GLuint basicProgram;
static GLuint squareVAO;
static GLuint squareVBO;

static GLuint outlineVPMatLocation;
static GLuint outlineColorVecLocation;

static Region** regions = NULL;

static const int defaultWindowWidth  = 1024;
static const int defaultWindowHeight = 768;

#if !(DUNGEN_MOBILE)
    static GLFWwindow* window = NULL;

    void _glfwSetup() {
        glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_TRUE);

        if (!glfwInit()) {
            exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        window = glfwCreateWindow(defaultWindowWidth, defaultWindowHeight, "DunGen", NULL, NULL);
        if (!window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwGetFramebufferSize(window, &framebufferDimensions.x, &framebufferDimensions.y);
        glfwGetWindowSize(window, &windowDimensions.x, &windowDimensions.y);

        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);

        glfwSetCursorPosCallback(window, MouseMoveCallback);
        glfwSetMouseButtonCallback(window, MouseClickCallback);
        glfwSetKeyCallback(window, KeyboardCallback);
    }
#endif // DUNGEN_MOBILE

void InitializeRendering() {
    #if !(DUNGEN_MOBILE)
        _glfwSetup();
    #endif // !(DUNGEN_MOBILE)

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    MainCamera.aperture = GB_MATH_PI / 2;
    MainCamera.position.x = 0.0f;
    MainCamera.position.y = 0.0f;
    MainCamera.position.z = 10.0f;
    MainCamera.view.x = 0.0f;
    MainCamera.view.y = 0.0f;
    MainCamera.view.z = -10.0f;
    MainCamera.up.x = 0.0f;
    MainCamera.up.y = 1.0f;
    MainCamera.up.z = 0.0f;
    MainCamera.zFarClip = 1.0f;
    MainCamera.zNearClip = -1.0f;
    MainCamera.windowWidth = windowDimensions.x;
    MainCamera.windowHeight = windowDimensions.y;

    float aspect = (float)MainCamera.windowWidth / (float)MainCamera.windowHeight;
    gb_mat4_perspective(&projectionMatrix, MainCamera.aperture, aspect, MainCamera.zNearClip, MainCamera.zFarClip);
    gb_mat4_look_at(&modelViewMatrix, MainCamera.position, MainCamera.view, MainCamera.up);
    gb_mat4_mul(&perspectiveMatrix, &projectionMatrix, &modelViewMatrix);

    gbVec2 scaleDims;
    scaleDims.x = GetTileDimensions().y * 0.5f;
    scaleDims.y = scaleDims.x;
    hexVerts[ 0] *= scaleDims.x; hexVerts[ 1] *= scaleDims.y;
    hexVerts[ 2] *= scaleDims.x; hexVerts[ 3] *= scaleDims.y;
    hexVerts[ 4] *= scaleDims.x; hexVerts[ 5] *= scaleDims.y;
    hexVerts[ 6] *= scaleDims.x; hexVerts[ 7] *= scaleDims.y;
    hexVerts[ 8] *= scaleDims.x; hexVerts[ 9] *= scaleDims.y;
    hexVerts[10] *= scaleDims.x; hexVerts[11] *= scaleDims.y;

    orthoDimensions.y = defaultWindowHeight;
    orthoDimensions.x = (int)((float)defaultWindowHeight * aspect);
    gb_mat4_ortho2d(&orthoMatrix, 0.0f, (float)orthoDimensions.x, (float)orthoDimensions.y, 0.0f);

    glGenVertexArrays(1, &squareVAO);
    glBindVertexArray(squareVAO);
    glGenBuffers(1, &squareVBO);
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(squareVerts),
        squareVerts,
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);
    
    glGenVertexArrays(1, &hexVAO);
    glBindVertexArray(hexVAO);
    glGenBuffers(1, &hexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, hexVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(hexVerts),
        hexVerts,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);

    glGenBuffers(1, &attribVBO);
    glBindBuffer(GL_ARRAY_BUFFER, attribVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(TileDrawData) * GetNumberOfTiles(),
        GetTileStartPointer()->draw,
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TileDrawData), (void*)offsetof(TileDrawData, worldPos));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TileDrawData), (void*)offsetof(TileDrawData, color));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(TileDrawData), (void*)offsetof(TileDrawData, overlayColor));
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    hexProgram = LoadProgram("shaders/gl/hex_vert.glsl", "shaders/gl/basic_frag.glsl");
    hexVPMatLocation = glGetUniformLocation(hexProgram, "vp");

    basicProgram = LoadProgram("shaders/gl/basic_vert.glsl", "shaders/gl/basic_frag.glsl");
    outlineVPMatLocation = glGetUniformLocation(basicProgram, "vp");
    outlineColorVecLocation = glGetUniformLocation(basicProgram, "color");

//    glfwGetFramebufferSize(window, &frameW, &frameH);
//    screenShotBuffer = malloc(sizeof(GLubyte) * frameW * frameH * 3);
//    stbi_flip_vertically_on_write(1);

    InitializeText();
    LoadFont("Pixel", "fonts/04B_03__.TTF", 32.0, true);

    renderingInitialized = true;
}

void UpdateRenderBuffers(TileSet* ts) {
    if (!renderingInitialized || hmlen(ts) == 0) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, attribVBO);

    // TODO: this might be a performance sink; could be more clever about batching these together
    for (int i=0; i < hmlen(ts); i++) {
        TileData* td = ts[i].key;
        glBufferSubData(
            GL_ARRAY_BUFFER,
            sizeof(TileDrawData) * td->meta->i,
            sizeof(TileDrawData),
            td->draw
        );
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FinalizeRendering() {
    free(screenShotBuffer);

    FinalizeText();

    #if !(DUNGEN_MOBILE)
        glfwDestroyWindow(window);
        glfwTerminate();
    #endif // !(DUNGEN_MOBILE)
}

Vec2i GetWindowDimensions(void) {
    return windowDimensions;
}

Vec2i GetFramebufferDimensions(void) {
    return framebufferDimensions;
}

Vec2i GetOrthoDimensions(void) {
    return orthoDimensions;
}

void SetWindowDimensions(Vec2i dims) {
    windowDimensions.x = dims.x;
    windowDimensions.y = dims.y;
}

void SetFramebufferDimensions(Vec2i dims) {
    framebufferDimensions.x = dims.x;
    framebufferDimensions.y = dims.y;
}

void DumpScreenShot(const char* fileName) {
//    glReadPixels(0, 0, frameW, frameH, GL_RGB, GL_UNSIGNED_BYTE, screenShotBuffer);
//    stbi_write_png(fileName, frameW, frameH, 3, screenShotBuffer, frameW * 3);
}

void AddRegionToRendering(Region* r) {
    arrpush(regions, r);
}

void RemoveRegionFromRendering(Region* r) {
    for (int i = 0; i < arrlen(regions); i++) {
        if (regions[i] == r) {
            arrdel(regions, i);
            return;
        }
    }
    fprintf(stderr, "ERROR: removing Region that was not part of the world.\n");
}

Region** GetRenderingRegions() {
    return regions;
}

#if !(DUNGEN_MOBILE)
    gbVec2 GetCursorPosition(void) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        gbVec2 pos = {(float)x, (float)y};
        return pos;
    }
#else
    gbVec2 GetCursorPosition(void) {
        gbVec2 pos = {-1.0f, -1.0f};
        return pos;
    }
#endif // !(DUNGEN_MOBILE)

gbVec2 WorldToScreen(gbVec2 worldCoordinates) {
    gbVec4 wc = {worldCoordinates.x, worldCoordinates.y, 0.0f, 1.0f};
    gbVec4 clip;
    gb_mat4_mul_vec4(&clip, &projectionMatrix, wc);
    gb_mat4_mul_vec4(&clip, &perspectiveMatrix, wc);
    gbVec3 ndc;
    gb_vec3_div(&ndc, clip.xyz, clip.w);

    gbVec2 viewOffset = { 0.0f, 0.0f };

    gbVec2 spos = {
            (
                (ndc.x + 1.0f) / 2.0f
            ) * windowDimensions.x + viewOffset.x,
            (
                (1.0f - ndc.y) / 2.0f
            ) * windowDimensions.y + viewOffset.y
    };

    return spos;
}

gbVec2 ScreenToWorld(gbVec2 screenCoordinates) {
    gbMat4 persp, final;
    gb_mat4_mul(&persp, &projectionMatrix, &modelViewMatrix);
    gb_mat4_inverse(&final, &persp);
    gbVec4 in = {
        screenCoordinates.x, screenCoordinates.y, 0.0f, 1.0f
    };

    gbVec2 viewOffset = { 0.0f, 0.0f };

    in.x = (in.x - viewOffset.x) / windowDimensions.x;
    in.y = (in.y - viewOffset.y) / windowDimensions.y;

    in.x = in.x * 2 - 1;
    in.y = in.y * 2 - 1;
    in.z = in.z * 2 - 1;

    gbVec4 out;
    gb_mat4_mul_vec4(&out, &final, in);
    out.x /= out.w;
    out.y /= out.w;
    out.z /= out.w;

    float multFactor = 10.0f; // TODO: this shouldn't be necessary :-/
    gbVec2 ret = {out.x * -multFactor, out.y * multFactor};
    return ret;
}

gbVec2 ScreenToOrtho(gbVec2 screenCoordinates) {
    float xPerc = screenCoordinates.x / windowDimensions.x;
    float yPerc = screenCoordinates.y / windowDimensions.y;

    gbVec2 ret = {
        xPerc * orthoDimensions.x,
        yPerc * orthoDimensions.y
    };
    return ret;
}

GLuint GetSquareVAO(void) {
    return squareVAO;
}

GLuint GetSquareVBO(void) {
    return squareVBO;
}

GLuint GetBasicProgram() {
    return basicProgram;
}

GLuint LoadProgram(const char* vertexFile, const char* fragmentFile) {
    const char* vertexSrc = NULL;
    const char* fragmentSrc = NULL;
    if (vertexFile != NULL) {
        vertexSrc = readTextFile(vertexFile);
        if (vertexSrc == NULL) {
            fprintf(stderr, "ERROR: couldn't load %s:\n", vertexFile);
            return 0;
        }
    }
    if (fragmentFile != NULL) {
        fragmentSrc = readTextFile(fragmentFile);
        if (fragmentSrc == NULL) {
            fprintf(stderr, "ERROR: couldn't load %s:\n", fragmentFile);
            return 0;
        }
    }

    GLint success;
    GLuint vs = 0, gs = 0, fs = 0;

    if (vertexSrc != NULL) {
        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertexSrc, NULL);
        glCompileShader(vs);
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            fprintf(stderr, "ERROR: couldn't compile %s:\n", vertexFile);
            char shader_log[4096];
            GLsizei log_length;
            glGetShaderInfoLog(vs, 4096, &log_length, shader_log);
            fprintf(stderr, "\t%s\n", shader_log);
        }
    }

    if (fragmentSrc != NULL) {
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragmentSrc, NULL);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            printf("ERROR: couldn't compile %s:\n", fragmentFile);
            char shader_log[4096];
            GLsizei log_length;
            glGetShaderInfoLog(fs, 4096, &log_length, shader_log);
            fprintf(stderr, "\t%s\n", shader_log);
        }
    }

    GLuint shaderIndex = glCreateProgram();
    if (vertexSrc != NULL) {
        glAttachShader(shaderIndex, vs);
    }
    if (fragmentSrc != NULL) {
        glAttachShader(shaderIndex, fs);
    }
    glLinkProgram(shaderIndex);
    glGetProgramiv(shaderIndex, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        fprintf(stderr, "ERROR: couldn't link %s and %s:\n", vertexFile, fragmentFile);
        char shader_log[4096];
        GLsizei log_length;
        glGetProgramInfoLog(shaderIndex, 4096, &log_length, shader_log);
        fprintf(stderr, "\t%s\n", shader_log);
        glDeleteProgram(shaderIndex);
        shaderIndex = 0;
    }

    if (vertexSrc != NULL) {
        free((void*)vertexSrc);
        glDeleteShader(vs);
    }
    if (fragmentSrc != NULL) {
        free((void*)fragmentSrc);
        glDeleteShader(fs);
    }

    return shaderIndex;
}

int Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(hexProgram);
    glUniformMatrix4fv(hexVPMatLocation, 1, GL_FALSE, perspectiveMatrix.e);

    glBindVertexArray(hexVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, (int)GetNumberOfTiles());
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glBindVertexArray(0);


    glUseProgram(basicProgram);
    glUniformMatrix4fv(outlineVPMatLocation, 1, GL_FALSE, perspectiveMatrix.e);

    for (long ri=0; ri < arrlen(regions); ri++) {
        if (regions[ri]->outline != NULL && regions[ri]->outline->vbo != 0) {
            glUniform4fv(outlineColorVecLocation, 1, regions[ri]->outline->color.e);
            glBindVertexArray(regions[ri]->outline->vao);
            glEnableVertexAttribArray(0);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, regions[ri]->outline->numPoints);
            glDisableVertexAttribArray(0);
        }
    }
    glBindVertexArray(0);
    
    PrepDrawText(&orthoMatrix);
    for (int i=0; i < arrlen(regions); i++) {
        if (regions[i]->label.scale >= 0.0f) {
            DrawText("Pixel", regions[i]->label.text, regions[i]->label.pos, regions[i]->label.color, regions[i]->label.scale);
        }
    }
    FinishDrawText();
    
    RenderBanners(&orthoMatrix);
    if (GetChoiceStatus() >= 0) {
        RenderChoices(&orthoMatrix);
    }
    
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glPushMatrix();
//        glMultMatrixf(orthoMatrix.e);
//        if (GetTileChoiceStatus() >= 0) {
//            RenderTileChoice();
//        }
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();
//
//    GL_CHECK();
//
    #if !(DUNGEN_MOBILE)
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            return 1;
        }
    #endif // !(DUNGEN_MOBILE)

    return 0;
}
