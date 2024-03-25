#include <Wildlands.h>
#include <Wildlands/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Wildlands
{
	class WLEditor : public Application
	{
	public:
		WLEditor()
			: Application("Wildlands Editor")
		{
			PushLayer(new EditorLayer());
		}

		~WLEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new WLEditor();
	}
}
