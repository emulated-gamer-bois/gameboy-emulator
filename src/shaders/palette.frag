#version 420

layout(location = 0) out vec4 fragmentColor;
in vec2 texCoord;

layout (binding = 0) uniform sampler2D screenTex;

const float C1_MAX = 0.25f;
const float C2_MAX = 0.50f;
const float C3_MAX = 0.75f;

uniform vec3 c1;
uniform vec3 c2;
uniform vec3 c3;
uniform vec3 c4;

void main() 
{
	vec4 texColor = texture(screenTex, texCoord);

	vec3 color = texColor.rrr;
	if (texColor.r <= C1_MAX)
	{
		color = c1;
	}
	else if (texColor.r <= C2_MAX)
	{
		color = c2;
	}
	else if (texColor.r <= C3_MAX)
	{
		color = c3;
	}
	else
	{
		color = c4;
	}
	fragmentColor = vec4(color, 1.f);
}


