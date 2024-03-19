#version 330 core

out vec4 Color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main()
{
	Color = texture(u_Texture, v_TexCoord * 10);
}