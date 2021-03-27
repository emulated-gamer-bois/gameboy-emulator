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
    fileExplorer.setCurrentDir(settings->romPath);
    fileExplorer.setFilter(".gb");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
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
 void Gui::toggleGui() {
     this->displayToolbar = !this->displayToolbar;
     if (!this->displayToolbar) {
         // Disable all widgets
         disableWidgets();
     }
 }

void Gui::showEditControls() {

    ImGui::SetNextWindowSize(ImGui::GetWindowSize(), ImGuiCond_Once);
    ImGui::Begin("Controls", &displayEditControls);
    for (int i = 0; i < this->settings->keyBinds.keybinds.capacity(); i++) {
        ImGui::Spacing();
        ImGui::Text("%s", this->settings->keyBinds.keybinds[i]->action_description.c_str());
        ImGui::SameLine(ImGui::GetWindowSize().x*0.5f,0);
        if(waitingForKeyBind && keyBindIndex == i){
            ImGui::PushStyleColor( ImGuiCol_Button, pressColor );
        }
        else{
            ImGui::PushStyleColor( ImGuiCol_Button, releaseColor );
        }
        ImVec2 vec(ImGui::GetWindowSize().x*0.25f,ImGui::GetWindowSize().y*0.05f);
        if (ImGui::Button(this->settings->keyBinds.keybinds[i]->keybind.c_str(), vec)) {
            keyBindIndex = i;
            waitingForKeyBind = true;
        }
        ImGui::PopStyleColor(1);
    }
    ImGui::End();
}

void Gui::showFileDialog() {
    float fileSelectAreaHeight = 150.f;
    float indentSize = 20.f;

    int windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Load ROM", &displayFileDialog, windowFlags);

    // Display Current Directory
    ImGui::Text("Current Directory: ");
    ImGui::Indent(indentSize);
    ImGui::Text(fileExplorer.getCurrentDir().absolutePath.c_str());
    ImGui::Unindent(indentSize);
    ImGui::Spacing();

    const std::vector<FileEntry>& fileEntryList = fileExplorer.getDirContents();

    // Display File Select
    ImGui::Text("File Select: ");
    ImGui::Indent(indentSize);
    if (ImGui::BeginListBox("##FileList", ImVec2(350.f, fileSelectAreaHeight))) {
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

    // Display Rom Folder Buttons
    ImGui::SameLine();
    ImGui::BeginChild("RomFolderButtons", ImVec2(140, fileSelectAreaHeight), true);
    if (ImGui::Button("Set Rom Folder")) {
        settings->romPath = fileExplorer.getCurrentDir().absolutePath;
    }
    ImGui::Spacing();
    if (ImGui::Button("Go to Rom Folder")) {
        fileExplorer.setCurrentDir(settings->romPath);
    }
    ImGui::EndChild();
    ImGui::Unindent(indentSize);
    ImGui::Spacing();

    // Display Selected File
    ImGui::Text("Selected File: ");
    ImGui::SameLine();
    bool romSelected = !fileEntryList.empty() && selectedFile != -1 && !fileEntryList.at(selectedFile).isDir;
    if (romSelected) {
        ImGui::Text(fileEntryList.at(selectedFile).filename.c_str());
    } else {
        ImGui::Text("None");
    }
    ImGui::Spacing();

    // Control Buttons
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
     if(this->settings->keyBinds.editKeyBinds(ImGui::GetIO().KeysDown, keyBindIndex)){
         this->waitingForKeyBind=false;
     }
}

void Gui::disableWidgets() {
    displayEditControls = false;
    displayFileDialog = false;
    displayToolbar = false;
    waitingForKeyBind =false;
}








