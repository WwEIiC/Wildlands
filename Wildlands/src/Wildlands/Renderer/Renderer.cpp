#include "WLPCH.h"
#include "Renderer.h"

namespace Wildlands
{
    glm::mat4 Renderer::s_VPMatrix = glm::mat4(1.0f);

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        s_VPMatrix = camera.GetVPMatrix();
    }
    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
    {
        shader->Bind();
        shader->SetUniformMat4("u_VPMatrix", s_VPMatrix);
        vertexArray->Bind();
        RenderCommand::DrawIndex(vertexArray);
    }
}
