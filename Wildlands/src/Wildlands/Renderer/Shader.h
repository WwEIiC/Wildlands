#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Wildlands
{
	class Shader
	{
	public:
		Shader(const std::string& vertSrc, const std::string& fragSrc);
		virtual ~Shader();

		virtual void Bind() const;

		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		uint32_t m_RendererID;//the id of shaderprogram
	};
}
