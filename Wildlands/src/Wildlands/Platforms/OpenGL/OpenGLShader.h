#pragma once

#include "Wildlands/Renderer/Shader.h"
#include <glm/glm.hpp>

namespace Wildlands
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
		virtual ~OpenGLShader() override;

		virtual const std::string& GetName() const override { return m_Name; }
		virtual void Bind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		void SetUniformInt(const std::string& name, int value);
		void SetUniformFloat(const std::string& name, float value);
		void SetUniformFloat2(const std::string& name, const glm::vec2& value);
		void SetUniformFloat3(const std::string& name, const glm::vec3& value);
		void SetUniformFloat4(const std::string& name, const glm::vec4& value);
		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		uint32_t m_RendererID;//the id of shaderprogram
		std::string m_Name;
	};
}
