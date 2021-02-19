/*
 * RenderView
 * This class handles the rendering of the Game Boy lcd.
 *
 * Author: Andreas Palmqvist
 */

#ifndef LAME_BOY_RENDERVIEW_H
#define LAME_BOY_RENDERVIEW_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <labhelper.h>

using namespace glm;

class RenderView {
private:
    const static int GAME_BOY_LCD_WIDTH = 160;
    const static int GAME_BOY_LCD_HEIGHT = 144;
    const static int VERTEX_AMOUNT = 6;

    int screenMultiplier;
    vec2 screenVertices[VERTEX_AMOUNT];
    GLuint vertexArrayObject;
    GLuint renderShaderProgram;
    GLuint fxShaderProgram;

public:
    RenderView(int screenMultiplier);
    RenderView();
    virtual ~RenderView();

    void render();
};

#endif //LAME_BOY_RENDERVIEW_H
