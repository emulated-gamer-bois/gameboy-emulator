//
// Created by isaaklindgren on 2021-03-18.
//

#include "Gui.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <optional>
#include <iostream>
#include "FileHelper.h"
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

    ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls", &displayEditControls);
    for (int i = 0; i < this->settings->keyBinds.keybinds.capacity(); i++) {
        ImGui::Spacing();
        ImGui::Text("%s", this->settings->keyBinds.keybinds[i]->action_description.c_str());
        ImGui::SameLine(ImGui::GetWindowSize().x*0.5f,0);
        if(waitingForKeyBind && keybindindex == i){
            ImGui::PushStyleColor( ImGuiCol_Button, pressColor );
        }
        else{
            ImGui::PushStyleColor( ImGuiCol_Button, releaseColor );
        }
        ImVec2 vec(ImGui::GetWindowSize().x*0.25f,ImGui::GetWindowSize().y*0.05f);
        if (ImGui::Button(this->settings->keyBinds.keybinds[i]->keybind.c_str(), vec)) {
            keybindindex = i;
            waitingForKeyBind = true;
        }
        ImGui::PopStyleColor(1);
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
            displayPlaySpeed();
            if (ImGui::MenuItem("Keybinds")) {
                displayEditControls = !displayEditControls;
            }
            ImGui::EndMenu();

        }
        ImGui::EndMainMenuBar();
    }
}
void Gui::displayPlaySpeed(){
     //TODO allow for slowing down the play speed.
    if (ImGui::BeginMenu("Play speed")) {
        for (int i = 0; i < settings->maxPlaySpeed; i++) {

            if (ImGui::MenuItem("Speed: ")) {
                settings->setPlaySpeed(i+1);
            }
            ImGui::SameLine();
            ImGui::Text("%d",i+1);
        }
        ImGui::EndMenu();
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








