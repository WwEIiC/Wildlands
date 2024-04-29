#pragma once

#include "Wildlands/Core/UUID.h"
#include "Wildlands/Core/Timestep.h"
#include "Wildlands/Renderer/EditorCamera.h"

#include <entt.hpp>
class b2World;

namespace Wildlands
{
	class Entity;

	class Scene
	{
	public:
		Scene() = default;
		~Scene();

		static Ref<Scene>Copy(Ref<Scene> other);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void UpdateEditor(Timestep ts, EditorCamera& camera);
		void UpdateRuntime(Timestep ts);
		void UpdateSimulation(Timestep ts, EditorCamera& camera);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestoryEntity(Entity entity);

		Entity DupilcateEntity(Entity entity);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetEntityByUUID(UUID uuid);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		void RenderScene(EditorCamera& camera);

		void Physics2DStart();
		void Physics2DStop();

		template <typename Comp>
		void OnComponentAdded(Entity entity, Comp& comp);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

	friend class Entity;
	friend class SceneHierarchyPanel;
	friend class SceneSerializer;
	};
}

