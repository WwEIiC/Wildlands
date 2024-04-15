#include "WLPCH.h"
#include "Scene.h"

#include "Wildlands/ECS/Components.h"
#include "Wildlands/ECS/Entity.h"

#include "Wildlands/Renderer/Renderer2D.h"

namespace Wildlands
{
	void Scene::UpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);
		}

		Renderer2D::EndScene();
	}
	void Scene::UpdateRuntime(Timestep ts)
	{
		// update scriptable entites
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& NSComp)
				{
					if (!NSComp.Instance)
					{
						NSComp.Instance = NSComp.InstantiateScript();
						NSComp.Instance->m_Entity = Entity(entity, this);

						NSComp.Instance->OnCreate();
					}
					NSComp.Instance->OnUpdate(ts);
				});
		}


		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = (view.get<TransformComponent, CameraComponent>(entity));
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

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

		std::string tag = name.empty() ? "Entity" : name;
		entity.AddComponent<TagComponent>(tag);

		entity.AddComponent<TransformComponent>();
		return entity;
	}

	void Scene::DestoryEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComp = view.get<CameraComponent>(entity);
			if (!cameraComp.FixedAspectRatio)
			{
				cameraComp.Camera.SetViewportSize(width, height);
			}
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto cameraComp = view.get<CameraComponent>(entity);
			if (cameraComp.Primary)
				return Entity(entity, this);
		}
		return Entity{};
	}


	template<typename Comp>
	void Scene::OnComponentAdded(Entity entity, Comp& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
}
