#include <Wildlands.h>
#include <Wildlands/Core/EntryPoint.h>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class Sandbox : public Wildlands::Application
{
public:
	Sandbox()
	{
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
