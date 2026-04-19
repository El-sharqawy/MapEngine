#version 460 core

layout (location = 0) in vec2 v2Pos;       // The Unit Quad (0.0 to 1.0)
layout (location = 1) in vec2 v2TexCoord;  // Texture Coordinates

out vec2 TexCoord;
out vec4 OverlayColor;

// Uniforms from SUICommand
uniform mat4 u_matProjection;   // Ortho matrix from SetInterfaceRenderState
uniform vec2 u_v2Position;      // m_v2GlobalPosition (Pixels)
uniform vec2 u_v2Size;          // m_v2Size (Pixels)
uniform vec4 u_v4Color;         // Tint color

void main()
{
    // 1. Scale the Unit Quad to the Widget's pixel size
    vec2 scaledPos = v2Pos * u_v2Size;

    // 2. Shift it to the Widget's global pixel position
    vec2 finalPixelPos = scaledPos + u_v2Position;

    // 3. Project from Pixels to OpenGL NDC (-1 to 1)
    // u_matProjection handles the math so (0,0) is Top-Left
    gl_Position = u_matProjection * vec4(finalPixelPos, 0.0, 1.0);
    
    // Pass data to Fragment Shader
    TexCoord = v2TexCoord;
    OverlayColor = u_v4Color;
}