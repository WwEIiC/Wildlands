#pragma once
#include "Wildlands/ECS/Entity.h"

namespace Wildlands
{
	class ScriptableEntity
	{
	public:
		ScriptableEntity() = default;
		virtual ~ScriptableEntity() = default;

		template<typename Comp>
		bool HasComponent()
		{
			return m_Entity.HasComponent<Comp>();
		}

		template<typename Comp>
		Comp& GetComponent()
		{
			WL_CORE_ASSERT(HasComponent<Comp>(), "Entity doesn't has this component");

			return m_Entity.GetComponent<Comp>();
		}

		template<typename Comp, typename... Args>
		Comp& AddComponent(Args&&... args)
		{
			WL_CORE_ASSERT(!HasComponent<Comp>(), "Entity already has component");

			return m_Entity.AddComponent<Comp>();
		}

		template<typename Comp>
		void RemoveComponent()
		{
			WL_CORE_ASSERT(HasComponent<Comp>(), "Entity doesn't has this component");

			m_Entity.RemoveComponent<Comp>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestory() {}
		virtual void OnUpdate(Timestep ts) {}

	private:
		Entity m_Entity;

	friend class Scene;
	};
}

