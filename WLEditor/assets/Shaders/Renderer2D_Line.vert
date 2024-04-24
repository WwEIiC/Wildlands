#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in uint a_EntityID;

layout (binding = 0) uniform Camera
{
	mat4 u_ViewprojectionMatrix;
};

struct VertexOutput{
	vec4 Color;
};
layout (location = 0) out VertexOutput Output;
layout (location = 1) out flat uint v_EntityID;


void main()
{
	Output.Color		= a_Color;
	v_EntityID			= a_EntityID;

	gl_Position = u_ViewprojectionMatrix * vec4(a_Position, 1.0f);
}
