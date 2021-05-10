/**
 * Source code for shaders used in this application.
 */

#pragma once

#include <string>

const std::string paletteFrag = "#version 420\n"
                                "layout(location = 0) out vec4 fragmentColor;\n"
                                "in vec2 texCoord;\n"
                                "layout (binding = 0) uniform sampler2D screenTex;\n"
                                "const float C1_MAX = 0.25f;\n"
                                "const float C2_MAX = 0.50f;\n"
                                "const float C3_MAX = 0.75f;\n"
                                "uniform vec3 c1;\n"
                                "uniform vec3 c2;\n"
                                "uniform vec3 c3;\n"
                                "uniform vec3 c4;\n"
                                "void main() \n"
                                "{\n"
                                "vec4 texColor = texture(screenTex, texCoord);\n"
                                "vec3 color = texColor.rrr;\n"
                                "if (texColor.r <= C1_MAX) color = c1;\n"
                                "else if (texColor.r <= C2_MAX) color = c2;\n"
                                "else if (texColor.r <= C3_MAX) color = c3;\n"
                                "else color = c4;\n"
                                "fragmentColor = vec4(color, 1.f);\n"
                                "}";

const std::string paletteVert = "#version 420\n"
                                "layout(location = 0) in vec2 position;\n"
                                "out vec2 texCoord;\n"
                                "void main() \n"
                                "{\n"
                                "gl_Position = vec4(position, 0.0, 1.0);\n"
                                "texCoord = 0.5 * (position + vec2(1,1));\n"
                                "texCoord.y = 1 - texCoord.y;\n"
                                "}";
