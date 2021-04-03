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
#include <memory>

#include "Palette.h"
#include "ErrorReport.h"
#include "gameboy/Definitions.h"

class RenderView {
public:
    RenderView();

    void initGL();
    void render() const;
    void clear() const;
    void setScreenTexture(uint8_t textureData[]);
    void setPalette(Palette palette);
    void setScreenMultiplier(int screenMultiplier);

    int getWidth() const;
    int getHeight() const;

private:
    const static int VERTEX_AMOUNT = 6;

    int screenMultiplier;
    glm::vec2 screenVertices[VERTEX_AMOUNT];
    GLuint vertexArrayObject;
    GLuint screenTexture;
    GLuint renderShaderProgram;
    GLuint fxShaderProgram;
    Palette palette;

    GLuint loadShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
    void glError();
    [[nodiscard]] const std::string getShaderErrorLog(GLuint shader) const;
    [[nodiscard]] const std::string getProgramErrorLog(GLuint program) const;
};
