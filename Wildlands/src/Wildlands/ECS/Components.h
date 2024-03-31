#pragma once

#include <glm/glm.hpp>

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
}
