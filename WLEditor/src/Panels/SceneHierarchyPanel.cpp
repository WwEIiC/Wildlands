#include "SceneHierarchyPanel.h"
#include "Wildlands/ECS/Components.h"
#include "Wildlands/Scripting/ScriptEngine.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>


namespace Wildlands
{
    extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
		m_SelectedEntity = {};
		if (!m_DefaultTexture)
			m_DefaultTexture = Texture2D::Create("assets/Textures/Checkerboard.png");
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene HierarchyPanel");

		if (m_Context)
		{
			m_Context->m_Registry.view<TransformComponent>().each([&](auto entityID, TransformComponent& transformComp)
				{
					Entity entity{ entityID, m_Context.get() };
					DrawEntityNode(entity);
				});

			if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) { m_SelectedEntity = {}; }

			// Right-Click on blank space.
			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
			{
				if (ImGui::MenuItem("Create Entity"))
				{
					m_Context->CreateEntity("Empty Entity");
				}

				ImGui::EndPopup();
			}
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

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
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
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
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
			DrawAddComponentItem<CameraComponent>("Camera");
			DrawAddComponentItem<ScriptComponent>("Script");
			DrawAddComponentItem<SpriteRendererComponent>("Sprite Renderer");
			DrawAddComponentItem<CircleRendererComponent>("Circle Renderer");
			DrawAddComponentItem<Rigidbody2DComponent>("Rigidbody 2D");
			DrawAddComponentItem<BoxCollider2DComponent>("Box Collider 2D");
			DrawAddComponentItem<CircleCollider2DComponent>("Circle Collider 2D");
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

		DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& scriptComp) mutable
		{
			bool scriptClassExists = ScriptEngine::EntityClassExists(scriptComp.ClassName);

			static char buffer[64];
			strcpy_s(buffer, sizeof(buffer), scriptComp.ClassName.c_str());

			if (!scriptClassExists)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

			if (ImGui::InputText("Class", buffer, sizeof(buffer)))
				scriptComp.ClassName = buffer;

			// Fields
			bool sceneRunning = scene->IsRunning();
			if (sceneRunning)
			{
				Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
				if (scriptInstance)
				{
					// Set the data in editor UI to the script instance
					const auto& classFields = scriptInstance->GetScriptClass()->GetFields();
					for (const auto& [name, field] : classFields)
					{
						if (field.Type == ScriptFieldType::Float)
						{
							float data = scriptInstance->GetFieldValue<float>(name);
							if (ImGui::DragFloat(name.c_str(), &data))
							{
								scriptInstance->SetFieldValue(name, data);
							}
						}
						//...other data type.
					}
				}
			}
			else
			{
				if (scriptClassExists)
				{
					Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComp.ClassName);
					const auto& classFields = entityClass->GetFields();

					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

					// retrieve all the fields from class data:
					// if the field had scriptFieldInstance then dispaly the controller to it
					// else create a scriptFieldInstance for this field.
					for (const auto& [name, field] : classFields)
					{
						// Field has been set in editor
						if (entityFields.find(name) != entityFields.end())
						{
							ScriptFieldInstance& scriptField = entityFields.at(name);

							// Display control to set it.
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptField.GetValue<float>();
								if (ImGui::DragFloat(name.c_str(), &data))
									scriptField.SetValue(data);
							}
						}
						else
						{
							// Create scriptFieldInstance and Display control to set it.
							if (field.Type == ScriptFieldType::Float)
							{
								float data = 0.0f;
								if (ImGui::DragFloat(name.c_str(), &data))
								{
									ScriptFieldInstance& fieldInstance = entityFields[name];
									fieldInstance.Field = field;
									fieldInstance.SetValue(data);
								}
							}
						}
					}
				}
			}

			if (!scriptClassExists)
				ImGui::PopStyleColor();
		});
		
		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [&](auto& spriteComp)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(spriteComp.Color));

			ImGui::Text("Texture");
			uint32_t textureID = spriteComp.Texture ? spriteComp.Texture->GetRendererID() : m_DefaultTexture->GetRendererID();
			ImGui::Image((void*)(uint64_t)textureID, ImVec2(64.0f, 64.0f), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
					if (texture->IsLoaded())
						spriteComp.Texture = texture;
					else
						WL_CORE_WARN("Could not load texture {0}", texturePath.filename().string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			if (ImGui::Button("Remove"))
				spriteComp.Texture = nullptr;

			ImGui::DragFloat("Tiling Factor", &spriteComp.TilingFactor, 0.1f, 0.0f, 100.0f, "%.2f");
		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [&](auto& circleComp)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(circleComp.Color));
			ImGui::DragFloat("Thickness", &circleComp.Thickness, 0.025f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Fade", &circleComp.Fade, 0.00025f, 0.0f, 1.0f, "%.3f");
		});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& rb2dComp)
		{
			const char* rigidbodyTypeStrings[] = { "Static", "Dynamic", "Kinematic"};
			const char* currentRBTypeString = rigidbodyTypeStrings[(int)rb2dComp.Type];

			if (ImGui::BeginCombo("Body Type", currentRBTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = rigidbodyTypeStrings[i] == currentRBTypeString;
					if (ImGui::Selectable(rigidbodyTypeStrings[i], isSelected))
					{
						currentRBTypeString = rigidbodyTypeStrings[i];
						rb2dComp.Type = (Rigidbody2DComponent::BodyType)i;
					}

					if (isSelected) { ImGui::SetItemDefaultFocus(); }
				}
				ImGui::EndCombo();
			}

			ImGui::Checkbox("FixedRotation", &rb2dComp.FixedRotation);
		});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& bc2dComp)
		{
			ImGui::DragFloat2("Size", glm::value_ptr(bc2dComp.Size), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat2("Offset", glm::value_ptr(bc2dComp.Offset), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat("Density", &bc2dComp.Density, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Friction", &bc2dComp.Friction , 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Restitution", &bc2dComp.Restitution, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Restitution Threshold", &bc2dComp.RestitutionThreshold, 0.01f, 0.0f, 0.0f, "%.2f");
		});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& cc2dComp)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(cc2dComp.Offset), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat("Radius", &cc2dComp.Radius, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat("Density", &cc2dComp.Density, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Friction", &cc2dComp.Friction, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Restitution", &cc2dComp.Restitution, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Restitution Threshold", &cc2dComp.RestitutionThreshold, 0.01f, 0.0f, 0.0f, "%.2f");
		});
	}

	template<typename Comp>
	void SceneHierarchyPanel::DrawAddComponentItem(const std::string& name)
	{
		if (!m_SelectedEntity.HasComponent<Comp>())
		{
			if (ImGui::MenuItem(name.c_str()))
			{
				m_SelectedEntity.AddComponent<Comp>();
				ImGui::CloseCurrentPopup();
			}
		}
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
