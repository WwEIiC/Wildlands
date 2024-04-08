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

	private:
		void DrawEntityNode(Entity& entity);
		void DrawEntityComponents(Entity& entity);

		template <typename Comp>
		void DrawComponent(const std::string& name, Entity& entity, std::function<void()> func)
		{
			if (entity.HasComponent<Comp>())
			{
				auto& component = entity.GetComponent<Comp>();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
				if (ImGui::TreeNodeEx((void*)typeid(Comp).hash_code(), flags, name.c_str()))
				{
					func();
					ImGui::TreePop();
				}
			}
		}
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}

