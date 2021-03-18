//
// Created by isaaklindgren on 2021-03-18.
//

#include "Gui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include "FileHelper.h"
#include "imgui_impl_sdl_gl3.h"
#include "Application.h"
void showEditControls(bool &show);

void toolbar();

bool show_edit_controls = false;

bool show_toolbar = true;

bool typing = false;

Gui::Gui() {
    io = ImGui::GetIO();

}


/**
 * Cleans up after ImGui.
 */
void Gui::terminate(SDL_Window *window) {
    ImGui_ImplSdlGL3_NewFrame(window); // If newframe is not ever run before shut down we crash.
    ImGui_ImplSdlGL3_Shutdown();
}

void Gui::gui(SDL_Window *window) {
    // Inform imgui of new frame
    ImGui_ImplSdlGL3_NewFrame(window);

    if (show_toolbar) { toolbar(); }
    if (show_edit_controls) { showEditControls(show_edit_controls); }
    ImGui::Render();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            default:
                std::cout << event.text.type << std::endl;
        }
    }
}

void Gui::init(SDL_Window *window) {
    ImGui_ImplSdlGL3_Init(window);

}

void Gui::showEditControls(bool &show) {
    typing = true;
    ImGui::Begin("Controls", &show_edit_controls);
    ImGui::Text("Press a ");
    static char buf[2] = {CONTROLLER_UP};



    ImGui::InputText("", buf, IM_ARRAYSIZE(buf));
    ImGui::End();
}


void toolbar() {

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load ROM", "")) {
                FileHelper::getParentDir("..");
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
                        //TODO actually set play speed to something.
                    }
                    ImGui::SameLine();
                    ImGui::Text(i % 2 == 0 ? "%.1f" : "%.0f", speed);

                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Controller")) {
                show_edit_controls = !show_edit_controls;
            }
            ImGui::EndMenu();

        }
        ImGui::EndMainMenuBar();
    }
}

void Gui::handleInput(SDL_Event event) {
    ImGui_ImplSdlGL3_ProcessEvent(&event);
}
