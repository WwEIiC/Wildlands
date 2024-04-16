#pragma once
#include <entt.hpp>
#include "Wildlands/ECS/Scene.h"

namespace Wildlands
{
	// the warpper of entt::entity
	class Entity
	{
	public:
		static const entt::entity EntityNull = entt::null;

	public:
		Entity() = default;
		Entity(entt::entity eHandle, Scene* sHandle)
			: m_EntityHandle(eHandle), m_SceneHandle(sHandle)
		{
		}


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

			auto& component = m_SceneHandle->m_Registry.emplace<Comp>(m_EntityHandle, std::forward<Args>(args)...);
			m_SceneHandle->OnComponentAdded<Comp>(*this, component);
			return component;
		}

		template<typename Comp>
		void RemoveComponent()
		{
			WL_CORE_ASSERT(HasComponent<Comp>(), "Entity doesn't has this component");

			m_SceneHandle->m_Registry.remove<Comp>(m_EntityHandle);
		}

	public:
		operator bool() const { return m_EntityHandle != EntityNull; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

		bool operator == (const Entity& other) const 
		{ 
			return m_EntityHandle == other.m_EntityHandle && m_SceneHandle == other.m_SceneHandle; 
		}
		bool operator != (const Entity& other) const { return !(*this == other); }

	private:
		entt::entity m_EntityHandle = EntityNull; // entt::entity == uint32_t
		Scene* m_SceneHandle = nullptr;
	};
}

