#pragma once

#include <string>
#include <unordered_map>

namespace Wildlands
{
	class Shader
	{
	public:
		static Ref<Shader> Create(const std::string& name, const std::string& vertPath, const std::string& fragPath);
		virtual ~Shader() = default;

		virtual const std::string& GetName() const = 0;
		virtual void Bind() const = 0;
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
