#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D spriteTexture;

void main()
{
    FragColor = vec4(TexCoord.x, TexCoord.y, 1, 1);
}