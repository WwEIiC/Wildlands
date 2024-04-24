#version 450 core

layout (location = 0) in vec3 a_WorldPosition;
layout (location = 1) in vec3 a_LocalPosition;
layout (location = 2) in vec4 a_Color;
layout (location = 3) in float a_Thickness;
layout (location = 4) in float a_Fade;
layout (location = 5) in uint a_EntityID;

layout (binding = 0) uniform Camera
{
	mat4 u_ViewprojectionMatrix;
};

struct VertexOutput{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};
layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat uint v_EntityID;



void main()
{
	Output.LocalPosition	= a_LocalPosition;
	Output.Color			= a_Color;
	Output.Thickness		= a_Thickness;
	Output.Fade				= a_Fade;
	v_EntityID				= a_EntityID;

	gl_Position = u_ViewprojectionMatrix * vec4(a_WorldPosition, 1.0f);
}
