#include <Wildlands.h>

class Sandbox : public Wildlands::Application
{
public:
	Sandbox()
	{
		PushLayer(new Wildlands::ImGuiLayer());
	}
	~Sandbox()
	{

	}
};

Wildlands::Application* Wildlands::CreateApplication()
{
	return new Sandbox();
}
