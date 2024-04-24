#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out uint o_EntityID;

struct VertexOutput{
	vec4 Color;
};

layout (location = 0) in VertexOutput Input;
layout (location = 1) in flat uint v_EntityID;

void main()
{
	o_Color = Input.Color;
	o_EntityID = v_EntityID;
}