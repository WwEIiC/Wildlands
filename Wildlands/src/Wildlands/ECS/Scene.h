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

	public:
		void OnViewportResize(uint32_t width, uint32_t height);

	private:
		entt::registry m_Registry;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	friend class Entity;
	friend class SceneHierarchyPanel;
	};
}

