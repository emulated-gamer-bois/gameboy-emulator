/*
 * RenderView
 * This class handles the rendering of the Game Boy lcd.
 *
 * Author: Andreas Palmqvist
 */

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <memory>

#include "Palette.h"
#include "../application/AppSettings.h"
#include "../helpers/ErrorReport.h"
#include "../gameboy/Definitions.h"
#include "PaletteHandler.h"

class RenderView {
public:
    explicit RenderView(AppSettings& settings, PaletteHandler& paletteHandler);

    void initGL();
    void render() const;
    void clear() const;
    void setScreenTexture(uint8_t textureData[]);
    void setViewportPos(int x, int y);
    void setViewportDim(int width, int height);

private:
    const static int VERTEX_AMOUNT = 6;

    AppSettings& settings;
    PaletteHandler& paletteHandler;

    glm::vec2 screenVertices[VERTEX_AMOUNT];
    int x;
    int y;
    int width;
    int height;
    GLuint vertexArrayObject;
    GLuint screenTexture;
    GLuint renderShaderProgram;
    GLuint fxShaderProgram;

    GLuint loadShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);

    bool glErrorFound(std::string& errorLog) const;
    [[nodiscard]] const std::string getShaderErrorLog(GLuint shader) const;
    [[nodiscard]] const std::string getProgramErrorLog(GLuint program) const;
};
