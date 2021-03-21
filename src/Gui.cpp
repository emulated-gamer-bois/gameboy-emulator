//
// Created by isaaklindgren on 2021-03-18.
//

#include "Gui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <array>
#include "FileHelper.h"
#include "imgui_impl_sdl_gl3.h"

void showEditControls(bool &show);

void toolbar();

bool show_edit_controls = false;
bool show_toolbar = true;
bool do_keybind = false;
SDL_Window *win;
Gui::Gui(Keybinds * controller) {
    this->controller = controller;
}
/**
 * Cleans up after ImGui.
 */
void Gui::terminate(SDL_Window *window) {
    ImGui_ImplSdlGL3_NewFrame(window); // If newframe is not ever run before shut down we crash.
    ImGui_ImplSdlGL3_Shutdown();
}

void Gui::draw_gui(SDL_Window *window) {

    // Inform imgui of new frame
    ImGui_ImplSdlGL3_NewFrame(window);
    if (show_toolbar) { toolbar(); }
    if (show_edit_controls) { showEditControls(); }
    ImGui::Render();
    SDL_GL_SwapWindow(window);
    if (do_keybind) {
        keyBind();
    }
}

void Gui::init(SDL_Window *window) {
    win = window;
    ImGui_ImplSdlGL3_Init(window);

}

void Gui::showEditControls() {
    ImGui::Begin("Controls", &show_edit_controls);
    for (int i = 0; i < this->controller->nControllers; i++) {
        ImGui::Text("%s", this->controller->controllerButtons[i]->action_description.c_str());
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (ImGui::Button(this->controller->controllerButtons[i]->keybind.c_str())) {
            showKeyBind(this->controller->controllerButtons[i]->action_description.c_str());
            keybindindex = i;
        }
    }
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
            if (ImGui::MenuItem("Keybinds")) {
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

void Gui::keyBind() {
    SDL_Event event;
    bool a = false;
    bool b = false;
    while (!(b&a)) {
        while (SDL_PollEvent(&event)) {
            bool keytaken=false;
            for(int i=0;i< this->controller->nControllers;i++){
                if(i!=keybindindex)
                    keytaken |= this->controller->controllerButtons[i]->keyval == event.key.keysym.sym;
            }
            if(!keytaken){
            if (event.type == SDL_KEYDOWN) {
                this->controller->controllerButtons[keybindindex]->keyval = event.key.keysym.sym;
                a=true;
            };
            if(event.type == SDL_TEXTINPUT){
                this->controller->controllerButtons[keybindindex]->keybind = event.text.text;
                b=true;
            }
            }

        }
    }
    do_keybind = false;
}

void Gui::showKeyBind(const char *buttonName) {
    std::string buf("Press a button to register a new keybind for ");
    buf.append(buttonName);
    ImGui::Begin("Keybind");
    ImGui::Text("%s", buf.c_str());
    ImGui::End();
    do_keybind = true;
}







