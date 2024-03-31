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
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
		for (auto& entity : group)
		{
			auto tuple = group.get<TransformComponent, SpriteComponent>(entity);
			auto& [transform, sprite] = tuple;

			Renderer2D::DrawQuad(transform, sprite.Color);
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
