#include "RenderView.h" // implements

using namespace glm;

RenderView::RenderView(int screenMultiplier, Palette palette) {
    this->screenMultiplier = screenMultiplier;
    this->palette = palette;
}

RenderView::RenderView() : RenderView(2, PALETTE_DMG) {}

void RenderView::initGL() {
    // Initialize screen quad.
    this->screenVertices[0] = {-1.0f, -1.0f};
    this->screenVertices[1] = {1.0f, -1.0f};
    this->screenVertices[2] = {1.0f, 1.0f};
    this->screenVertices[3] = {-1.0f, -1.0f};
    this->screenVertices[4] = {1.0f, 1.0f};
    this->screenVertices[5] = {-1.0f, 1.0f};

    this->vertexArrayObject = 0;
    glGenVertexArrays(1, &this->vertexArrayObject);
    labhelper::createAddAttribBuffer(this->vertexArrayObject,
                                     this->screenVertices,
                                     sizeof(this->screenVertices),
                                     0,
                                     2,
                                     GL_FLOAT);

    // Initialize shader programs, if renderShaderProgram is 0, the shader program could not
    // be loaded.
    this->renderShaderProgram = labhelper::loadShaderProgram("../src/shaders/test.vert",
                                                             "../src/shaders/test.frag",
                                                             false);
    this->fxShaderProgram = 0; // TO-DO Implement post process fx
}

void RenderView::render() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, LCD_WIDTH * this->screenMultiplier, LCD_HEIGHT * this->screenMultiplier);
    glClearColor(0., 0., 0., 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(this->renderShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->screenTexture);

    glUniform3f(glGetUniformLocation(this->renderShaderProgram, "c1"),
                this->palette.c1.r, this->palette.c1.g, this->palette.c1.b);
    glUniform3f(glGetUniformLocation(this->renderShaderProgram, "c2"),
                this->palette.c2.r, this->palette.c2.g, this->palette.c2.b);
    glUniform3f(glGetUniformLocation(this->renderShaderProgram, "c3"),
                this->palette.c3.r, this->palette.c3.g, this->palette.c3.b);
    glUniform3f(glGetUniformLocation(this->renderShaderProgram, "c4"),
                this->palette.c4.r, this->palette.c4.g, this->palette.c4.b);

    glBindVertexArray(this->vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_AMOUNT);
    glBindVertexArray(0);
}

void RenderView::setScreenTexture(uint8_t textureData[]) {
    glGenTextures(1, &this->screenTexture);
    glBindTexture(GL_TEXTURE_2D, this->screenTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 LCD_WIDTH,
                 LCD_HEIGHT,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 textureData);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderView::setPalette(Palette palette) {
    this->palette = palette;
}

void RenderView::setScreenMultiplier(int screenMultiplier) {
    this->screenMultiplier = screenMultiplier;
}

int RenderView::getWidth() const {
    return LCD_WIDTH * this->screenMultiplier;
}

int RenderView::getHeight() const {
    return LCD_HEIGHT * this->screenMultiplier;
}