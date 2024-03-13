#include "WLPCH.h"
#include "Renderer.h"

#include "Wildlands/Platforms/OpenGL/OpenGLShader.h"

namespace Wildlands
{
    glm::mat4 Renderer::s_VPMatrix = glm::mat4(1.0f);

    void Renderer::Init()
    {
        RenderCommand::Init();
    }

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        s_VPMatrix = camera.GetVPMatrix();
    }
    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->Bind();
        std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("u_VPMatrix", s_VPMatrix);
        std::dynamic_pointer_cast<OpenGLShader>(shader)->SetUniformMat4("u_Transform", transform);
        vertexArray->Bind();
        RenderCommand::DrawIndex(vertexArray);
    }
    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }
}
