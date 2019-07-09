#include "../stdafx.h"
#include "rendering.h"

#include "util.h"
#include "../world.h"

const int windowWidth  = 1024;
const int windowHeight = 768;

GLFWwindow* window = NULL;

CameraData MainCamera;
hmm_mat4 viewMatrix;

const float hexVertices[] =
{
    0.0f,               0.0f,
    0.5f * -0.8660254f, 0.5f *  0.5f,
    0.5f * -0.8660254f, 0.5f * -0.5f,
    0.5f *  0.0000000f, 0.5f * -1.0f,
    0.5f *  0.8660254f, 0.5f * -0.5f,
    0.5f *  0.8660254f, 0.5f *  0.5f,
    0.5f *  0.0f,       0.5f *  1.0f,
    0.5f * -0.8660254f, 0.5f *  0.5f,
};

//const float hexTexVertices[] =
//{
//    hexVertices[0]  + 0.5f, hexVertices[1]  + 0.5f,
//    hexVertices[2]  + 0.5f, hexVertices[3]  + 0.5f,
//    hexVertices[4]  + 0.5f, hexVertices[5]  + 0.5f,
//    hexVertices[6]  + 0.5f, hexVertices[7]  + 0.5f,
//    hexVertices[8]  + 0.5f, hexVertices[9]  + 0.5f,
//    hexVertices[10] + 0.5f, hexVertices[11] + 0.5f,
//    hexVertices[12] + 0.5f, hexVertices[13] + 0.5f,
//    hexVertices[14] + 0.5f, hexVertices[15] + 0.5f,
//};



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
    
    MainCamera.aperture = 90.0f;
    MainCamera.position.X = 0.0f;
    MainCamera.position.Y = 0.0f;
    MainCamera.position.Z = 10.0f;
    MainCamera.view.X = 0.0f;
    MainCamera.view.Y = 0.0f;
    MainCamera.view.Z = -10.0f;
    MainCamera.up.X = 0.0f;
    MainCamera.up.Y = 1.0f;
    MainCamera.up.Z = 0.0f;
    MainCamera.zFarClip = 200.0f;
    MainCamera.zNearClip = 0.001f;
    MainCamera.windowWidth = windowWidth;
    MainCamera.windowHeight = windowHeight;
    
    viewMatrix = HMM_Perspective(90.0f, (float)MainCamera.windowWidth / (float)MainCamera.windowHeight, MainCamera.zNearClip, MainCamera.zFarClip);
    hmm_mat4 look = HMM_LookAt(MainCamera.position, MainCamera.view, MainCamera.up);
    viewMatrix = HMM_MultiplyMat4(viewMatrix, look);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf((const float*) viewMatrix.Elements);
    handleGLErrors(__FILE__, __LINE__);
}

void FinalizeRendering() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, hexVertices);
    
    RenderTiles();

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
