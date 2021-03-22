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
Gui::Gui(AppSettings * settings) {
    this->settings = settings;
    displayEditControls = false;
    displayFileDialog = false;
    displayToolbar = false;
    do_keybind =false;
    typing = false;
}

/**
 */
void Gui::init(SDL_Window *window) {
    ImGui_ImplSdlGL3_Init(window);
}

/**
 */
void Gui::handleGui(SDL_Window *window) {

    // Inform imgui of new frame
    ImGui_ImplSdlGL3_NewFrame(window);
    if (displayToolbar) { toolbar(); }
    if (displayEditControls) { showEditControls(); }
    if (displayFileDialog) { showFileDialog(); }

    ImGui::Render();
    SDL_GL_SwapWindow(window);
    if (do_keybind) {
        keyBind();
    }
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
         //TODO disable everything.
     }
 }

void Gui::showEditControls() {
    ImGui::Begin("Controls", &displayEditControls);
    for (int i = 0; i < this->settings->keyBinds.nControllers; i++) {
        ImGui::Spacing();
        ImGui::Text("%s", this->settings->keyBinds.controllerButtons[i]->action_description.c_str());
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        ImVec2 vec(ImGui::GetWindowSize().x*0.25f,ImGui::GetWindowSize().y*0.05f);
        ImGui::SameLine(ImGui::GetWindowSize().x*0.5f,0);
        if (ImGui::Button(this->settings->keyBinds.controllerButtons[i]->keybind.c_str(),vec)) {
            showKeyBind(this->settings->keyBinds.controllerButtons[i]->action_description.c_str());
            keybindindex = i;
        }
    }
    ImGui::End();
}

void Gui::showFileDialog() {
    ImGui::Begin("Load ROM", &displayFileDialog);
    auto parentPath = FileHelper::getCurrentDir(settings->defaultPath);
    if (parentPath != std::nullopt) {
        ImGui::Text("%s",parentPath->absolutePath.c_str());
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
            if (ImGui::MenuItem("Keybinds")) {
                displayEditControls = !displayEditControls;
            }
            ImGui::EndMenu();

        }
        ImGui::EndMainMenuBar();
    }
}


void Gui::keyBind() {
    SDL_Event event;
    bool a = false;
    bool b = false;
    while (!(b&a)) {
        while (SDL_PollEvent(&event)) {
            bool keytaken=false;
            for(int i=0;i< this->settings->keyBinds.nControllers;i++){
                if(i!=keybindindex)
                    keytaken |= this->settings->keyBinds.controllerButtons[i]->keyval == event.key.keysym.sym;
            }
            if(!keytaken){
            if (event.type == SDL_KEYDOWN) {
                this->settings->keyBinds.controllerButtons[keybindindex]->keyval = event.key.keysym.sym;
                a=true;
            };
            if(event.type == SDL_TEXTINPUT){
                this->settings->keyBinds.controllerButtons[keybindindex]->keybind = event.text.text;
                b=true;
            }
            }

        }
    }
    do_keybind = false;
}

void Gui::showKeyBind(const char *buttonName) {
    ImGuiStyle& style = ImGui::GetStyle();
    std::string buf("Press a button to register a new keybind for ");
    buf.append(buttonName);
    ImGui::Begin("Keybind");
    ImGui::SetWindowFontScale(1.3);
    ImGui::SameLine(ImGui::GetWindowSize().x*0.05,0);
    ImGui::SetCursorPosY(ImGui::GetWindowHeight()*0.3f);
    ImGui::Text("%s", buf.c_str());
    ImGui::End();
    do_keybind = true;
}







