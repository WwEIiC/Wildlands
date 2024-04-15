#version 450 core

out vec4 Color;

in vec2 v_TexCoord;
in vec4 v_Color;
in flat float v_TexIndex;
in float v_TileFactor;

uniform sampler2D u_Textures[32];

void main()
{
	Color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TileFactor) * v_Color;
}