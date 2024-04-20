#version 450 core

layout(location = 0) out vec4 Color;
layout(location = 1) out uint EntityID;

struct VertexOutput{
	vec2 TexCoord;
	vec4 Color;
	flat float TexIndex;
	float TileFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat uint v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	Color = texture(u_Textures[int(Input.TexIndex)], Input.TexCoord * Input.TileFactor) * Input.Color;
	EntityID = v_EntityID;
}