#include "WLPCH.h"
#include "Renderer2D.h"
#include "Wildlands/Renderer/RenderCommand.h"

#include "Wildlands/Renderer/VertexArray.h"
#include "Wildlands/Renderer/Shader.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Wildlands
{
	struct Renderer2DData
	{
		Ref<VertexArray> vertexArray;
		Ref<Shader> shader;
		Ref<Texture2D> defaultTexture;
	};

	static Renderer2DData* s_Data;

	void Renderer2D::Init()
	{
		WL_PROFILE_FUNCTION();

		s_Data = new Renderer2DData();

		float squareVertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		BufferLayout layout = {
			{EShaderDataType::Float3, "a_Position"},
			{EShaderDataType::Float2, "a_TexCoord"}
		};
		squareVB->SetLayout(layout);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

		s_Data->vertexArray = VertexArray::Create();
		s_Data->vertexArray->AddVertexBuffer(squareVB);
		s_Data->vertexArray->SetIndexBuffer(squareIB);

		s_Data->shader =  Shader::Create("Texture", "assets/shaders/TexShader.vert", "assets/shaders/TexShader.frag");
		s_Data->shader->Bind();
		s_Data->shader->SetInt("u_Texture", 0);

		s_Data->defaultTexture = Texture2D::Create(1, 1);
		uint32_t defaultTextureData = 0xffffffff;
		s_Data->defaultTexture->SetData(&defaultTextureData, sizeof(uint32_t));
	}
	void Renderer2D::Destory()
	{
		WL_PROFILE_FUNCTION();

		delete s_Data;
	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		WL_PROFILE_FUNCTION();

		s_Data->shader->Bind();
        s_Data->shader->SetMat4("u_VPMatrix", camera.GetVPMatrix());
	}

	void Renderer2D::EndScene()
	{
		WL_PROFILE_FUNCTION();

	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		WL_PROFILE_FUNCTION();

		s_Data->shader->Bind();
		s_Data->shader->SetFloat4("u_Color", color);
		s_Data->defaultTexture->Bind();

		glm::mat4 transforms = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
        s_Data->shader->SetMat4("u_Transform", transforms);

		s_Data->vertexArray->Bind();
		RenderCommand::DrawIndex(s_Data->vertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, texture);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		WL_PROFILE_FUNCTION();

		s_Data->shader->Bind();
		s_Data->shader->SetFloat4("u_Color", glm::vec4(1.0f));

		texture->Bind(0);

		glm::mat4 transforms = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->shader->SetMat4("u_Transform", transforms);

		s_Data->vertexArray->Bind();
		RenderCommand::DrawIndex(s_Data->vertexArray);
	}
}
