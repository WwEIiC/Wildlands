#include <Wildlands.h>
#include <Wildlands/Core/EntryPoint.h>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class Sandbox : public Wildlands::Application
{
public:
	Sandbox(Wildlands::ApplicationCommandLineArgs args)
		: Application("WLEditor", args)
	{
		PushLayer(new Sandbox2D());
	}
	~Sandbox()
	{
	}
};

Wildlands::Application* Wildlands::CreateApplication(Wildlands::ApplicationCommandLineArgs args)
{
	return new Sandbox(args);
}
