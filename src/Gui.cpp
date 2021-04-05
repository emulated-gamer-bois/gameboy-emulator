//
// Created by isaaklindgren on 2021-03-18.
//

#include "Gui.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <sstream>
#include <utility>

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
/**
 * Constructor
 */

Gui::Gui(std::shared_ptr<AppSettings> settings, std::shared_ptr<PaletteHandler> paletteHandler) {
    this->settings = std::move(settings);
    this->paletteHandler = std::move(paletteHandler);
    selectedFile = -1;
    // TODO get from settings probably
    selectedPalette = 0;
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
    if (displayPaletteSettings) { showPaletteSettings(); }

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

void Gui::setChangePaletteCallback(std::function<void(int)>&& changePaletteCallback) {
    this->changePaletteCallback = changePaletteCallback;
}

void Gui::showEditControls() {
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
    if (ImGui::BeginListBox("##FileList", ImVec2(listBoxWidth, listBoxHeight))) {
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
    ImGui::BeginChild("RomFolderButtons", ImVec2(140, listBoxHeight), true);
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
            FATAL_ERROR("Could not call loadRomCallback.");
        }
    }
}

void Gui::showPaletteSettings() {
    bool changeColor = false;

    ImGui::Begin("Palette Settings", &displayPaletteSettings, windowFlags);
    // Display File Select
    ImGui::Text("Palette Select ");
    ImGui::Indent(indentSpace);
    if (ImGui::BeginListBox("##PaletteList", ImVec2(listBoxWidth, listBoxHeight))) {
        for (int i = 0; i < paletteHandler->getPaletteAmount(); i++) {
            int flags = ImGuiSelectableFlags_AllowDoubleClick;
            bool isSelected = (selectedPalette == i);
            if (ImGui::Selectable(paletteHandler->getPaletteName(i).c_str(), isSelected, flags)) {
                selectedPalette = i;
                if (ImGui::IsMouseDoubleClicked(0)) {
                    changeColor = true;
                }
            }
        }
        ImGui::EndListBox();
    }

    Palette temp = paletteHandler->getPalette(selectedPalette);
    ImVec4 c1 = ImVec4(temp.c1.r, temp.c1.g, temp.c1.b, 1.f);
    ImVec4 c2 = ImVec4(temp.c2.r, temp.c2.g, temp.c2.b, 1.f);
    ImVec4 c3 = ImVec4(temp.c3.r, temp.c3.g, temp.c3.b, 1.f);
    ImVec4 c4 = ImVec4(temp.c4.r, temp.c4.g, temp.c4.b, 1.f);
    ImGui::ColorButton("##c1", c1, ImGuiColorEditFlags_NoBorder, ImVec2(87.5f, 30.f));
    ImGui::SameLine(0.f, 0.f);
    ImGui::ColorButton("##c2", c2, ImGuiColorEditFlags_NoBorder, ImVec2(87.5f, 30.f));
    ImGui::SameLine(0.f, 0.f);
    ImGui::ColorButton("##c3", c3, ImGuiColorEditFlags_NoBorder, ImVec2(87.5f, 30.f));
    ImGui::SameLine(0.f, 0.f);
    ImGui::ColorButton("##c4", c4, ImGuiColorEditFlags_NoBorder, ImVec2(87.5f, 30.f));

    ImGui::End();

    if (changeColor) {
        try {
            changePaletteCallback(selectedPalette);
        } catch(...) {
            FATAL_ERROR("Could not call changePaletteCallback.");
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
            if (ImGui::MenuItem("Palette Settings")) {
                displayPaletteSettings = !displayPaletteSettings;
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
                    settings->emulationSpeedMultiplier = speed;
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
    displayPaletteSettings = false;
    displayToolbar = false;
    waitingForKeyBind =false;
}












