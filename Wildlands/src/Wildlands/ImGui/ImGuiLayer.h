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
		virtual void UIRender() override;

		void Begin();
		void End();
	private:
		void SetImGuiStyle();
	private:
		float m_Time = 0.0f;
	};
}

