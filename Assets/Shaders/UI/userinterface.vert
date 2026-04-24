#version 460 core

layout(location = 0) in vec2 a_Position;   // unit quad: (0,0) to (1,1)
layout(location = 1) in vec2 a_TexCoord;   // UV: (0,0) to (1,1)
layout(location = 2) in vec4 a_Color;   // UV: (0,0) to (1,1)

uniform mat4 u_Projection;   // world-space ortho from GetProjectionMatrix()
uniform mat4 u_Transform;    // per-tile: translate(worldX, worldY) * scale(256, 256)

out vec2 v_TexCoord;

void main()
{
    v_TexCoord  = a_TexCoord;
    gl_Position = u_Projection * u_Transform * vec4(a_Position, 0.0, 1.0);
}
