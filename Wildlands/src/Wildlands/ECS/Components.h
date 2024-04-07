#pragma once

#include <glm/glm.hpp>

#include "Wildlands/Renderer/Cameras.h"
#include "Wildlands/ECS/SceneCamera.h"
#include "Wildlands/ECS/ScriptableEntity.h"

namespace Wildlands
{
	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent& transformComp) = default;
		TransformComponent(const glm::mat4& transform)
			: Transform(transform) {}

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent& tagComp) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct SpriteComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent& spriteComp) = default;
		SpriteComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& spriteComp) = default;
	};

	/// <summary>
	/// use cpp to script
	/// </summary>
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void(*DestoryScript)(ScriptableEntity*);

		template <typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestoryScript = [](ScriptableEntity* instance) { delete instance; instance = nullptr; };
		}
	};
}
