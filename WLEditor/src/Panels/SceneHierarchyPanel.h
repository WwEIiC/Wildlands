#pragma once

#include "Wildlands/Core/Base.h"
#include "Wildlands/ECS/Scene.h"
#include "Wildlands/ECS/Entity.h"
#include <functional>
#include <imgui.h>

namespace Wildlands
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
	private:
		void DrawEntityNode(Entity& entity);
		void DrawEntityComponents(Entity& entity);
	public:
		void DrawWindows();
		void ShowSettingsWindow();
		bool m_ShowSettingsWindow = false;
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}

