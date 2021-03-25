//
// Created by isaaklindgren on 2021-03-18.
//

#include "Gui.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
/**
 * Constructor
 */
Gui::Gui(AppSettings * settings) {
    this->settings = settings;
    disableWidgets();
}

/**
 */
void Gui::init(SDL_Window *window,SDL_GLContext *glContext, char * glsl_version) {
    fileExplorer.setCurrentDir(settings->defaultPath);
    fileExplorer.setFilter(".[a-z0-9]*");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

/**
 */
void Gui::handleGui(SDL_Window *window) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
    //Call ImGui
    if (displayToolbar) { toolbar(); }
    if (displayEditControls) { showEditControls(); }
    if (displayFileDialog) { showFileDialog(); }
    //Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (waitingForKeyBind) {
        keyBind();
    }
}

/**
 * Cleans up after ImGui.
 */
void Gui::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

/**
 */
void Gui::handleInput(SDL_Event event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}

/**
 * Toggles the toolbar.
 */
 void Gui::toggleToolbar() {
     this->displayToolbar = !this->displayToolbar;
     if (!this->displayToolbar) {
         // Disable all widgets
         disableWidgets();
     }
 }

void Gui::showEditControls() {
    ImGui::Begin("Controls", &displayEditControls);
    for (int i = 0; i < this->settings->keyBinds.controllerButtons.capacity(); i++) {
        ImGui::Spacing();
        ImGui::Text("%s", this->settings->keyBinds.controllerButtons[i]->action_description.c_str());
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        ImVec2 vec(ImGui::GetWindowSize().x*0.25f,ImGui::GetWindowSize().y*0.05f);
        ImGui::SameLine(ImGui::GetWindowSize().x*0.5f,0);
        if(waitingForKeyBind && keybindindex == i){
            ImGui::PushStyleColor( ImGuiCol_Button, pressColor );
        }
        else{
            ImGui::PushStyleColor( ImGuiCol_Button, releaseColor );
        }
        if (ImGui::Button(this->settings->keyBinds.controllerButtons[i]->keybind.c_str(),vec)) {
            keybindindex = i;
            waitingForKeyBind = true;
        }
        ImGui::PopStyleColor(1);
    }
    ImGui::End();
}

void Gui::showFileDialog() {
    ImGui::Begin("Load ROM", &displayFileDialog);
    ImGui::Text(fileExplorer.getCurrentDir().c_str());
    if (ImGui::BeginListBox("nut")) {
        ImGui::EndListBox();
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
                        settings->setPlaySpeed(speed);
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
     if(this->settings->keyBinds.editKeyBinds(ImGui::GetIO().KeysDown,keybindindex)){
         this->waitingForKeyBind=false;
     }
}

void Gui::disableWidgets() {
    displayEditControls = false;
    displayFileDialog = false;
    displayToolbar = false;
    waitingForKeyBind =false;
}








