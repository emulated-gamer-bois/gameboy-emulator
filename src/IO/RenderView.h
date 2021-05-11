
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../application/AppSettings.h"
#include "PaletteHandler.h"
/**
 * This class handles the open gl integration in the emulator. The texture data sent to this class is processed and
 * rendered.
 */

class RenderView {
public:
    explicit RenderView(AppSettings& settings, PaletteHandler& paletteHandler);

    /**
     * Initialises openGl.
     */
    void initGL();

    /**
     * Renders the current texture.
     */
    void render() const;

    /**
     * Clears the rendered screen.
     */
    void clear() const;

    /**
     * Sets the current texture of this view. The texture is a one channel bitmap.
     *
     * @param textureData
     */
    void setScreenTexture(uint8_t textureData[]);

    /**
     * Sets the position of the viewport.
     *
     * @param x
     * @param y
     */
    void setViewportPos(int x, int y);

    /**
     * Sets the dimensions of the viewport.
     *
     * @param width
     * @param height
     */
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
    [[nodiscard]] std::string getShaderErrorLog(GLuint shader) const;
    [[nodiscard]] std::string getProgramErrorLog(GLuint program) const;
};
