#include "../stdafx.h"
#include "rendering.h"

#include "util.h"
#include "world.h"

const int windowWidth  = 1024;
const int windowHeight = 768;

GLFWwindow* window = NULL;

CameraData MainCamera;
gbMat4 viewMatrix;

Outline** outlines = NULL;

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
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
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
    gbMat4 look;
    gb_mat4_look_at(&look, MainCamera.position, MainCamera.view, MainCamera.up);
    gb_mat4_mul(&viewMatrix, &viewMatrix, &look);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(viewMatrix.e);
    handleGLErrors(__FILE__, __LINE__);
}

void FinalizeRendering() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void AddOutline(Outline* o) {
    arrpush(outlines, o);
}

void RemoveOutline(Outline* o) {
    for (int i = 0; i < arrlen(outlines); i++) {
        if (outlines[i] == o) {
            arrdel(outlines, i);
            break;
        }
    }
    fprintf(stderr, "ERROR: removing outline that was not part of the world.\n");
}

int Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    RenderTiles();
    for (int i = 0; i < arrlen(outlines); i++) {
        RenderOutline(outlines[i]);
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glfwSwapBuffers(window);
    glfwPollEvents();

    handleGLErrors(__FILE__, __LINE__);

    if (glfwWindowShouldClose(window)) {
        return 1;
    }
    return 0;
}
