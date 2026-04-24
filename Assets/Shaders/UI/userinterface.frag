#version 460 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4      u_Color;     // tint (1,1,1,1) = no tint
uniform bool      u_HasTexture = false;
uniform float u_Alpha;  // 0.0 = invisible, 1.0 = fully loaded

out vec4 FragColor;

void main()
{
    vec4 color = u_HasTexture ? texture(u_Texture, v_TexCoord) : u_Color;
    FragColor = vec4(color.rgb, color.a * u_Alpha);
}
