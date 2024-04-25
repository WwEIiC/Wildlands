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
	vec4 textureColor = texture(u_Textures[int(v_TexIndex)], Input.TexCoord * Input.TileFactor) * Input.Color;
	if (textureColor.a == 0.0)
		discard;

	o_Color = textureColor;
	o_EntityID = v_EntityID;
}