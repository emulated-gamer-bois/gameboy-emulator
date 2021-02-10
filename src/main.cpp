#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif

#include <GL/glew.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <chrono>

#include <labhelper.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

///////////////////////////////////////////////////////////////////////////////
// Various globals
///////////////////////////////////////////////////////////////////////////////
SDL_Window* g_window = nullptr;
float currentTime = 0.0f;
float previousTime = 0.0f;
float deltaTime = 0.0f;
int windowWidth, windowHeight;

GLuint testProgram;

void display(void) {
    // Create quad
    vec2 vertices[] = { { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 1.0f, 1.0f },
                        { -1.0f, -1.0f }, { 1.0f, 1.0f },  { -1.0f, 1.0f } };
    int vertexAmount = 6;
    GLuint vertexArrayObject = 0;

    if(vertexArrayObject == 0)
    {
        glGenVertexArrays(1, &vertexArrayObject);
        labhelper::createAddAttribBuffer(vertexArrayObject, vertices, sizeof(vertices), 0, 2, GL_FLOAT);
    }
    glBindVertexArray(vertexArrayObject);

    // Check if window size has changed and resize buffers as needed
    {
        int w, h;
        SDL_GetWindowSize(g_window, &w, &h);
        if(w != windowWidth || h != windowHeight)
        {
            windowWidth = w;
            windowHeight = h;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0., 0., 0., 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(testProgram);

    glDrawArrays(GL_TRIANGLES, 0, vertexAmount);
}

int main(int argc, char* argv[]) {
    g_window = labhelper::init_window_SDL("Lame Boy");

    GLuint shader = labhelper::loadShaderProgram("../src/shaders/test.vert",
                                                 "../src/shaders/test.frag",
                                                 false);
    if(shader != 0)
    {
        testProgram = shader;
    }

    bool stopRendering = false;
    auto startTime = std::chrono::system_clock::now();

    while(!stopRendering) {
        //update currentTime
        std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
        previousTime = currentTime;
        currentTime = timeSinceStart.count();
        deltaTime = currentTime - previousTime;
        // render to window
        display();

        // Swap front and back buffer. This frame will now been displayed.
        SDL_GL_SwapWindow(g_window);

        // Check if it is time to quit or not
        {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    stopRendering = true;
                }
            }
        }
    }

    labhelper::shutDown(g_window);

	return 0;
}
