//
// Created by isaaklindgren on 2021-03-18.
//

#include "Gui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <optional>
#include "FileHelper.h"
#include "imgui_impl_sdl_gl3.h"
#include "Application.h" // TODO: Gui and application should not both depend on eachother.

/**
 * Constructor
 */
Gui::Gui() {
    displayEditControls = false;
    displayFileDialog = false;
    displayToolbar = false;

    typing = false;
}

/**
 */
void Gui::init(SDL_Window *window) {
    ImGui_ImplSdlGL3_Init(window);
}

/**
 */
void Gui::draw_gui(SDL_Window *window) {
    // Inform imgui of new frame
    ImGui_ImplSdlGL3_NewFrame(window);

    if (displayToolbar) { toolbar(); }
    if (displayEditControls) { showEditControls(); }
    if (displayFileDialog) { showFileDialog(); }
    ImGui::Render();
}

/**
 * Cleans up after ImGui.
 */
void Gui::terminate(SDL_Window *window) {
    ImGui_ImplSdlGL3_NewFrame(window); // If newframe is not ever run before shut down we crash.
    ImGui_ImplSdlGL3_Shutdown();
}

/**
 */
void Gui::handleInput(SDL_Event event) {
    ImGui_ImplSdlGL3_ProcessEvent(&event);
}

/**
 * Toggles the toolbar.
 */
 void Gui::toggleToolbar() {
     this->displayToolbar = !this->displayToolbar;
     if (!this->displayToolbar) {
         // Disable all widgets
         this->displayEditControls = false;
         this->displayFileDialog = false;
     }
 }

int Gui::savekeybind(){
    ImGuiIO& io = ImGui::GetIO();

    // If we poll events here they will not be catched by application so we need to solve this some
    // other way. One less efficient way is:

    // TODO we would want to display the keys that are binded and I'm not sure that is done with
    //  SDL_Keycode that is really just an int. We also probably want a Settings struct we can
    //  send in to this class so it can be modified here and interpreted by Application. We could
    //  probably also use lambdas to accomplish this.

    for (int i = 0; i < 512; i++) {
        if (io.KeysDown[i]) {
            return i;
        }
    }

    return -1;


    /*while(Running)
    {
        while( SDL_PollEvent(&event) )
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    std::cout<<"Keybound to: "<< event.key.keysym.sym <<std::endl;
                    this->handleInput(event);
                    return event.key.keysym.sym;

                default:
                    -1;
            };

        }
    }*/

}

void Gui::showEditControls() {
    typing = true;
    ImGui::Begin("Controls", &displayEditControls);
    ImGui::Text("Press a ");
    static char buf[2] = {CONTROLLER_UP};
    ImGui::InputText("", buf, IM_ARRAYSIZE(buf));
    if(ImGui::IsItemActive()){
        //TODO might be here where we assign new keybinds.
    }
    ImGui::End();
}

void Gui::showFileDialog() {
    ImGui::Begin("Load ROM", &displayFileDialog);
    auto parentPath = FileHelper::getParentDir("..");
    if (parentPath != std::nullopt) {
        ImGui::Text(parentPath->absolutePath.c_str());
    }
    ImGui::End();
}

void Gui::toolbar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load ROM", "")) {
                displayFileDialog = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Edit", "")) {}

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::BeginMenu("Play speed")) {
                float speed = 0.0f;
                for (int i = 0; i < 4; i++) {
                    speed += 0.5f;
                    if (ImGui::MenuItem("Speed: ")) {
                        //TODO actually set play speed to something. Settings struct or lambdas.
                    }
                    ImGui::SameLine();
                    ImGui::Text(i % 2 == 0 ? "%.1f" : "%.0f", speed);
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Controller")) {
                displayEditControls = true;
            }
            ImGui::EndMenu();

        }
        ImGui::EndMainMenuBar();
    }
}