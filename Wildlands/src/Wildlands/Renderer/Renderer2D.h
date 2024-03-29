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

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f));

	public:
		struct Stats
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			uint32_t GetVertexCount() { return quadCount * 4; }
			uint32_t GetIndexCount() { return quadCount * 6; }
		};

		static Stats& GetStats();
		static void ResetStats();

	private:
		static void NextBatch();
	};
}


