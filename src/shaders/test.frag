#version 420

layout(location = 0) out vec4 fragmentColor;
in vec2 texCoord;

layout (binding = 0) uniform sampler2D screenTex;

const float C1_MAX = 0.205f;
const float C2_MAX = 0.565f;
const float C3_MAX = 0.86f;

void main() 
{
	vec4 texColor = texture(screenTex, texCoord);

	vec3 color = texColor.rrr;
	if (texColor.r <= C1_MAX)
	{
		color = vec3(8/255.f, 24/255.f, 32/255.f);
	}
	else if (texColor.r <= C2_MAX)
	{
		color = vec3(52/255.f, 104/255.f, 86/255.f);
	}
	else if (texColor.r <= C3_MAX)
	{
		color = vec3(136/255.f, 192/255.f, 12/255.f);
	}
	else
	{
		color = vec3(224/255.f, 248/255.f, 208/255.f);
	}
	fragmentColor = vec4(color, 1.f);
}


