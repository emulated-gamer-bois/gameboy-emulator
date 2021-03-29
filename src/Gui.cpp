//
// Created by isaaklindgren on 2021-03-18.
//

#include "Gui.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <sstream>

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
/**
 * Constructor
 */
Gui::Gui(AppSettings* settings) {
    this->settings = settings;

    selectedFile = -1;

    disableWidgets();
    displayToolbar = true;
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

    //Call ImGui
    if (displayToolbar) { showToolbar(); }
    if (displayEditControls) { showEditControls(); }
    if (displayFileDialog) { showFileDialog(); }

    //Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (waitingForKeyBind) { keyBind(); }
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
 * Toggles the showToolbar.
 */
void Gui::toggleGui() {
    displayToolbar = !displayToolbar;
    if (!displayToolbar) {
        // Disable all widgets
        disableWidgets();
    }
}

void Gui::setLoadRomCallback(std::function<void(std::string)>&& loadRomCallback) {
    this->loadRomCallback = loadRomCallback;
}

void Gui::showEditControls() {
    //ImGui::SetNextWindowSize(ImGui::GetWindowSize(), ImGuiCond_Once);
    ImGui::Begin("Controls", &displayEditControls, windowFlags);
    for (int i = 0; i < settings->keyBinds.keybinds.capacity(); i++) {
        if (i == KEY_INDEX_JOYPAD_START) {
            ImGui::Text("JoyPad Keys ");
            ImGui::Spacing();
        } else if (i == KEY_INDEX_SPECIAL_START) {
            ImGui::Text("Special Keys ");
            ImGui::Spacing();
        }

        ImGui::Indent(indentSpace);
        ImGui::Text("%s", settings->keyBinds.keybinds[i]->action_description.c_str());
        ImGui::SameLine(130,0);

        if(waitingForKeyBind && keyBindIndex == i) {
            ImGui::PushStyleColor( ImGuiCol_Button, pressColor );
        } else {
            ImGui::PushStyleColor( ImGuiCol_Button, releaseColor );
        }

        ImVec2 buttonSize(90, 18);
        if (ImGui::Button(settings->keyBinds.keybinds[i]->keybind.c_str(), buttonSize)) {
            keyBindIndex = i;
            waitingForKeyBind = true;
        }
        ImGui::PopStyleColor(1);
        ImGui::Unindent(indentSpace);

        ImGui::Spacing();
    }
    ImGui::End();
}

void Gui::showFileDialog() {
    float fileSelectAreaHeight = 150.f;
    bool loadRom = false;

    ImGui::Begin("Load ROM", &displayFileDialog, windowFlags);

    // Display Current Directory
    ImGui::Text("Current Directory ");
    ImGui::Indent(indentSpace);
    ImGui::Text(fileExplorer.getCurrentDir().absolutePath.c_str());
    ImGui::Unindent(indentSpace);
    ImGui::Spacing();

    const std::vector<FileEntry>& fileEntryList = fileExplorer.getDirContents();

    // Display File Select
    ImGui::Text("File Select ");
    ImGui::Indent(indentSpace);
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
                    } else {
                        loadRom = true;
                    }
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
    ImGui::Unindent(indentSpace);
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
        loadRom = true;
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

    if (loadRom) {
        try {
            loadRomCallback(fileEntryList.at(selectedFile).absolutePath);
        } catch(...) {
            std::cerr << "GUI error: something went wrong with the load rom callback." << std::endl;
        }
    }
}

void Gui::showToolbar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load ROM", "")) {
                displayFileDialog = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            displayPlaySpeed();
            if (ImGui::MenuItem("Key Binds")) {
                displayEditControls = !displayEditControls;
            }
            ImGui::EndMenu();

        }
        ImGui::EndMainMenuBar();
    }
}

void Gui::displayPlaySpeed(){
     //TODO allow for slowing down the play speed.
    if (ImGui::BeginMenu("Emulation speed")) {
        float speed = MIN_EMULATION_SPEED_FLOAT;
        while (speed <= MAX_EMULATION_SPEED_FLOAT) {
            std::stringstream ss;
            ss << speed << 'x';
            if (ImGui::MenuItem(ss.str().c_str())) {
                if (speed < 1) {
                    std::cerr << "Gui Error: Slower speeds not implemented yet." << std::endl;
                } else {
                    settings->emulationSpeedMultiplier = speed;
                }
            }

            speed = speed * 2;
        }
        ImGui::EndMenu();
    }
 }

void Gui::keyBind() {
    if(settings->keyBinds.editKeyBinds(ImGui::GetIO().KeysDown, keyBindIndex)){
        waitingForKeyBind=false;
    }
}

void Gui::disableWidgets() {
    displayEditControls = false;
    displayFileDialog = false;
    displayToolbar = false;
    waitingForKeyBind =false;
}








