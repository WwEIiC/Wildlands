#include "WLPCH.h"
#include "Scene.h"

#include "Wildlands/ECS/Components.h"
#include "Wildlands/ECS/Entity.h"

#include "Wildlands/Renderer/Renderer2D.h"

namespace Wildlands
{
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}

	void Scene::Update(Timestep ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;

		{
			//auto group = m_Registry.group<TransformComponent>(entt::get<CameraComponent>);
			auto group = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto& entity : group)
			{
				const auto& [transform, camera] = (group.get<TransformComponent, CameraComponent>(entity));
				if (camera.IsMain)
				{
					mainCamera = &camera.Camera;
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, *cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
			for (auto& entity : group)
			{
				const auto& [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);

				Renderer2D::DrawQuad(transform, sprite.Color);
			}

			Renderer2D::EndScene();
		}
	}
	void Scene::UIRender()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		std::string tag = name.empty() ? "Entity" : name;
		entity.AddComponent<TagComponent>(tag);
		return entity;
	}
}
