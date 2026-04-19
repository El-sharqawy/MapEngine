#version 460 core

in vec2 TexCoord;
in vec4 OverlayColor;

out vec4 FragColor;

uniform sampler2D u_Texture;

void main()
{
    // Sample the UI Texture Atlas
    vec4 sampled = texture(u_Texture, TexCoord);
    
    // Multiply by the UI Element's color (tint + alpha)
    FragColor = OverlayColor; //sampled;

    // Optimization: If the alpha is essentially zero, discard to save on blending
    if(FragColor.a < 0.001)
        discard;
}