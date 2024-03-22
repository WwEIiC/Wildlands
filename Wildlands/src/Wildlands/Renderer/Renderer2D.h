#pragma once
#include "Wildlands/Renderer/Cameras.h"
#include "Wildlands/Renderer/Texture.h"

namespace Wildlands
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Destory();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f));
	};
}


