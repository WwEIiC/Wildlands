#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TexIndex;
layout (location = 4) in float a_TileFactor;
layout (location = 5) in uint a_EntityID;

layout (binding = 0) uniform Camera
{
	mat4 u_ViewprojectionMatrix;
};

struct VertexOutput{
	vec2 TexCoord;
	vec4 Color;
	float TileFactor;
};
layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat float v_TexIndex;
layout (location = 4) out flat uint v_EntityID;



void main()
{
	Output.TexCoord		= a_TexCoord;
	Output.Color		= a_Color;
	Output.TileFactor	= a_TileFactor;
	v_TexIndex			= a_TexIndex;
	v_EntityID			= a_EntityID;

	gl_Position = u_ViewprojectionMatrix * vec4(a_Position, 1.0f);
}
