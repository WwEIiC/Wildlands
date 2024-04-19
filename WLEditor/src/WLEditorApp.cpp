#include <Wildlands.h>
#include <Wildlands/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Wildlands
{
	class WLEditor : public Application
	{
	public:
		WLEditor(ApplicationCommandLineArgs args)
			: Application("WLEditor", args)
		{
			PushLayer(new EditorLayer());
		}

		~WLEditor()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new WLEditor(args);
	}
}
