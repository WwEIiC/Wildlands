#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out uint o_EntityID;

struct VertexOutput{
	vec2 TexCoord;
	vec4 Color;
	float TileFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 3) in flat float v_TexIndex;
layout (location = 4) in flat uint v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	o_Color = texture(u_Textures[int(v_TexIndex)], Input.TexCoord * Input.TileFactor) * Input.Color;
	o_EntityID = v_EntityID;
}