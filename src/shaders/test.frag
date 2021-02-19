#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;
in vec2 texCoord;

void main() 
{
	vec2 uv = texCoord;
	fragmentColor = vec4(uv.x, uv.y, 0.5f, 1.f);
}


