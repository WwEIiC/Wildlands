#include "WLPCH.h"
#include "Shader.h"

#include "Wildlands/Renderer/Renderer.h"
#include "Wildlands/Platforms/OpenGL/OpenGLShader.h"

namespace Wildlands
{
	//////////////////////////////////// Shader ///////////////////////////////////////////////
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertPath, const std::string& fragPath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			WL_CORE_ASSERT(false, "RendererAPI is None");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertPath, fragPath);
		}

		WL_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	//////////////////////////////////// ShaderLib ///////////////////////////////////////////////

	void ShaderLib::Add(const Ref<Shader>& shader)
	{
		const std::string& name = shader->GetName();
			
		WL_CORE_ASSERT(!IsExists(name), "Shader already exists");
			
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLib::Load(const std::string& name, const std::string& vertPath, const std::string& fragPath)
	{
		Ref<Shader> shader = Shader::Create(name, vertPath, fragPath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLib::Get(const std::string& name)
	{
		WL_CORE_ASSERT(IsExists(name), "Shader is not exists");

		return m_Shaders[name];
	}

	bool ShaderLib::IsExists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}