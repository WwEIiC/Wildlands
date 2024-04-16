#version 450 core

layout(location = 0) out vec4 Color;
layout(location = 1) out uint EntityID;

in vec2 v_TexCoord;
in vec4 v_Color;
in flat float v_TexIndex;
in float v_TileFactor;
in flat uint v_EntityID;

uniform sampler2D u_Textures[32];

void main()
{
	Color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TileFactor) * v_Color;
	EntityID = v_EntityID;
}