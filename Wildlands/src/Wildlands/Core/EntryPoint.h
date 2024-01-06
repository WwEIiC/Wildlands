#pragma once
//This will be include in the application.
//Make the entry point inside the engine, instead of inside the application.

#ifdef WL_PLATFORM_WINDOWS

extern Wildlands::Application* Wildlands::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Wildlands::CreateApplication();
	app->Run();
	delete app;
}

#endif
