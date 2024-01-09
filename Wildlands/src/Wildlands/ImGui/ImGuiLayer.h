#pragma once
#include "Wildlands/Core/Core.h"
#include "Wildlands/Core/Layer.h"

namespace Wildlands
{
	class WL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer() override;

		virtual void Attach() override;
		virtual void Detach() override;
		virtual void Update() override;

		virtual void OnEvent(Event& event) override;
	private:
		bool OnWindowResize(class WindowResizeEvent& event);

		bool OnMouseMoved(class MouseMovedEvent& event);
		bool OnMouseScrolled(class MouseScrolledEvent& event);
		bool OnMouseButtonDown(class MouseButtonDownEvent& event);
		bool OnMouseButtonUp(class MouseButtonUpEvent& event);

		bool OnKeyDown(class KeyDownEvent& event);
		bool OnKeyUp(class KeyUpEvent& event);
		bool OnKeyType(class KeyTypeEvent& event);
	private:
		float m_Time = 0.0f;
	};
}

