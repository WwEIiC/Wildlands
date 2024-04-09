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
				ImGuiTreeNodeFlags compTreeNodeflags = ImGuiTreeNodeFlags_DefaultOpen
													| ImGuiTreeNodeFlags_AllowItemOverlap;

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				bool isOpened = ImGui::TreeNodeEx((void*)typeid(Comp).hash_code(), compTreeNodeflags, name.c_str());
				ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
				if (ImGui::Button("...", ImVec2{ 20, 20 }))
					ImGui::OpenPopup("ComponentSettings");
				ImGui::PopStyleVar();

				bool toRemove = false;
				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component")) { toRemove = true; }
					ImGui::EndPopup();
				}

				if (isOpened)
				{
					func();
					ImGui::TreePop();
				}

				if (toRemove)
					entity.RemoveComponent<Comp>();
			}
		}
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}

