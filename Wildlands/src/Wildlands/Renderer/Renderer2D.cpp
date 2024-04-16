#include "WLPCH.h"
#include "Renderer2D.h"
#include "Wildlands/Renderer/RenderCommand.h"

#include "Wildlands/Renderer/VertexArray.h"
#include "Wildlands/Renderer/Shader.h"

#include <glm/gtc/matrix_transform.hpp>


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

	struct Renderer2DData
	{
		Renderer2D::Stats stats;

		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertecies = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextures = 32;
		uint32_t IndexCount = 0;

		Ref<VertexArray> VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		Ref<Shader> Shader;
		Ref<Texture2D> DefaultTexture;

		QuadVertex* NextQuadVertex = nullptr;
		QuadVertex* QuadVerteciesBase = nullptr;

		std::array<Ref<Texture2D>, MaxTextures> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = default texture.

		glm::vec4 QuadVertexPos[4];
	};
	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		WL_PROFILE_FUNCTION();

		s_Data.VertexArray = VertexArray::Create();

		s_Data.VertexBuffer = VertexBuffer::Create(s_Data.MaxVertecies * sizeof(QuadVertex));
		s_Data.VertexBuffer->SetLayout({
			{EShaderDataType::Float3, "a_Position"},
			{EShaderDataType::Float4, "a_Color"},
			{EShaderDataType::Float2, "a_TexCoord"},
			{EShaderDataType::Float,  "a_TexIndex"},
			{EShaderDataType::Float,  "a_TileFactor"},
			{EShaderDataType::UInt,  "a_EntityID"}
			});
		s_Data.VertexArray->AddVertexBuffer(s_Data.VertexBuffer);

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
		s_Data.VertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_Data.DefaultTexture = Texture2D::Create(1, 1);
		uint32_t defaultTextureData = 0xffffffff;
		s_Data.DefaultTexture->SetData(&defaultTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextures];
		for (uint32_t i = 0; i < s_Data.MaxTextures; i++)
			samplers[i] = i;

		s_Data.Shader =  Shader::Create("Texture", "assets/shaders/TexShader.vert", "assets/shaders/TexShader.frag");
		s_Data.Shader->Bind();
		s_Data.Shader->SetIntArray("u_Textures", samplers, s_Data.MaxTextures);

		s_Data.TextureSlots[0] = s_Data.DefaultTexture;

		s_Data.QuadVertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}
	void Renderer2D::Destory()
	{
		WL_PROFILE_FUNCTION();

		delete[] s_Data.QuadVerteciesBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& cTransform)
	{
		WL_PROFILE_FUNCTION();

		glm::mat4 VPMatrix = camera.GetProjection() * glm::inverse(cTransform);
		s_Data.Shader->Bind();
		s_Data.Shader->SetMat4("u_VPMatrix", VPMatrix);

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		WL_PROFILE_FUNCTION();

		glm::mat4 VPMatrix = camera.GetViewProjection();
		s_Data.Shader->Bind();
		s_Data.Shader->SetMat4("u_VPMatrix", VPMatrix);

		StartBatch();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		WL_PROFILE_FUNCTION();

		s_Data.Shader->Bind();
        s_Data.Shader->SetMat4("u_VPMatrix", camera.GetVPMatrix());

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

		if (s_Data.IndexCount == 0) { return; }

		s_Data.VertexArray->Bind();
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.NextQuadVertex - (uint8_t*)s_Data.QuadVerteciesBase);
		s_Data.VertexBuffer->SetData(s_Data.QuadVerteciesBase, dataSize);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndex(s_Data.VertexArray, s_Data.IndexCount);

		s_Data.stats.drawCalls++;
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
		if (s_Data.IndexCount >= Renderer2DData::MaxIndices)
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
			s_Data.NextQuadVertex->Position = transform * s_Data.QuadVertexPos[i];
			s_Data.NextQuadVertex->Color = color;
			s_Data.NextQuadVertex->Texcoord = textureCoords[i];
			s_Data.NextQuadVertex->TextureIndex = textureIndex;
			s_Data.NextQuadVertex->TileFactor = tileFactor;
			s_Data.NextQuadVertex->EntityID = entityID;
			s_Data.NextQuadVertex++;
		}

		s_Data.IndexCount += 6;
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
		if (s_Data.IndexCount >= Renderer2DData::MaxIndices)
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
			s_Data.NextQuadVertex->Position = transform * s_Data.QuadVertexPos[i];
			s_Data.NextQuadVertex->Color = texColor;
			s_Data.NextQuadVertex->Texcoord = textureCoords[i];
			s_Data.NextQuadVertex->TextureIndex = textureIndex;
			s_Data.NextQuadVertex->TileFactor = tileFactor;
			s_Data.NextQuadVertex->EntityID = entityID;
			s_Data.NextQuadVertex++;
		}

		s_Data.IndexCount += 6;
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
		if (s_Data.IndexCount >= Renderer2DData::MaxIndices)
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

		s_Data.IndexCount += 6;
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
		if (s_Data.IndexCount >= Renderer2DData::MaxIndices)
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

		s_Data.IndexCount += 6;
		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& spriteComp, uint32_t entityID)
	{
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
		s_Data.IndexCount = 0;
		s_Data.NextQuadVertex = s_Data.QuadVerteciesBase;

		s_Data.TextureSlotIndex = 1;
	}
	void Renderer2D::NextBatch()
	{
		EndScene();
		
		Flush();
		StartBatch();
	}
}
