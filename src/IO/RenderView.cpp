#include "RenderView.h" // implements
#include <fstream>

using namespace glm;

RenderView::RenderView(AppSettings& settings, PaletteHandler& paletteHandler):
settings{settings}, paletteHandler{paletteHandler} {}

void RenderView::initGL() {
    std::string glErrorLog;

    // Initialize screen quad.
    screenVertices[0] = {-1.0f, -1.0f};
    screenVertices[1] = {1.0f, -1.0f};
    screenVertices[2] = {1.0f, 1.0f};
    screenVertices[3] = {-1.0f, -1.0f};
    screenVertices[4] = {1.0f, 1.0f};
    screenVertices[5] = {-1.0f, 1.0f};

    vertexArrayObject = 0;
    glGenVertexArrays(1, &vertexArrayObject);

    // Create the VAO.
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
    if (glErrorFound(glErrorLog)) { FATAL_ERROR(glErrorLog); }

    glBindVertexArray(vertexArrayObject);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(0);
    if (glErrorFound(glErrorLog)) { FATAL_ERROR(glErrorLog); }

    // Initialize shader programs, if renderShaderProgram is 0, the shader program could not
    // be loaded.
    renderShaderProgram = loadShaderProgram("../src/shaders/palette.vert",
                                            "../src/shaders/palette.frag");
    fxShaderProgram = 0; // TODO Implement post process fx
    glGenTextures(1, &screenTexture);
}

void RenderView::render() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, LCD_WIDTH * settings.screenMultiplier, LCD_HEIGHT * settings.screenMultiplier);

    glUseProgram(renderShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);

    const Palette& palette = paletteHandler.getPalette(settings.paletteNumber);
    glUniform3f(glGetUniformLocation(renderShaderProgram, "c1"),
                palette.c1.r, palette.c1.g, palette.c1.b);
    glUniform3f(glGetUniformLocation(renderShaderProgram, "c2"),
                palette.c2.r, palette.c2.g, palette.c2.b);
    glUniform3f(glGetUniformLocation(renderShaderProgram, "c3"),
                palette.c3.r, palette.c3.g, palette.c3.b);
    glUniform3f(glGetUniformLocation(renderShaderProgram, "c4"),
                palette.c4.r, palette.c4.g, palette.c4.b);

    glBindVertexArray(vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_AMOUNT);
    glBindVertexArray(0);
}

void RenderView::clear() const {
    glClearColor(0., 0., 0., 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderView::setScreenTexture(uint8_t textureData[]) {
    std::string glErrorLog;

    // Free previous texture. It is the only gl resource that needs to be deleted continuously in this code since
    // the texture is replaced so often.
    if (screenTexture != 0) {
        glDeleteTextures(1, &screenTexture);
    }

    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
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
    if (glErrorFound(glErrorLog)) { FATAL_ERROR(glErrorLog); }
}

GLuint RenderView::loadShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {
    std::string glErrorLog;

    // Create shader objects.
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load shader source codes.
    std::ifstream vsFile(vertexShader);
    std::string vsSrc((std::istreambuf_iterator<char>(vsFile)), std::istreambuf_iterator<char>());

    std::ifstream fsFile(fragmentShader);
    std::string fsSrc((std::istreambuf_iterator<char>(fsFile)), std::istreambuf_iterator<char>());

    // Set vertex shader and fragment shader source code.
    const char* vsTemp = vsSrc.c_str();
    const char* fsTemp = fsSrc.c_str();
    glShaderSource(vertShader, 1, &vsTemp, nullptr);
    glShaderSource(fragShader, 1, &fsTemp, nullptr);

    // For shader compilation error handling.
    GLint compiled = 0;

    // Compile vertex shader.
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) { FATAL_ERROR(getShaderErrorLog(vertShader)); }

    // Compile fragment shader.
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) { FATAL_ERROR(getShaderErrorLog(fragShader)); }

    // Create shader program and attach shaders.
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glDeleteShader(vertShader);
    glAttachShader(shaderProgram, fragShader);
    glDeleteShader(fragShader);
    if (glErrorFound(glErrorLog)) { FATAL_ERROR(glErrorLog); }

    // For shader linking error handling.
    GLint linked = 0;

    // Link here
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) { FATAL_ERROR(getProgramErrorLog(shaderProgram)); }

    return shaderProgram;
}

bool RenderView::glErrorFound(std::string& errorLog) const {
    std::stringstream ss;

    bool foundError = false;
    auto errorCode = glGetError();
    while (errorCode != GL_NO_ERROR) {
        foundError = true;

        auto* errorMessage = gluErrorString(errorCode);

        ss << "GL Error #" << errorCode << " - ";
        if (!errorMessage) {
            ss << "(No message)" << std::endl;
        } else {
            ss << errorMessage << std::endl;
        }

        errorCode = glGetError();
    }

    errorLog = ss.str();

    return foundError;
}

const std::string RenderView::getShaderErrorLog(GLuint shader) const {
    int logLength = 0;
    int charsWritten = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

    auto log = std::make_unique<char[]>(logLength);
    if(logLength > 0) {
        glGetShaderInfoLog(shader, logLength, &charsWritten, log.get());
    }

    std::string strLog = log.get();
    return strLog;
}

const std::string RenderView::getProgramErrorLog(GLuint program) const {
    int logLength = 0;
    int charsWritten = 0;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

    auto log = std::make_unique<char[]>(logLength);
    if(logLength > 0) {
        glGetProgramInfoLog(program, logLength, &charsWritten, log.get());
    }

    std::string strLog = log.get();
    return strLog;
}