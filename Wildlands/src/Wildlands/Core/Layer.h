#pragma once
#include "Wildlands/Core/Core.h"

#include "Wildlands/Events/Event.h"

namespace Wildlands
{
	class WL_API Layer
	{
	public:
		Layer(const std::string& name = "LayerName");
		virtual ~Layer() = default;

		virtual void Attach() {}
		virtual void Detach() {}
		virtual void Update() {}
		virtual void UIRender() {}

		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() { return m_Name; }
	protected:
		std::string m_Name;
	};
}

