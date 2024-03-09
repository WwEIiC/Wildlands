#include <Wildlands.h>
#include "imgui.h"

class TestLayer : public Wildlands::Layer
{
public:
	TestLayer()
		: Layer("TestLayer"), m_Camera({-1.6f, 1.6f, -0.9f, 0.9f})
	{
		//VAO VBO IBO Shader
		m_VertexArray.reset(Wildlands::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 0.2f, 1.0f, 1.0f
		};
		std::shared_ptr<Wildlands::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Wildlands::VertexBuffer::Create(vertices, sizeof(vertices)));

		Wildlands::BufferLayout layout = {
			{ Wildlands::EShaderDataType::Float3, "a_Position"},
			{ Wildlands::EShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		
		unsigned int indices[3] = { 0, 1, 2	};
		std::shared_ptr<Wildlands::IndexBuffer> indexBuffer;
		indexBuffer.reset(Wildlands::IndexBuffer::Create(indices, 3));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		std::string vertexSrc = Wildlands::FileReader::Get().Read("../Wildlands/src/Wildlands/Renderer/Shaders/VertextShader.vert");
		std::string fragmentSrc = Wildlands::FileReader::Get().Read("../Wildlands/src/Wildlands/Renderer/Shaders/FragmentShader.frag");

		m_Shader.reset(new Wildlands::Shader(vertexSrc, fragmentSrc));

	}

	virtual void Attach() override
	{
		WL_INFO("TestLayer Attach!");
	}
	virtual void Detach() override
	{
		WL_INFO("TestLayer Detach!");
	}

	virtual void Update() override
	{
		Wildlands::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Wildlands::RenderCommand::Clear();

		Wildlands::Renderer::BeginScene(m_Camera);

		Wildlands::Renderer::Submit(m_Shader, m_VertexArray);

		Wildlands::Renderer::EndScene();

		glm::vec3 position = m_Camera.GetPosition();
		if (Wildlands::Input::IsKeyDown(WL_KEY_A))	{	position -= glm::vec3(0.01f, 0.0f, 0.0f);	}	
		if (Wildlands::Input::IsKeyDown(WL_KEY_D))	{	position += glm::vec3(0.01f, 0.0f, 0.0f);	}
		if (Wildlands::Input::IsKeyDown(WL_KEY_W))	{	position += glm::vec3(0.0f, 0.01f, 0.0f);	}
		if (Wildlands::Input::IsKeyDown(WL_KEY_S))	{	position -= glm::vec3(0.0f, 0.01f, 0.0f);	}
		m_Camera.SetPosition(position);

		float rotation = m_Camera.GetRotation();
		if (Wildlands::Input::IsKeyDown(WL_KEY_Q)) { rotation += 0.1f; }
		if (Wildlands::Input::IsKeyDown(WL_KEY_E)) { rotation -= 0.1f; }
		m_Camera.SetRotation(rotation);
	}

	virtual void UIRender() override
	{
		ImGui::Begin("TestUI");
		ImGui::End();
	}

	virtual void OnEvent(Wildlands::Event& event) override
	{
		Wildlands::EventDispatcher dispatcher(event);
	}

private:
	std::shared_ptr<Wildlands::Shader> m_Shader;
	std::shared_ptr<Wildlands::VertexArray> m_VertexArray;

	Wildlands::OrthographicCamera m_Camera;

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
