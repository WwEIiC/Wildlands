#include "WLPCH.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Wildlands
{
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertPath, const std::string& fragPath)
		: m_Name(name)
	{
		WL_PROFILE_FUNCTION();

		//set up vertex shader
		uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);

		std::ifstream vFile(vertPath), fFile(fragPath);

		if (!vFile.is_open())
		{
			WL_CORE_ERROR("Failed to open file: {0}", vertPath);
		}
		if (!fFile.is_open())
		{
			WL_CORE_ERROR("Failed to open file: {0}", fragPath);
		}

		std::stringstream vBuffer, fBuffer;
		{
			WL_PROFILE_SCOPE("Reading shader files - OpenGLShader::OpenGLShader()");
			vBuffer << vFile.rdbuf();
			fBuffer << fFile.rdbuf();
		}
		std::string vertSrc = vBuffer.str(), fragSrc = fBuffer.str();


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
	OpenGLShader::~OpenGLShader()
	{
		WL_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		WL_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		WL_PROFILE_FUNCTION();
		SetUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		WL_PROFILE_FUNCTION();
		
		SetUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		WL_PROFILE_FUNCTION();
		SetUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		WL_PROFILE_FUNCTION();
		SetUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		WL_PROFILE_FUNCTION();
		SetUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		WL_PROFILE_FUNCTION();
		SetUniformMat4(name, value);
	}

	void OpenGLShader::SetUniformInt(const std::string& name, int value)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::SetUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::SetUniformFloat(const std::string& name, float value)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::SetUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
