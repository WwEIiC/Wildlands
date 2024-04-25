#include <Wildlands.h>
#include <Wildlands/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Wildlands
{
	class WLEditor : public Application
	{
	public:
		WLEditor(const ApplicationSpecification& specification)
			: Application(specification)
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "WLEditor";
		spec.CommandLineArgs = args;

		return new WLEditor(spec);
	}
}
