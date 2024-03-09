#include "WLPCH.h"
#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Wildlands
{
	Shader::Shader(const std::string& vertSrc, const std::string& fragSrc)
	{
		//set up vertex shader
		uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const char* srouce = vertSrc.c_str();
		glShaderSource(vertexShader, 1, &srouce, 0);

		glCompileShader(vertexShader);
		int succeed = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &succeed);
		if (!succeed)
		{
			int length = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(vertexShader, length, &length, message);
			WL_CORE_ERROR(message);
			WL_CORE_ASSERT(false, "Failed to compile vertex shader");
			glDeleteShader(vertexShader);
			return;
		}

		//set up fragment shader
		uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		srouce = fragSrc.c_str();
		glShaderSource(fragmentShader, 1, &srouce, 0);

		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &succeed);
		if (!succeed)
		{
			int length = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(fragmentShader, length, &length, message);
			WL_CORE_ERROR(message);
			WL_CORE_ASSERT(false, "Failed to compile fragment shader");
			glDeleteShader(fragmentShader);
			return;
		}

		//set up the shader program
		m_RendererID = glCreateProgram();

		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);

		glLinkProgram(m_RendererID);
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &succeed);
		if (!succeed)
		{
			int length = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetProgramInfoLog(m_RendererID, length, &length, message);
			WL_CORE_ERROR(message);
			WL_CORE_ASSERT(false, "Failed to link shader program");
			glDeleteProgram(m_RendererID);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}