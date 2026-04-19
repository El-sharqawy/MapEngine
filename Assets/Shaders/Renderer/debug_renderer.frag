#version 460 core

layout (location = 0) out vec4 v4FragColor;

in vec3 v3Position;
in vec4 v4Color;

void main()
{
	v4FragColor = v4Color;
}
