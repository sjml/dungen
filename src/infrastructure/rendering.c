#include "../stdafx.h"
#include "rendering.h"

#include "util.h"
#include "world.h"
#include "text.h"
#include "outline.h"

const int windowWidth  = 1024;
const int windowHeight = 768;

GLFWwindow* window = NULL;

CameraData MainCamera;
gbMat4 viewMatrix;
gbMat4 modelMatrix;
gbMat4 perspectiveMatrix;
gbMat4 orthoMatrix;

TileSet** tileSets = NULL;

typedef struct {
    char* text;
    gbVec2 pos;
    float scale;
    gbVec4 color;
} textInfo;
textInfo* textInfos = NULL;


void InitializeRendering() {
    glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_TRUE);
    glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_TRUE);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(windowWidth, windowHeight, "DunGen", NULL, NULL);
    if (!window) {
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
//    glfwSetWindowSizeCallback(Camera::ResizeCallback);
//    glfwSetKeyCallback(keyboardInput);
//    glfwSetCharCallback(charInput);
//    glfwDisable(GLFW_KEY_REPEAT);
//    glfwSetMousePosCallback(MouseMotion);
//    glfwSetMouseButtonCallback(MouseButton);
//    glfwSetMouseWheelCallback(MouseWheel);
//    glfwSetWindowCloseCallback(windowClosed);

    glClearDepth(1.0f);
    glPolygonMode(GL_FRONT, GL_FILL);
    glShadeModel(GL_FLAT);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    // commented because of easy_text; restore culling if text gets fancier
    // glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CCW);
    // glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearStencil(0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    MainCamera.aperture = M_PI / 2;
    MainCamera.position.x = 0.0f;
    MainCamera.position.y = 0.0f;
    MainCamera.position.z = 10.0f;
    MainCamera.view.x = 0.0f;
    MainCamera.view.y = 0.0f;
    MainCamera.view.z = -10.0f;
    MainCamera.up.x = 0.0f;
    MainCamera.up.y = 1.0f;
    MainCamera.up.z = 0.0f;
    MainCamera.zFarClip = 200.0f;
    MainCamera.zNearClip = 0.001f;
    MainCamera.windowWidth = windowWidth;
    MainCamera.windowHeight = windowHeight;

    gb_mat4_perspective(&viewMatrix, MainCamera.aperture, (float)MainCamera.windowWidth / (float)MainCamera.windowHeight, MainCamera.zNearClip, MainCamera.zFarClip);
    gb_mat4_look_at(&modelMatrix, MainCamera.position, MainCamera.view, MainCamera.up);
    gb_mat4_mul(&perspectiveMatrix, &viewMatrix, &modelMatrix);

    gb_mat4_ortho2d(&orthoMatrix, 0.0f, 1024.0f, 768.0f, 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(perspectiveMatrix.e);
    handleGLErrors(__FILE__, __LINE__);
}

void FinalizeRendering() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void AddTileSetToRendering(TileSet* ts) {
    arrpush(tileSets, ts);
}

void RemoveTileSetFromRendering(TileSet* ts) {
    for (int i = 0; i < arrlen(tileSets); i++) {
        if (tileSets[i] == ts) {
            arrdel(tileSets, i);
            return;
        }
    }
    fprintf(stderr, "ERROR: removing TileSet that was not part of the world.\n");
}

TileSet** GetRenderingTileSets() {
    return tileSets;
}

gbVec2 WorldToScreen(gbVec2* worldCoordinates) {
    gbVec4 wc = {worldCoordinates->x, worldCoordinates->y, 0.0f, 1.0f};
    gbVec4 clip;
    gb_mat4_mul_vec4(&clip, &viewMatrix, wc);
    gb_mat4_mul_vec4(&clip, &perspectiveMatrix, wc);
    gbVec3 ndc;
    gb_vec3_div(&ndc, clip.xyz, clip.w);

    // these would properly come from the viewport function if we had one
    gbVec2 viewDim = { 1024.0f, 768.0f };
    gbVec2 viewOffset = { 0.0f, 0.0f };
    
    gbVec2 spos = {
            (
                (ndc.x + 1.0) / 2.0
            ) * viewDim.x + viewOffset.x,
            (
                (1.0 - ndc.y) / 2.0
            ) * viewDim.y + viewOffset.y
    };

    return spos;
}

void ClearTextStrings(void) {
    while (arrlen(textInfos) > 0) {
        textInfo ti = arrpop(textInfos);
        free(ti.text);
    }
}

void AddTextString(char* text, gbVec2* pos, float scale, gbVec4* color) {
    textInfo ti;
    ti.text = malloc(sizeof(char) * (strlen(text) + 1));
    strcpy(ti.text, text);
    ti.pos.x = pos->x;
    ti.pos.y = pos->y;
    ti.scale = scale;
    ti.color.r = color->r;
    ti.color.g = color->g;
    ti.color.b = color->b;
    ti.color.a = color->a;
    arrpush(textInfos, ti);
}

int Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        RenderTiles();
        for (int i = 0; i < arrlen(tileSets); i++) {
            if (tileSets[i]->outline != NULL) {
                RenderOutline(tileSets[i]->outline);
            }
        }
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPushMatrix();
        glMultMatrixf(orthoMatrix.e);
        for (int i=0; i < arrlen(textInfos); i++) {
            PrintTextString(textInfos[i].text, &textInfos[i].pos, &textInfos[i].color, textInfos[i].scale, -1.0f);
        }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glfwSwapBuffers(window);
    glfwPollEvents();

    handleGLErrors(__FILE__, __LINE__);

    if (glfwWindowShouldClose(window)) {
        return 1;
    }
    return 0;
}
