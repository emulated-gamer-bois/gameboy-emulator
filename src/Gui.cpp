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

    selectedFile = -1;

    disableWidgets();
}

/**
 */
void Gui::init(SDL_Window *window,SDL_GLContext *glContext, char * glsl_version) {
    // File dialog init
    fileExplorer.setCurrentDir(settings->defaultPath);
    fileExplorer.setFilter(".gb");

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
    //ImGui::ShowDemoWindow();
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
    float indentSize = 20.f;

    ImGui::Begin("Load ROM", &displayFileDialog, ImGuiWindowFlags_NoResize);
    ImGui::Text("Current Directory: ");
    ImGui::Indent(indentSize);
    ImGui::Text(fileExplorer.getCurrentDir().absolutePath.c_str());
    ImGui::Unindent(indentSize);
    ImGui::Spacing();

    const std::vector<FileEntry>& fileEntryList = fileExplorer.getDirContents();

    ImGui::Text("File Select: ");
    ImGui::Indent(indentSize);
    if (ImGui::BeginListBox("##FileList")) {
        for (int i = 0; i < fileEntryList.size(); i++) {
            int flags = ImGuiSelectableFlags_AllowDoubleClick;
            bool isSelected = (selectedFile == i);
            if (ImGui::Selectable(fileEntryList.at(i).filename.c_str(), isSelected, flags)) {
                selectedFile = i;
                if (ImGui::IsMouseDoubleClicked(0)) {
                    if (fileEntryList.at(i).isDir) {
                        fileExplorer.moveTo(fileEntryList.at(i));
                        selectedFile = -1;
                    }
                    //else
                        // Load here
                }
            }
        }
        ImGui::EndListBox();
    }
    ImGui::Unindent(indentSize);
    ImGui::Spacing();

    ImGui::Text("Selected File: ");
    ImGui::SameLine();
    bool romSelected = !fileEntryList.empty() && selectedFile != -1 && !fileEntryList.at(selectedFile).isDir;
    if (romSelected) {
        ImGui::Text(fileEntryList.at(selectedFile).filename.c_str());
    } else {
        ImGui::Text("None");
    }
    ImGui::Spacing();

    if (!romSelected) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
    if (ImGui::Button("Load")) {

    }
    if (!romSelected) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        displayFileDialog = false;
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








