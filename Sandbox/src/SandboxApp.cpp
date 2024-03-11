#include <Wildlands.h>
#include "imgui.h"

#include "Wildlands/Platforms/OpenGL/OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>

class TestLayer : public Wildlands::Layer
{
public:
	TestLayer()
		: Layer("TestLayer"), m_Camera({-1.6f, 1.6f, -0.9f, 0.9f})
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


		m_Shader = (Wildlands::Shader::Create("./assets/Shaders/VertextShader.vert", "./assets/Shaders/FragmentShader.frag"));
		m_TexShader = (Wildlands::Shader::Create("./assets/Shaders/TexShader.vert", "./assets/Shaders/TexShader.frag"));

		//m_Texture = Wildlands::Texture2D::Create("./assets/Textures/Checkerboard.png");
		m_Texture = Wildlands::Texture2D::Create("./assets/Textures/Texture1.png");
		m_TexShader->Bind();

		std::dynamic_pointer_cast<Wildlands::OpenGLShader>(m_TexShader)->SetUniformInt("u_Texture", 0);
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
		Wildlands::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Wildlands::RenderCommand::Clear();

		Wildlands::Renderer::BeginScene(m_Camera);

		m_Texture->Bind();
		//std::dynamic_pointer_cast<Wildlands::OpenGLShader>(m_Shader)->SetUniformFloat4("u_Color", color);
		//Wildlands::Renderer::Submit(m_Shader, m_VertexArray);
		Wildlands::Renderer::Submit(m_TexShader, m_VertexArray);

		Wildlands::Renderer::EndScene();

		glm::vec3 position = m_Camera.GetPosition();
		if (Wildlands::Input::IsKeyDown(WL_KEY_A)) { position.x -= m_CameraMoveSpeed * ts; }
		if (Wildlands::Input::IsKeyDown(WL_KEY_D)) { position.x += m_CameraMoveSpeed * ts; }
		if (Wildlands::Input::IsKeyDown(WL_KEY_W)) { position.y += m_CameraMoveSpeed * ts; }
		if (Wildlands::Input::IsKeyDown(WL_KEY_S)) { position.y -= m_CameraMoveSpeed * ts; }
		m_Camera.SetPosition(position);

		float rotation = m_Camera.GetRotation();
		if (Wildlands::Input::IsKeyDown(WL_KEY_Q)) { rotation += m_CameraRotateSpeed * ts; }
		if (Wildlands::Input::IsKeyDown(WL_KEY_E)) { rotation -= m_CameraRotateSpeed * ts; }
		m_Camera.SetRotation(rotation);
	}

	virtual void UIRender() override
	{
		ImGui::Begin("TestUI");
		ImGui::ColorEdit4("Color", glm::value_ptr(color));
		ImGui::End();
	}

	virtual void OnEvent(Wildlands::Event& event) override
	{
		Wildlands::EventDispatcher dispatcher(event);
	}

private:
	Wildlands::Ref<Wildlands::Shader> m_Shader;
	Wildlands::Ref<Wildlands::Texture> m_Texture;
	
	Wildlands::Ref<Wildlands::Shader> m_TexShader;
	Wildlands::Ref<Wildlands::VertexArray> m_VertexArray;

	Wildlands::OrthographicCamera m_Camera;
	glm::vec4 color = {0.8f, 0.2f, 0.8f, 1.0f};

	float m_CameraMoveSpeed = 0.5f;
	float m_CameraRotateSpeed = 120.0f;
};

class Sandbox : public Wildlands::Application
{
public:
	Sandbox()
	{
		PushLayer(new TestLayer());
	}
	~Sandbox()
	{
	}
};

Wildlands::Application* Wildlands::CreateApplication()
{
	return new Sandbox();
}
