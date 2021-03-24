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
    displayEditControls = false;
    displayFileDialog = false;
    displayToolbar = false;
    do_keybind =false;
    typing = false;
}

/**
 */
void Gui::init(SDL_Window *window,SDL_GLContext *glContext, char * glsl_version) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    this->io = &ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

/**
 */
void Gui::handleGui(SDL_Window *window) {
    //Inits new frame
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
    //TODO move to application
    SDL_GL_SwapWindow(window);
    if (do_keybind) {
        keyBind();
    }
}

/**
 * Cleans up after ImGui.
 */
void Gui::terminate(SDL_Window *window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

/**
 */
void Gui::handleInput(SDL_Event event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
   // ImGui::CaptureKeyboardFromApp(true);

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

    for(int i =0;i<512;i++) {
        if (io->KeysDown[i]) {
            bool keytaken = false;
            for (int j = 0; j < this->settings->keyBinds.nControllers; j++) {
                if (j != keybindindex)
                    keytaken |= this->settings->keyBinds.controllerButtons[j]->keyval == SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i));
            }
            if (!keytaken) {
                this->settings->keyBinds.controllerButtons[keybindindex]->keybind = SDL_GetScancodeName(
                        static_cast<SDL_Scancode>(i));
                this->settings->keyBinds.controllerButtons[keybindindex]->keyval = SDL_GetKeyFromScancode(
                        static_cast<SDL_Scancode>(i));
                this->do_keybind = false;

            }
        }
    }
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







