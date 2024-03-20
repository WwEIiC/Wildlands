#pragma once
//This will be include in the application.
//Make the entry point inside the engine, instead of inside the application.

#ifdef WL_PLATFORM_WINDOWS

extern Wildlands::Application* Wildlands::CreateApplication();

int main(int argc, char** argv)
{
	Wildlands::Log::Init();

	WL_PROFILE_BEGIN_SESSION("Startup", "./profile/WLProfile-Startup.json");
	auto app = Wildlands::CreateApplication();
	WL_PROFILE_END_SESSION();

	WL_PROFILE_BEGIN_SESSION("Runtime", "./profile/WLProfile-Runtime.json");
	app->Run();
	WL_PROFILE_END_SESSION();

	WL_PROFILE_BEGIN_SESSION("Shutdown", "./profile/WLProfile-Shutdown.json");
	delete app;
	WL_PROFILE_END_SESSION();
}

#endif
