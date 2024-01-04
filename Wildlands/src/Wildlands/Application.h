#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Wildlands
{
	class WL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	//Will be defined in Client
	Application* CreateApplication();
}

