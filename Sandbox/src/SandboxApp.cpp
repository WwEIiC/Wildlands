#include <Wildlands.h>

class Sandbox : public Wildlands::Application
{
public:
	Sandbox()
	{

	}
	~Sandbox()
	{

	}
};

Wildlands::Application* Wildlands::CreateApplication()
{
	return new Sandbox();
}
