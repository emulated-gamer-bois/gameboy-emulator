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

#include "RenderView.h"
#include "temp_screen_helper.h"
#include <iostream>

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

int main(int argc, char* argv[]) {
    g_window = labhelper::init_window_SDL("Lame Boy", 160*2, 144*2);

    RenderView* rv = new RenderView(2);

    bool stopRendering = false;
    auto startTime = std::chrono::system_clock::now();

    // Deluxe temp------------------------------------------------------------------------------------------------------
    uint8_t* byteArr = temporary::importTempscreen();
    if (!byteArr) {
        delete rv;
        labhelper::shutDown(g_window);
        return 0;
    }
    rv->setScreenTexture(byteArr);
    delete[] byteArr;
    // End Deluxe temp--------------------------------------------------------------------------------------------------

    while(!stopRendering) {
        //update currentTime
        std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
        previousTime = currentTime;
        currentTime = timeSinceStart.count();
        deltaTime = currentTime - previousTime;
        // render to window
        rv->render();

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

    delete rv;

    labhelper::shutDown(g_window);

	return 0;
}
