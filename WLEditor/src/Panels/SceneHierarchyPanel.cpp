#include "SceneHierarchyPanel.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Wildlands/ECS/Components.h"

namespace Wildlands
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene HierarchyPanel");

		m_Context->m_Registry.view<TransformComponent>().each([&](auto entityID, TransformComponent& transformComp)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()){ m_SelectedEntity = {};}

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectedEntity)
		{
			DrawEntityComponents(m_SelectedEntity);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		auto& tagComp = entity.GetComponent<TagComponent>();
		ImGuiTreeNodeFlags flags = ((entity == m_SelectedEntity) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow;

		bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagComp.Tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		if (isOpened)
		{
			ImGui::TreePop();
		}

	}

	void SceneHierarchyPanel::DrawEntityComponents(Entity& entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.HasComponent<TransformComponent>())
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), flags, "Transform"))
			{
				auto& transform = entity.GetComponent<TransformComponent>().Transform;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), flags, "Camera"))
			{
				auto& cameraComp = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComp.Camera;

				ImGui::Checkbox("Primary", &cameraComp.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = projectionTypeStrings[i] == currentProjectionTypeString;
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected) { ImGui::SetItemDefaultFocus(); }
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float fov = glm::degrees(camera.GetPerspectiveFOV());
					if (ImGui::DragFloat("FOV", &fov)) { camera.SetPerspectiveFOV(glm::radians(fov)); }

					float nearClip = camera.GetPerspectiveNear();
					if (ImGui::DragFloat("Near Clip", &nearClip)) { camera.SetPerspectiveNear(nearClip); }

					float farClip = camera.GetPerspectiveFar();
					if (ImGui::DragFloat("Far Clip", &farClip)) { camera.SetPerspectiveNear(farClip); }
				}
				else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float size = camera.GetOrthoSize();
					if (ImGui::DragFloat("Size", &size)) { camera.SetOrthoSize(size); }

					float nearClip = camera.GetOrthoNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip)) { camera.SetOrthoNearClip(nearClip); }

					float farClip = camera.GetOrthoFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip)) { camera.SetOrthoFarClip(farClip); }

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);
				}

				ImGui::TreePop();
			}
		}
	}
}
