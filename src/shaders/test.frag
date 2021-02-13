#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;
in vec2 texCoord;

layout (binding = 0) uniform sampler2D screenTex;

void main() 
{
	vec4 texColor = texture(screenTex, texCoord);
	fragmentColor = vec4(texColor.rgb, 1.f);
}


