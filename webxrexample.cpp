#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <emscripten.h>

#include "webxrexample.h"
#include "shaderbuilder.h"
#include "glm/gtx/string_cast.hpp"

int WebXrExample::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("Dear ImGui Emscripten example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    GLContext = SDL_GL_CreateContext(window);
    if (!GLContext)
    {
        fprintf(stderr, "Failed to initialize WebGL context!\n");
        return -1;
    }
    SDL_GL_SetSwapInterval(1); // Enable vsync

    initBuffers();
    initShaders();

    setupCallBack();
    initWebXr();

    return 1;
}

/* Create a simple triangle mesh plane in the XY-plane
 */
void createGrid(std::vector<glm::vec3>& points, int gridSize, float stepSize) {
    int numVertices = (2*gridSize)*(2*gridSize)*10;
    points = std::vector<glm::vec3>(numVertices);

    size_t cnt = 0;
    for(int i = -gridSize; i < gridSize; ++i) {
        for(int j = -gridSize; j < gridSize; ++j) {
            glm::vec3 A = glm::vec3(float(i) * stepSize, 0.0f, float(j) * stepSize);
            glm::vec3 B = glm::vec3(float(i) * stepSize + stepSize, 0.0f, float(j) * stepSize);
            glm::vec3 C = glm::vec3(float(i) * stepSize, 0.0f, float(j) * stepSize + stepSize);
            glm::vec3 D = glm::vec3(float(i) * stepSize + stepSize, 0.0f, float(j) * stepSize + stepSize);

            points[cnt] = A;
            points[cnt + 1] = B;

            points[cnt + 2] = B;
            points[cnt + 3] = C;

            points[cnt + 4] = C;
            points[cnt + 5] = A;


            points[cnt + 6] = B;
            points[cnt + 7] = C;

            points[cnt + 8] = C;
            points[cnt + 9] = D;

            points[cnt + 10] = D;
            points[cnt + 11] = B;
            cnt += 12;
        }    
    }
}

void WebXrExample::initBuffers() {
    std::vector<glm::vec3> points;
    createGrid(points, 40, 1.0f);
    numVertices = points.size();
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

    glBindVertexArray(0);
}



void WebXrExample::initShaders() {
    shader = ShaderBuilder::createShaderProgram(
                   "shaders/vertvertex.glsl",
                   "shaders/fragvertex.glsl");
}

/* Source: https://github.com/mosra/magnum/blob/master/src/Magnum/Platform/EmscriptenApplication.cpp
 * EmscriptenApplication::setupAnimationFrame
*/
void WebXrExample::setupCallBack() {
    _callback = [](void* userData) -> int {
        auto& app = *static_cast<WebXrExample*>(userData);
        app.drawEyes();

        //This should return false sometime?

        return true;
    };
}

/* Source: https://github.com/mosra/magnum/blob/master/src/Magnum/Platform/EmscriptenApplication.cpp
 * EmscriptenApplication::redraw
*/
void WebXrExample::drawLoop() {
    EM_ASM({
        var drawEvent = function() {
            var id = window.requestAnimationFrame(drawEvent);

            if(!wasmTable.get($0).apply(null, [$1])) {
                window.cancelAnimationFrame(id);
            }
        };

        window.requestAnimationFrame(drawEvent);
    }, _callback, this);
}


void WebXrExample::initWebXr() {
    webxr_init(
        WEBXR_SESSION_MODE_IMMERSIVE_VR,
        /* Frame callback */
        [](void* userData, int, float[16], WebXRView* views) {
            static_cast<WebXrExample*>(userData)->drawWebXRFrame(views);
        },
        /* Session end callback */
        [](void* userData) {
            static_cast<WebXrExample*>(userData)->sessionStart();
        },
        /* Session end callback */
        [](void* userData) {
            static_cast<WebXrExample*>(userData)->sessionEnd();
        },
        /* Error callback */
        [](void* userData, int error) {
            static_cast<WebXrExample*>(userData)->onError(error);
        },
        /* userData */
        this);
}

void WebXrExample::sessionStart() {
    if(xr_inXR) return;
    xr_inXR = true;
    std::cout << "Entered VR" << std::endl;
}

void WebXrExample::sessionEnd() {
    xr_inXR = false;
    std::cout << "Exited VR" << std::endl;
    
    drawLoop();
}

void WebXrExample::onError(int error) {
    switch(error) {
        case WEBXR_ERR_API_UNSUPPORTED:
             std::cerr << "WebXR unsupported in this browser.";
             break;
        case WEBXR_ERR_GL_INCAPABLE:
             std::cerr << "GL context cannot be used to render to WebXR";
             break;
        case WEBXR_ERR_SESSION_UNSUPPORTED:
             std::cerr << "VR not supported on this device";
             break;
        default:
             std::cerr << "Unknown WebXR error with code" << error;
    }
}

void WebXrExample::drawWebXRFrame(WebXRView* views) {
    int viewIndex = 0;
    for(WebXRView view : {views[0], views[1]}) {
        xr_viewports[viewIndex] = glm::ivec4(view.viewport[0], view.viewport[1], view.viewport[2], view.viewport[3]);
        xr_viewMatrices[viewIndex] = glm::make_mat4(view.viewMatrix);   
		xr_projectionMatrices[viewIndex] = glm::make_mat4(view.projectionMatrix);

        ++viewIndex;
    }

    /*
    WebXRInputSource sources[2];
    int sourcesCount = 0;
    webxr_get_input_sources(sources, 5, &sourcesCount);

    for(int i = 0; i < sourcesCount; ++i) {
        webxr_get_input_pose(&sources[i], _controllerTransformations[i].data());
    }*/
    
   drawEyes();
}

void WebXrExample::drawEyes() {
    if(xr_inXR) {
        glClearColor(0.0f, 0.0f, 0.0, 1.0f);
    } else {
        glClearColor(1.0, 0.0f, 0.0, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const int viewCount = xr_inXR ? 2 : 1;
    for(int eye = 0; eye < viewCount; ++eye) {
        draw(eye);
    }
}


void WebXrExample::draw(int eye) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if(event.type == SDL_MOUSEBUTTONDOWN) {
            //do something
        } else if(event.type == SDL_KEYDOWN) {
            //do something
        }
    }
    
    renderScene(eye);

    SDL_GL_SwapWindow(window);
}

void WebXrExample::renderScene(int eye) {
    if(!xr_inXR) {
        xr_projectionMatrices[0] = glm::perspective(90.0f, 3.0f/4.0f, 0.01f, 100.0f);
        xr_viewMatrices[0] = glm::mat4();
        xr_viewports[0] = glm::ivec4(0, 0, 500, 500); 
    }

    glm::ivec4 viewport = xr_viewports[eye];
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    glm::mat4x4 viewMatrix = xr_viewMatrices[eye];
    viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0, -1.5, 0.0)); //translate so that we are above plane

    glBindVertexArray(vao);
    glUseProgram(shader);

    GLuint mvpLocation2 = glGetUniformLocation(shader, "proj_matrix");
    glUniformMatrix4fv(mvpLocation2, 1, GL_FALSE, glm::value_ptr(xr_projectionMatrices[eye]));

    GLuint mvpLocation = glGetUniformLocation(shader, "matrix");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    GLuint colorLocation = glGetUniformLocation(shader, "vertexCol");
    glUniform3f(colorLocation, 0.0f, 1.0f, 0.0f);

    //draw grid of points and lines
    glDrawArrays(GL_LINES, 0, numVertices);
    
    glDrawArrays(GL_POINTS, 0, numVertices);
    
    glBindVertexArray(0);
    glUseProgram(0);
}