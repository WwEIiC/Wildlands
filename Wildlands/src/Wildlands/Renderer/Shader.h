#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Wildlands
{
	class Shader
	{
	public:
		static Ref<Shader> Create(const std::string& name, const std::string& vertPath, const std::string& fragPath);
		virtual ~Shader() = default;

		virtual const std::string& GetName() const = 0;
		virtual void Bind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
	};

	class ShaderLib
	{
	public:
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& name, const std::string& vertPath, const std::string& fragPath);

		Ref<Shader> Get(const std::string& name);

		bool IsExists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}
