#pragma once

#include "Wildlands/Core/Base.h"
#include "Wildlands/Core/Log.h"
#include "Wildlands/ECS/Scene.h"
#include "Wildlands/ECS/Entity.h"

namespace Wildlands
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity& entity);
		void DrawEntityComponents(Entity& entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}

