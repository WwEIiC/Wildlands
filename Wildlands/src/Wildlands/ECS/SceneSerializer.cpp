#include "WLPCH.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>
#include "Wildlands/ECS/Components.h"

namespace YAML
{
	// add the encode and decode of glm::vec3 and glm::vec4 
	// so that can use xxx.as<glm::vec3>();
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Wildlands
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << "12345"; // WLTODO: Entity ID

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent" << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent" << YAML::BeginMap; // TransformComponent

			auto& transformComp = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transformComp.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transformComp.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComp.Scale;

			out << YAML::EndMap; // TransformComponent
		}
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent" << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComp = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComp.Color;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent <CameraComponent>())
		{
			out << YAML::Key << "CameraComponent" << YAML::BeginMap; // CameraComponent

			auto& cameraComp = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComp.Camera;

			out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera


			out << YAML::Key << "Primary" << YAML::Value << cameraComp.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComp.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}
		out << YAML::EndMap; // Entity
	}
	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.view<TagComponent>().each([&](auto entityID, auto& tagComp)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (entity)
					SerializeEntity(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
		WL_CORE_TRACE("Succeed Serialize Scene {0} to {1}", "Untitled", filepath);
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
	}
	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		if (!data["Scene"]) { return false; }

		std::string sceneName = data["Scene"].as<std::string>();
		WL_CORE_TRACE("Deserialize Scene {0}", sceneName);

		YAML::Node entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid  = entity["Entity"].as<uint64_t>(); // WLTODO: Entity ID

				std::string name;
				if (entity["TagComponent"])
					name = entity["TagComponent"]["Tag"].as<std::string>();

				Entity targetEntity = m_Scene->CreateEntity(name);

				auto transformComp = entity["TransformComponent"];
				if (transformComp)
				{
					// Every Entity has the TransformComponent.
					auto& tc = targetEntity.GetComponent<TransformComponent>();
					tc.Position = transformComp["Position"].as<glm::vec3>();
					tc.Rotation = transformComp["Rotation"].as<glm::vec3>();
					tc.Scale = transformComp["Scale"].as<glm::vec3>();
				}

				auto spriteRendererComp = entity["SpriteRendererComponent"];
				if (spriteRendererComp)
				{
					auto& src = targetEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComp["Color"].as<glm::vec4>();
				}

				auto cameraComp = entity["CameraComponent"];
				if (cameraComp)
				{
					auto& cc = targetEntity.AddComponent<CameraComponent>();

					auto camera = cameraComp["Camera"];

					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)camera["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveFOV(camera["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(camera["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(camera["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(camera["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(camera["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(camera["OrthographicFar"].as<float>());

					cc.Primary = cameraComp["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComp["FixedAspectRatio"].as<bool>();
				}
			}
		}
		return true;
	}
	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		return false;
	}
}
