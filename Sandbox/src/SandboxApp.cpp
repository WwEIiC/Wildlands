#include <Wildlands.h>
#include "imgui.h"

class TestLayer : public Wildlands::Layer
{
public:
	TestLayer()
		: Layer("TestLayer")
	{
	}

	void Attach() override
	{
		WL_INFO("TestLayer Attached");
	}

	void Detach() override
	{
		WL_INFO("TestLayer Detached");
	}

	void Update() override
	{
	}

	void UIRender() override
	{
		ImGui::Begin("Test UI");
		ImGui::End();
	}

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
