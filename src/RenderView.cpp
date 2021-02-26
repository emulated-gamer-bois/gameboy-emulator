#include "RenderView.h" // implements

using namespace glm;

RenderView::RenderView(int screenMultiplier, Palette palette) {
    this->screenMultiplier = screenMultiplier;
    this->palette = palette;
}

RenderView::RenderView() : RenderView(4, PALETTE_POCKET) {}

void RenderView::initGL() {
    // Initialize screen quad.
    screenVertices[0] = {-1.0f, -1.0f};
    screenVertices[1] = {1.0f, -1.0f};
    screenVertices[2] = {1.0f, 1.0f};
    screenVertices[3] = {-1.0f, -1.0f};
    screenVertices[4] = {1.0f, 1.0f};
    screenVertices[5] = {-1.0f, 1.0f};

    vertexArrayObject = 0;
    glGenVertexArrays(1, &vertexArrayObject);
    labhelper::createAddAttribBuffer(vertexArrayObject,
                                     screenVertices,
                                     sizeof(screenVertices),
                                     0,
                                     2,
                                     GL_FLOAT);

    // Initialize shader programs, if renderShaderProgram is 0, the shader program could not
    // be loaded.
    renderShaderProgram = labhelper::loadShaderProgram("../src/shaders/test.vert",
                                                       "../src/shaders/test.frag",
                                                       false);
    fxShaderProgram = 0; // TO-DO Implement post process fx
}

void RenderView::render() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, GAME_BOY_LCD_WIDTH * screenMultiplier, GAME_BOY_LCD_HEIGHT * screenMultiplier);
    glClearColor(0., 0., 0., 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(renderShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);

    glUniform3f(glGetUniformLocation(renderShaderProgram, "c1"), palette.c1.r, palette.c1.g, palette.c1.b);
    glUniform3f(glGetUniformLocation(renderShaderProgram, "c2"), palette.c2.r, palette.c2.g, palette.c2.b);
    glUniform3f(glGetUniformLocation(renderShaderProgram, "c3"), palette.c3.r, palette.c3.g, palette.c3.b);
    glUniform3f(glGetUniformLocation(renderShaderProgram, "c4"), palette.c4.r, palette.c4.g, palette.c4.b);

    glBindVertexArray(vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_AMOUNT);
    glBindVertexArray(0);
}

void RenderView::setScreenTexture(uint8_t textureData[]) {
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 GAME_BOY_LCD_WIDTH,
                 GAME_BOY_LCD_HEIGHT,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 textureData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderView::setPalette(Palette palette) {
    this->palette = palette;
}

int RenderView::getWidth() const {
    return GAME_BOY_LCD_WIDTH * screenMultiplier;
}

int RenderView::getHeight() const {
    return GAME_BOY_LCD_HEIGHT * screenMultiplier;
}