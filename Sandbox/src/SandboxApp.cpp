#include <Wildlands.h>
#include <Wildlands/Core/EntryPoint.h>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class TestLayer : public Wildlands::Layer
{
public:
	TestLayer()
		: Layer("TestLayer"), m_CameraController(1280.f / 720.f)
	{
		//VAO VBO IBO Shader
		m_VertexArray = Wildlands::VertexArray::Create();

		float vertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		Wildlands::Ref<Wildlands::VertexBuffer> vertexBuffer;
		vertexBuffer = (Wildlands::VertexBuffer::Create(vertices, sizeof(vertices)));

		Wildlands::BufferLayout layout = {
			{ Wildlands::EShaderDataType::Float3, "a_Position"},
			{ Wildlands::EShaderDataType::Float2, "a_TexCoord"}
		};
		vertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		
		unsigned int indices[6] = { 0, 1, 2, 2, 3, 0};
		Wildlands::Ref<Wildlands::IndexBuffer> indexBuffer;
		indexBuffer = (Wildlands::IndexBuffer::Create(indices, 6));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		m_Shader = (Wildlands::Shader::Create("DefalutShader", "./assets/Shaders/FlatColor.vert", "./assets/Shaders/FlatColor.frag"));
		m_TexShader = (Wildlands::Shader::Create("TexShader", "./assets/Shaders/TexShader.vert", "./assets/Shaders/TexShader.frag"));

		m_Texture = Wildlands::Texture2D::Create("./assets/Textures/Checkerboard.png");
		//m_Texture = Wildlands::Texture2D::Create("./assets/Textures/Texture1.png");
		m_TexShader->Bind();

		m_TexShader->SetInt("u_Texture", 0);
	}

	virtual void Attach() override
	{
		WL_INFO("TestLayer Attach!");
	}
	virtual void Detach() override
	{
		WL_INFO("TestLayer Detach!");
	}

	virtual void Update(Wildlands::Timestep ts) override
	{
		//Camera
		m_CameraController.OnUpdate(ts);

		Wildlands::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Wildlands::RenderCommand::Clear();

		Wildlands::Renderer::BeginScene(m_CameraController.GetCamera());

		m_Texture->Bind();
		//std::dynamic_pointer_cast<Wildlands::OpenGLShader>(m_Shader)->SetUniformFloat4("u_Color", color);
		//Wildlands::Renderer::Submit(m_Shader, m_VertexArray);
		Wildlands::Renderer::Submit(m_TexShader, m_VertexArray);

		Wildlands::Renderer::EndScene();

		
	}

	virtual void UIRender() override
	{
		ImGui::Begin("TestUI");
		ImGui::ColorEdit4("Color", glm::value_ptr(color));
		ImGui::End();
	}

	virtual void OnEvent(Wildlands::Event& event) override
	{
		m_CameraController.OnEvent(event);
	}

private:
	Wildlands::Ref<Wildlands::Shader> m_Shader;
	Wildlands::Ref<Wildlands::Texture> m_Texture;
	
	Wildlands::Ref<Wildlands::Shader> m_TexShader;
	Wildlands::Ref<Wildlands::VertexArray> m_VertexArray;

	Wildlands::OrthographicCameraController m_CameraController;

	glm::vec4 color = {0.8f, 0.2f, 0.8f, 1.0f};
};

class Sandbox : public Wildlands::Application
{
public:
	Sandbox()
	{
		//PushLayer(new TestLayer());
		PushLayer(new Sandbox2D());
	}
	~Sandbox()
	{
	}
};

Wildlands::Application* Wildlands::CreateApplication()
{
	return new Sandbox();
}
