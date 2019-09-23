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

static Vec2i windowDimensions;
static Vec2i framebufferDimensions;
static Vec2i orthoDimensions;

static GLubyte* screenShotBuffer;

static CameraData MainCamera;
static gbMat4 projectionMatrix;
static gbMat4 modelViewMatrix;
static gbMat4 perspectiveMatrix;
static gbMat4 orthoMatrix;

static Region** regions = NULL;

static const int defaultWindowWidth  = 1024;
static const int defaultWindowHeight = 768;

#if !(DUNGEN_MOBILE)
    static GLFWwindow* window = NULL;

    void _glfwSetup() {
        #if DEBUG
            glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
        #else
            glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_TRUE);
        #endif

        glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_TRUE);

        if (!glfwInit()) {
            exit(EXIT_FAILURE);
        }

        //    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        //    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        //    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
        glClearDepth(1.0f);
        glPolygonMode(GL_FRONT, GL_FILL);
    #endif // !(DUNGEN_MOBILE)

    glShadeModel(GL_FLAT);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glEnableClientState(GL_VERTEX_ARRAY);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearStencil(0);
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

    orthoDimensions.y = defaultWindowHeight;
    orthoDimensions.x = (int)((float)defaultWindowHeight * aspect);
    gb_mat4_ortho2d(&orthoMatrix, 0.0f, (float)orthoDimensions.x, (float)orthoDimensions.y, 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(perspectiveMatrix.e);
    handleGLErrors(__FILE__, __LINE__);

//    glfwGetFramebufferSize(window, &frameW, &frameH);
//    screenShotBuffer = malloc(sizeof(GLubyte) * frameW * frameH * 3);
//    stbi_flip_vertically_on_write(1);

    InitializeText();
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

int Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

        RenderTiles();

        for (long i = 0; i < arrlen(regions); i++) {
            if (regions[i]->outline != NULL) {
                RenderOutline(regions[i]->outline);
            }
        }
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPushMatrix();
        glMultMatrixf(orthoMatrix.e);
        for (int i = 0; i < arrlen(regions); i++) {
            if (regions[i]->label.scale >= 0.0f) {
                glColor4f(
                    regions[i]->label.color.r,
                    regions[i]->label.color.g,
                    regions[i]->label.color.b,
                    regions[i]->label.color.a
                );
                DrawGameText(
                    regions[i]->label.text,
                    "fonts/04B_03__.TTF",
                    regions[i]->label.scale,
                    (int)regions[i]->label.pos.x,
                    (int)regions[i]->label.pos.y,
                    0.0f
                );
            }
        }
        RenderBanners();
        if (GetChoiceStatus() >= 0) {
            RenderChoices();
        }
        if (GetTileChoiceStatus() >= 0) {
            RenderTileChoice();
        }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    handleGLErrors(__FILE__, __LINE__);

    #if !(DUNGEN_MOBILE)
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwWindowShouldClose(window)) {
            return 1;
        }
    #endif // !(DUNGEN_MOBILE)

    return 0;
}
