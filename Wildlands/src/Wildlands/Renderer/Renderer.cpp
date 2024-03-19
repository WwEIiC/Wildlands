#include "WLPCH.h"
#include "Renderer.h"
#include "Renderer2D.h"

#include "Wildlands/Platforms/OpenGL/OpenGLShader.h"

namespace Wildlands
{
    Unique<Renderer::SceneData> Renderer::s_SceneData = CreateUnique<Renderer::SceneData>();

    void Renderer::Init()
    {
        RenderCommand::Init();
        Renderer2D::Init();
    }

    void Renderer::Destory()
    {
        Renderer2D::Destory();
    }

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        s_SceneData->VPMatrix = camera.GetVPMatrix();
    }
    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->Bind();
        shader->SetMat4("u_VPMatrix", s_SceneData->VPMatrix);
        shader->SetMat4("u_Transform", transform);
        vertexArray->Bind();
        RenderCommand::DrawIndex(vertexArray);
    }
    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }
}
