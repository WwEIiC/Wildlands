#include "WLPCH.h"
#include "Renderer2D.h"
#include "Wildlands/Renderer/RenderCommand.h"

#include "Wildlands/Renderer/VertexArray.h"
#include "Wildlands/Renderer/UniformBuffer.h"
#include "Wildlands/Renderer/Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Wildlands
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 Texcoord;
		float TextureIndex;
		float TileFactor;
		//Editor-only
		uint32_t EntityID;
	};
	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;
		//Editor-only
		uint32_t EntityID;
	}; 
	struct LineVertex 
	{
		glm::vec3 Position;
		glm::vec4 Color;
		//Editor-only
		uint32_t EntityID;
	};

	struct Renderer2DData
	{
		Renderer2D::Stats stats;

		static const uint32_t MaxQuads = 5000;
		static const uint32_t MaxVertecies = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextures = 32;

		// Quad
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		uint32_t QuadIndexCount = 0;
		QuadVertex* NextQuadVertex = nullptr;
		QuadVertex* QuadVerteciesBase = nullptr;

		Ref<Texture2D> WhiteTexture;
		std::array<Ref<Texture2D>, MaxTextures> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = default texture.
		glm::vec4 QuadVertexPos[4];

		// Circle
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;
		uint32_t CircleIndexCount = 0;
		CircleVertex* NextCircleVertex = nullptr;
		CircleVertex* CircleVerteciesBase = nullptr;

		// Line
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;
		uint32_t LineVertexCount = 0;
		LineVertex* NextLineVertex = nullptr;
		LineVertex* LineVerteciesBase = nullptr;
		float LineWidth = 0.7f;

		// Uniform Buffer
		struct CameraData
		{
			glm::mat4 ViewProjectionMatrix;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};
	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		WL_PROFILE_FUNCTION();

		// Quad
		s_Data.QuadVertexArray = VertexArray::Create();
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertecies * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"	},
			{ ShaderDataType::Float4, "a_Color"		},
			{ ShaderDataType::Float2, "a_TexCoord"	},
			{ ShaderDataType::Float,  "a_TexIndex"	},
			{ ShaderDataType::Float,  "a_TileFactor"},
			{ ShaderDataType::UInt,   "a_EntityID"	}
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.QuadVerteciesBase = new QuadVertex[s_Data.MaxVertecies];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		for (uint32_t i = 0, offset = 0; i < s_Data.MaxIndices; i += 6, offset += 4)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
		}
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circle
		s_Data.CircleVertexArray = VertexArray::Create();
		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertecies * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition"	},
			{ ShaderDataType::Float3, "a_LoaclPosition"	},
			{ ShaderDataType::Float4, "a_Color"			},
			{ ShaderDataType::Float,  "a_Thickness"		},
			{ ShaderDataType::Float,  "a_Fade"			},
			{ ShaderDataType::UInt,   "a_EntityID"		}
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB);// just use the same indexbuffer of the quad.
		s_Data.CircleVerteciesBase = new CircleVertex[s_Data.MaxVertecies];

		// Line
		s_Data.LineVertexArray = VertexArray::Create();
		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertecies * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"	},
			{ ShaderDataType::Float4, "a_Color"		},
			{ ShaderDataType::UInt,	  "a_EntityID"	}
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVerteciesBase = new LineVertex[s_Data.MaxVertecies];

		// Other default things
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t defaultTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&defaultTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextures];
		for (uint32_t i = 0; i < s_Data.MaxTextures; i++)
			samplers[i] = i;

		s_Data.QuadShader	=  Shader::Create("Quad", "assets/shaders/Renderer2D_Quad.vert", "assets/shaders/Renderer2D_Quad.frag");
		s_Data.CircleShader =  Shader::Create("Circle", "assets/shaders/Renderer2D_Circle.vert", "assets/shaders/Renderer2D_Circle.frag");
		s_Data.LineShader	=  Shader::Create("Circle", "assets/shaders/Renderer2D_Line.vert", "assets/shaders/Renderer2D_Line.frag");

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}
	void Renderer2D::Destory()
	{
		WL_PROFILE_FUNCTION();

		delete[] s_Data.QuadVerteciesBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& cTransform)
	{
		WL_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjectionMatrix = camera.GetProjection() * glm::inverse(cTransform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		WL_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjectionMatrix = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		WL_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjectionMatrix = camera.GetVPMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		WL_PROFILE_FUNCTION();

		Flush();
	}
	void Renderer2D::Flush()
	{
		WL_PROFILE_FUNCTION();

		// Draw Quad.
		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.NextQuadVertex - (uint8_t*)s_Data.QuadVerteciesBase);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVerteciesBase, dataSize);

			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.QuadShader->Bind();
			RenderCommand::DrawIndex(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

			s_Data.stats.drawCalls++;
		}

		// Draw Circle
		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.NextCircleVertex - (uint8_t*)s_Data.CircleVerteciesBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVerteciesBase, dataSize);

			s_Data.CircleShader->Bind();
			RenderCommand::DrawIndex(s_Data.CircleVertexArray, s_Data.CircleIndexCount);

			s_Data.stats.drawCalls++;
		} 

		// Draw Line 
		if (s_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.NextLineVertex - (uint8_t*)s_Data.LineVerteciesBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVerteciesBase, dataSize);

			s_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawLine(s_Data.LineVertexArray, s_Data.LineVertexCount);

			s_Data.stats.drawCalls++;
		}
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		WL_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint32_t entityID)
	{
		WL_PROFILE_FUNCTION();

		//the buffer is full, flush it and start next batch.
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		const float textureIndex = 0.f;
		const float tileFactor = 1.0f;

		glm::vec2 textureCoords[4] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		for (int i = 0; i < 4; i++)
		{
			s_Data.NextQuadVertex->Position		= transform * s_Data.QuadVertexPos[i];
			s_Data.NextQuadVertex->Color		= color;
			s_Data.NextQuadVertex->Texcoord		= textureCoords[i];
			s_Data.NextQuadVertex->TextureIndex = textureIndex;
			s_Data.NextQuadVertex->TileFactor	= tileFactor;
			s_Data.NextQuadVertex->EntityID		= entityID;
			s_Data.NextQuadVertex++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tileFactor, const glm::vec4& texColor)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, texture, tileFactor, texColor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tileFactor, const glm::vec4&texColor)
	{
		WL_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tileFactor, texColor);
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tileFactor, const glm::vec4& texColor, uint32_t entityID)
	{
		WL_PROFILE_FUNCTION();

		//the buffer is full, flush it and start next batch.
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}
		if (textureIndex == 0.f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextures)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex++] = texture;
		}

		glm::vec2 textureCoords[4] = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
		};

		for (int i = 0; i < 4; i++)
		{
			s_Data.NextQuadVertex->Position		= transform * s_Data.QuadVertexPos[i];
			s_Data.NextQuadVertex->Color		= texColor;
			s_Data.NextQuadVertex->Texcoord		= textureCoords[i];
			s_Data.NextQuadVertex->TextureIndex = textureIndex;
			s_Data.NextQuadVertex->TileFactor	= tileFactor;
			s_Data.NextQuadVertex->EntityID		= entityID;
			s_Data.NextQuadVertex++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.stats.quadCount++;
	}


	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		WL_PROFILE_FUNCTION();

		//the buffer is full, flush it and start next batch.
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		const float textureIndex = 0.f;
		const float tileFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		glm::vec2 textureCoords[4] = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
		};

		for (int i = 0; i < 4; i++)
		{
			s_Data.NextQuadVertex->Position = transform * s_Data.QuadVertexPos[i];
			s_Data.NextQuadVertex->Color = color;
			s_Data.NextQuadVertex->Texcoord = textureCoords[i];
			s_Data.NextQuadVertex->TextureIndex = textureIndex;
			s_Data.NextQuadVertex->TileFactor = tileFactor;
			s_Data.NextQuadVertex++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.stats.quadCount++;
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tileFactor, const glm::vec4& texColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tileFactor, texColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tileFactor, const glm::vec4& texColor)
	{
		WL_PROFILE_FUNCTION();

		//the buffer is full, flush it and start next batch.
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextures)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex++] = texture;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		glm::vec2 textureCoords[4] = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
		};

		for (int i = 0; i < 4; i++)
		{
			s_Data.NextQuadVertex->Position = transform * s_Data.QuadVertexPos[i];
			s_Data.NextQuadVertex->Color = texColor;
			s_Data.NextQuadVertex->Texcoord = textureCoords[i];
			s_Data.NextQuadVertex->TextureIndex = textureIndex;
			s_Data.NextQuadVertex->TileFactor = tileFactor;
			s_Data.NextQuadVertex++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, uint32_t entityID)
	{
		WL_PROFILE_FUNCTION();

		// WLTODO::implement for circle.
		//the buffer is full, flush it and start next batch.
		//if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		//	NextBatch();

		for (int i = 0; i < 4; i++)
		{
			s_Data.NextCircleVertex->WorldPosition	= transform * s_Data.QuadVertexPos[i];
			s_Data.NextCircleVertex->LocalPosition	= s_Data.QuadVertexPos[i] * 2.0f; //[-1.0f, 1.0f]
			s_Data.NextCircleVertex->Color			= color;
			s_Data.NextCircleVertex->Thickness		= thickness;
			s_Data.NextCircleVertex->Fade			= fade;
			s_Data.NextCircleVertex->EntityID		= entityID;
			s_Data.NextCircleVertex++;
		}

		s_Data.CircleIndexCount += 6;
		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& pStart, const glm::vec3& pEnd, const glm::vec4& color, uint32_t entityID)
	{
		WL_PROFILE_FUNCTION();

		// WLTODO::implement for line.
		//the buffer is full, flush it and start next batch.
		//if (s_Data.LineVertexCount >= Renderer2DData::MaxVertecies)
		//	NextBatch();

		s_Data.NextLineVertex->Position = pStart;
		s_Data.NextLineVertex->Color = color;
		s_Data.NextLineVertex->EntityID = entityID;
		s_Data.NextLineVertex++;

		s_Data.NextLineVertex->Position = pEnd;
		s_Data.NextLineVertex->Color = color;
		s_Data.NextLineVertex->EntityID = entityID;
		s_Data.NextLineVertex++;

		s_Data.LineVertexCount += 2;
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, uint32_t entityID)
	{
		glm::vec3 pLB = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 pRB = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 pRT = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 pLT = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(pLB, pRB, color, entityID);
		DrawLine(pRB, pRT, color, entityID);
		DrawLine(pRT, pLT, color, entityID);
		DrawLine(pLT, pLB, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, uint32_t entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.QuadVertexPos[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& spriteComp, uint32_t entityID)
	{
		if (spriteComp.Texture)
			DrawQuad(transform, spriteComp.Texture, spriteComp.TilingFactor, spriteComp.Color, entityID);
		else
			DrawQuad(transform, spriteComp.Color, entityID);
	}
	
	Renderer2D::Stats& Renderer2D::GetStats()
	{
		return s_Data.stats;
	}
	void Renderer2D::ResetStats()
	{
		s_Data.stats.drawCalls = 0;
		s_Data.stats.quadCount = 0;
	}

	void Renderer2D::StartBatch()
	{
		// Quad data
		s_Data.QuadIndexCount = 0;
		s_Data.NextQuadVertex = s_Data.QuadVerteciesBase;

		// Circle data
		s_Data.CircleIndexCount = 0;
		s_Data.NextCircleVertex = s_Data.CircleVerteciesBase;

		// Line data
		s_Data.LineVertexCount = 0;
		s_Data.NextLineVertex = s_Data.LineVerteciesBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}
}
