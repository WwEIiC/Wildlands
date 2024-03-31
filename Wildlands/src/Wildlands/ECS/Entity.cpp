#include "WLPCH.h"
#include "Entity.h"

namespace Wildlands
{
	Entity::Entity(entt::entity eHandle, Scene* sHandle)
		: m_EntityHandle(eHandle), m_SceneHandle(sHandle)
	{
	}
}
