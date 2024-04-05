#pragma once
#include <entt.hpp>
#include "Wildlands/ECS/Scene.h"

namespace Wildlands
{
	// the warpper of entt::entity
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity eHandle, Scene* sHandle);

		template<typename Comp>
		bool HasComponent()
		{
			return m_SceneHandle->m_Registry.all_of<Comp>(m_EntityHandle);
		}

		template<typename Comp>
		Comp& GetComponent()
		{
			WL_CORE_ASSERT(HasComponent<Comp>(), "Entity doesn't has this component");

			return m_SceneHandle->m_Registry.get<Comp>(m_EntityHandle);
		}

		template<typename Comp, typename... Args>
		Comp& AddComponent(Args&&... args)
		{
			WL_CORE_ASSERT(!HasComponent<Comp>(), "Entity already has component");

			return m_SceneHandle->m_Registry.emplace<Comp>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename Comp>
		void RemoveComponent()
		{
			WL_CORE_ASSERT(HasComponent<Comp>(), "Entity doesn't has this component");

			m_SceneHandle->m_Registry.remove<Comp>(m_EntityHandle);
		}
		
	public:
		operator bool() const {return m_EntityHandle != entt::null; }

	private:
		entt::entity m_EntityHandle = entt::null; // entt::entity == uint32_t
		Scene* m_SceneHandle = nullptr;
	};
}

