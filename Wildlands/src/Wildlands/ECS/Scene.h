#pragma once

#include <entt.hpp>
#include "Wildlands/Core/Timestep.h"

namespace Wildlands
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Update(Timestep ts);
		void UIRender();

		Entity CreateEntity(const std::string& name = std::string());

	private:
		entt::registry m_Registry;

	friend class Entity;
	};
}

