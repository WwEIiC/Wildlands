#pragma once

#include "Wildlands/Core/Timestep.h"
#include "Wildlands/Renderer/EditorCamera.h"
#include "Wildlands/Core/UUID.h"

#include <entt.hpp>
class b2World;

namespace Wildlands
{
	class Entity;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		void OnRuntimeStart();
		void OnRuntimeStop();

		void UpdateEditor(Timestep ts, EditorCamera& camera);
		void UpdateRuntime(Timestep ts);
		void UIRender();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestoryEntity(Entity entity);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
	private:
		template <typename Comp>
		void OnComponentAdded(Entity entity, Comp& comp);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

	friend class Entity;
	friend class SceneHierarchyPanel;
	friend class SceneSerializer;
	};
}

