#version 460 core

layout (location = 0) in vec3 m_v3Position;
layout (location = 1) in vec3 m_v3Normals;
layout (location = 2) in vec2 m_v2TexCoord;
layout (location = 3) in vec4 m_v4Color;

// #define MODERN_OPENGL_PATH

#ifdef MODERN_OPENGL_PATH
// BINDING 0: The "Eye" (Global Camera)
layout (std140, binding = 0) uniform CameraData
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
} camera;

// BINDING 1: The "World" (All Object Positions)
layout(std430, binding = 0) readonly buffer MatrixBuffer
{
    mat4 modelMatrices[]; // Leave empty [] to match the SSBO size automatically
    // Note: If on 4.3 but not 4.6, you might need an extension for gl_DrawID
    // or pass it as a vertex attribute.
};
#else
// --- Legacy Path: Uses standard Uniform ---
uniform mat4 u_ModelMatrix;
uniform struct CameraData
{
    mat4 View;
    mat4 Projection;
    mat4 ViewProjection;
} camera;
#endif

// Outputs to Fragment Shader
out vec3 v3Position;
out vec3 v3Normals;
out vec2 v2TexCoord;
out vec4 v4Color;

void main()
{
    // 1. Get the matrix for THIS patch
    mat4 model;
#ifdef MODERN_OPENGL_PATH
    model = modelMatrices[gl_DrawID];
#else
    model = mat4(1.0f);
#endif

    // 2. Calculate final screen position
    gl_Position = camera.ViewProjection * model * vec4(m_v3Position, 1.0);

    // 3. Pass data to Fragment Shader
    v3Position = (model * vec4(m_v3Position, 1.0)).xyz; // World Space Pos
    v3Normals  = mat3(transpose(inverse(model))) * m_v3Normals; 
    v2TexCoord = m_v2TexCoord;
    v4Color    = m_v4Color;
}