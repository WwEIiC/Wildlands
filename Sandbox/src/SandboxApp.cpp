#include <Wildlands.h>
#include <Wildlands/Core/EntryPoint.h>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class Sandbox : public Wildlands::Application
{
public:
	Sandbox(const Wildlands::ApplicationSpecification& specification)
		: Wildlands::Application(specification)
	{
		PushLayer(new Sandbox2D());
	}
	~Sandbox()
	{
	}
};

Wildlands::Application* Wildlands::CreateApplication(Wildlands::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../WLEditor";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
