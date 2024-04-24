#pragma once
#include "Wildlands/Renderer/Cameras.h"
#include "Wildlands/Renderer/EditorCamera.h"
#include "Wildlands/Renderer/Texture.h"
#include "Wildlands/ECS/Entity.h"

namespace Wildlands
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Destory();

		static void BeginScene(const Camera& camera, const glm::mat4& cTransform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, uint32_t entityID = (uint32_t)Entity::EntityNull);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f), uint32_t entityID = (uint32_t)Entity::EntityNull);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const glm::vec4& texColor = glm::vec4(1.0f));

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, uint32_t entityID = (uint32_t)Entity::EntityNull);

		static void DrawLine(const glm::vec3& pStart, const glm::vec3& pEnd, const glm::vec4& color, uint32_t entityID = (uint32_t)Entity::EntityNull);
		static float GetLineWidth();
		static void SetLineWidth(float width);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, uint32_t entityID = (uint32_t)Entity::EntityNull);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, uint32_t entityID = (uint32_t)Entity::EntityNull);

		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& spriteComp, uint32_t entityID);
	public:
		struct Stats
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			uint32_t GetVertexCount() const { return quadCount * 4; }
			uint32_t GetIndexCount() const { return quadCount * 6; }
		};

		static Stats& GetStats();
		static void ResetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
}


