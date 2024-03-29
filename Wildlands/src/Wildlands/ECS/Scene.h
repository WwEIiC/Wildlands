#pragma once

#include "entt.hpp"

namespace Wildlands
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		entt::registry& Registry() { return m_Registry; }

		void OnUpdate(float deltaTime);
		void OnRender();
	};
}

