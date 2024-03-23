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
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texcoord;
		float textureIndex;
		float tileFactor;
	};

	struct Renderer2DData
	{
		Renderer2D::Stats stats;

		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertecies = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextures = 32;
		uint32_t indexCount = 0;

		Ref<VertexArray> vertexArray;
		Ref<VertexBuffer> vertexBuffer;
		Ref<Shader> shader;
		Ref<Texture2D> defaultTexture;

		QuadVertex* nextQuadVertex = nullptr;
		QuadVertex* quadVerteciesBase = nullptr;

		std::array<Ref<Texture2D>, MaxTextures> textureSlots;
		uint32_t textureSlotIndex = 1; // 0 = default texture.

		glm::vec4 quadVertexPos[4];
	};
	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		WL_PROFILE_FUNCTION();

		s_Data.vertexArray = VertexArray::Create();

		s_Data.vertexBuffer = VertexBuffer::Create(s_Data.MaxVertecies * sizeof(QuadVertex));
		s_Data.vertexBuffer->SetLayout({
			{EShaderDataType::Float3, "a_Position"},
			{EShaderDataType::Float4, "a_Color"},
			{EShaderDataType::Float2, "a_TexCoord"},
			{EShaderDataType::Float,  "a_TexIndex"},
			{EShaderDataType::Float,  "a_TileFactor"}
			});
		s_Data.vertexArray->AddVertexBuffer(s_Data.vertexBuffer);

		s_Data.quadVerteciesBase = new QuadVertex[s_Data.MaxVertecies];

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
		s_Data.vertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_Data.defaultTexture = Texture2D::Create(1, 1);
		uint32_t defaultTextureData = 0xffffffff;
		s_Data.defaultTexture->SetData(&defaultTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextures];
		for (uint32_t i = 0; i < s_Data.MaxTextures; i++)
			samplers[i] = i;

		s_Data.shader =  Shader::Create("Texture", "assets/shaders/TexShader.vert", "assets/shaders/TexShader.frag");
		s_Data.shader->Bind();
		s_Data.shader->SetIntArray("u_Textures", samplers, s_Data.MaxTextures);

		s_Data.textureSlots[0] = s_Data.defaultTexture;

		s_Data.quadVertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPos[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPos[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPos[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}
	void Renderer2D::Destory()
	{
		WL_PROFILE_FUNCTION();

		delete[] s_Data.quadVerteciesBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		WL_PROFILE_FUNCTION();

		s_Data.shader->Bind();
        s_Data.shader->SetMat4("u_VPMatrix", camera.GetVPMatrix());

		s_Data.indexCount = 0;
		s_Data.nextQuadVertex = s_Data.quadVerteciesBase;

		s_Data.textureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		WL_PROFILE_FUNCTION();

		s_Data.vertexArray->Bind();
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.nextQuadVertex - (uint8_t*)s_Data.quadVerteciesBase);
		s_Data.vertexBuffer->SetData(s_Data.quadVerteciesBase, dataSize);

		Flush();
	}
	void Renderer2D::Flush()
	{
		WL_PROFILE_FUNCTION();

		if (s_Data.indexCount == 0) { return; }

		for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
			s_Data.textureSlots[i]->Bind(i);

		RenderCommand::DrawIndex(s_Data.vertexArray, s_Data.indexCount);

		s_Data.stats.drawCalls++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		WL_PROFILE_FUNCTION();

		//the buffer is full, flush it and start next batch.
		if (s_Data.indexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		const float textureIndex = 0.f;
		const float tileFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		glm::vec2 textureCoords[4] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		for (int i = 0; i < 4; i++)
		{
			s_Data.nextQuadVertex->position = transform * s_Data.quadVertexPos[i];
			s_Data.nextQuadVertex->color = color;
			s_Data.nextQuadVertex->texcoord = textureCoords[i];
			s_Data.nextQuadVertex->textureIndex = textureIndex;
			s_Data.nextQuadVertex->tileFactor = tileFactor;
			s_Data.nextQuadVertex++;
		}

		s_Data.indexCount += 6;
		s_Data.stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tileFactor, const glm::vec4& texColor)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, texture, tileFactor, texColor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tileFactor, const glm::vec4&texColor)
	{
		WL_PROFILE_FUNCTION();

		//the buffer is full, flush it and start next batch.
		if (s_Data.indexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.f;
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}
		if (textureIndex == 0.f)
		{
			if (s_Data.textureSlotIndex >= Renderer2DData::MaxTextures)
				NextBatch();

			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex++] = texture;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		glm::vec2 textureCoords[4] = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
		};

		for (int i = 0; i < 4; i++)
		{
			s_Data.nextQuadVertex->position = transform * s_Data.quadVertexPos[i];
			s_Data.nextQuadVertex->color = texColor;
			s_Data.nextQuadVertex->texcoord = textureCoords[i];
			s_Data.nextQuadVertex->textureIndex = textureIndex;
			s_Data.nextQuadVertex->tileFactor = tileFactor;
			s_Data.nextQuadVertex++;
		}

		s_Data.indexCount += 6;
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
		if (s_Data.indexCount >= Renderer2DData::MaxIndices)
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
			s_Data.nextQuadVertex->position = transform * s_Data.quadVertexPos[i];
			s_Data.nextQuadVertex->color = color;
			s_Data.nextQuadVertex->texcoord = textureCoords[i];
			s_Data.nextQuadVertex->textureIndex = textureIndex;
			s_Data.nextQuadVertex->tileFactor = tileFactor;
			s_Data.nextQuadVertex++;
		}

		s_Data.indexCount += 6;
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
		if (s_Data.indexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.textureSlotIndex >= Renderer2DData::MaxTextures)
				NextBatch();

			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex++] = texture;
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
			s_Data.nextQuadVertex->position = transform * s_Data.quadVertexPos[i];
			s_Data.nextQuadVertex->color = texColor;
			s_Data.nextQuadVertex->texcoord = textureCoords[i];
			s_Data.nextQuadVertex->textureIndex = textureIndex;
			s_Data.nextQuadVertex->tileFactor = tileFactor;
			s_Data.nextQuadVertex++;
		}

		s_Data.indexCount += 6;
		s_Data.stats.quadCount++;
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
	void Renderer2D::NextBatch()
	{
		EndScene();

		s_Data.indexCount = 0;
		s_Data.nextQuadVertex = s_Data.quadVerteciesBase;

		s_Data.textureSlotIndex = 1;
	}
}
