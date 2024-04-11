#include "SceneHierarchyPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
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
		m_SelectedEntity = {};
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

		// Right-Click on blank space.
		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				m_Context->CreateEntity("Empty Entity");
			}

			ImGui::EndPopup();
		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectedEntity)
			DrawEntityComponents(m_SelectedEntity);
		ImGui::End();

		DrawWindows();
	}

	static void DrawVec3Controller(const std::string& name, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.f)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[1];

		ImGui::PushID(name.c_str());
		ImGui::Columns(2);

		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) { values.x = resetValue; }
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) { values.y = resetValue; }
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) { values.z = resetValue; }
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	template <typename Comp, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity& entity, UIFunction uiFunc)
	{
		if (entity.HasComponent<Comp>())
		{
			auto& component = entity.GetComponent<Comp>();
			const ImGuiTreeNodeFlags compTreeNodeflags = ImGuiTreeNodeFlags_DefaultOpen 
				| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding 
				| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 availableRegionSize = ImGui::GetContentRegionAvail();
			ImGui::Separator();
			bool isOpened = ImGui::TreeNodeEx((void*)typeid(Comp).hash_code(), compTreeNodeflags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(availableRegionSize.x - lineHeight * 0.5f);
			if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
				ImGui::OpenPopup("ComponentSettings");


			bool toRemove = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component")) { toRemove = true; }
				ImGui::EndPopup();
			}

			if (isOpened)
			{
				uiFunc(component);
				ImGui::TreePop();
			}

			if (toRemove)
				entity.RemoveComponent<Comp>();
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		auto& tagComp = entity.GetComponent<TagComponent>();
		ImGuiTreeNodeFlags flags = ((entity == m_SelectedEntity) ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagComp.Tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		bool deleteEntity = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity")) { deleteEntity = true; }

			ImGui::EndPopup();
		}

		if (isOpened)
		{
			ImGui::TreePop();
		}

		if (deleteEntity) 
		{
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};

			m_Context->DestoryEntity(entity); 
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
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Sprite Renderer"))
			{
				if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
					m_SelectedEntity.AddComponent<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Camera"))
			{
				if (!m_SelectedEntity.HasComponent<CameraComponent>())
					m_SelectedEntity.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& transformComp)
			{
				DrawVec3Controller("Position", transformComp.Position);
				glm::vec3 ratationInDegrees = glm::degrees(transformComp.Rotation);
				DrawVec3Controller("Rotation", ratationInDegrees);
				transformComp.Rotation = glm::radians(ratationInDegrees);
				DrawVec3Controller("Scale", transformComp.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& cameraComp)
			{
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

					float nearClip = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip)) { camera.SetPerspectiveNearClip(nearClip); }

					float farClip = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip)) { camera.SetPerspectiveNearClip(farClip); }
				}
				else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float size = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &size)) { camera.SetOrthographicSize(size); }

					float nearClip = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip)) { camera.SetOrthographicNearClip(nearClip); }

					float farClip = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip)) { camera.SetOrthographicFarClip(farClip); }

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);
				}
			});
		
		DrawComponent<SpriteRendererComponent>("Sprite", entity, [](auto& spriteComp)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(spriteComp.Color));
			});
	}

	void SceneHierarchyPanel::DrawWindows()
	{
		if (m_ShowSettingsWindow)
			ShowSettingsWindow();
	}
	void SceneHierarchyPanel::ShowSettingsWindow()
	{
		ImGui::Begin("Settings", &m_ShowSettingsWindow);
        ImGui::ShowFontSelector("Fonts##Selector");
		ImGui::End();
	}

}
