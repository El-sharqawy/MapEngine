#version 460 core

layout (location = 0) in vec3 m_v3Position;
layout (location = 1) in vec4 m_v4Color;

layout (std140, binding = 0) uniform CameraData
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
} camera;

uniform bool u_bRenderingPolyLines;
uniform mat4 u_Projection;
uniform vec2 u_CameraOrigin;  // ← NEW: top-left world pixel of screen

out vec3 v3Position;
out vec4 v4Color;

void main()
{
	if (u_bRenderingPolyLines)
	{
		vec3 screenPos = m_v3Position - vec3(u_CameraOrigin.xy, 0.0f);
		gl_Position = u_Projection * vec4(screenPos.xy, 0.0, 1.0);
	}
	else
	{
		gl_Position = camera.ViewProjection * vec4(m_v3Position, 1.0);
	}

	v3Position = m_v3Position;
	v4Color = m_v4Color;
}
