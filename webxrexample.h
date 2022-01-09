#pragma once

//#include "imgui.h"
//#include "imgui_impl_sdl.h"
//#include "imgui_impl_opengl3.h"

#include <SDL.h>
#include <SDL_opengles2.h>
#include <glm/glm.hpp>

#include <iostream>
#include "webxr.h"


class WebXrExample {

public:
    int init();

    void sessionStart();
    void sessionEnd();
    void onError(int error);
    void drawWebXRFrame(WebXRView* views);

    void drawEyes();
    void drawLoop();

    void render();
private:
    glm::mat4x4 xr_viewMatrices[2];
    glm::mat4x4 xr_projectionMatrices[2];
    glm::ivec4 xr_viewports[2];
    bool xr_inXR = false;

    size_t numVertices;
    GLuint vao, vbo, shader;

    SDL_Window* window = NULL;
    SDL_GLContext GLContext = NULL;

    void draw(int eye);
    void initWebXr();

    void initBuffers();
    void initShaders();
    void renderScene(int eye);

    void setupCallBack();
    int (*_callback)(void*);
};