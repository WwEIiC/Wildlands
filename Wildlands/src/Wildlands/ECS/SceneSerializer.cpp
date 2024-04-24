#include "WLPCH.h"
#include "SceneSerializer.h"
#include "Wildlands/ECS/Entity.h"

#include <yaml-cpp/yaml.h>
#include "Wildlands/ECS/Components.h"

namespace YAML
{
	// add the encode and decode of glm::vec3 and glm::vec4 
	// so that can use xxx.as<glm::vec3>();
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
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
			node.SetStyle(EmitterStyle::Flow);
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
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
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

	static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::BodyType::Static:    return "Static";
		case Rigidbody2DComponent::BodyType::Dynamic:   return "Dynamic";
		case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		WL_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")    return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		WL_CORE_ASSERT(false, "Unknown body type");
		return Rigidbody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		WL_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Every entity must have id component");

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
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
			out << YAML::Key << "Position"	<< YAML::Value << transformComp.Position;
			out << YAML::Key << "Rotation"	<< YAML::Value << transformComp.Rotation;
			out << YAML::Key << "Scale"		<< YAML::Value << transformComp.Scale;

			out << YAML::EndMap; // TransformComponent
		}
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent" << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComp = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComp.Color;

			out << YAML::EndMap; // SpriteRendererComponent
		} 
		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent" << YAML::BeginMap; // CircleRendererComponent

			auto& circleRendererComp = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color"		<< YAML::Value << circleRendererComp.Color;
			out << YAML::Key << "Thickness"	<< YAML::Value << circleRendererComp.Thickness;
			out << YAML::Key << "Fade"		<< YAML::Value << circleRendererComp.Fade;

			out << YAML::EndMap; // CircleRendererComponent
		}
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent" << YAML::BeginMap; // CameraComponent

			auto& cameraComp = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComp.Camera;

			out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType"	<< YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV"	<< YAML::Value << camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNear"	<< YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar"	<< YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize"	<< YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear"	<< YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar"	<< YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera


			out << YAML::Key << "Primary"			<< YAML::Value << cameraComp.Primary;
			out << YAML::Key << "FixedAspectRatio"	<< YAML::Value << cameraComp.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent" << YAML::BeginMap; // Rigidbody2DComponent

			auto& rb2dComp = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType"		<< YAML::Value << RigidBody2DBodyTypeToString(rb2dComp.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComp.FixedRotation;

			out << YAML::EndMap; // Rigidbody2DComponent
		}
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent" << YAML::BeginMap; // BoxCollider2DComponent

			auto& bc2dComp = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset"				<< YAML::Value << bc2dComp.Offset;
			out << YAML::Key << "Size"					<< YAML::Value << bc2dComp.Size;
			out << YAML::Key << "Density"				<< YAML::Value << bc2dComp.Density;
			out << YAML::Key << "Friction"				<< YAML::Value << bc2dComp.Friction;
			out << YAML::Key << "Restitution"			<< YAML::Value << bc2dComp.Restitution;
			out << YAML::Key << "RestitutionThreshold"	<< YAML::Value << bc2dComp.RestitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
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
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Scene"]) { return false; }

		std::string sceneName = data["Scene"].as<std::string>();
		WL_CORE_TRACE("Deserialize Scene {0}", sceneName);

		YAML::Node entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid  = entity["Entity"].as<uint64_t>();

				std::string name;
				if (entity["TagComponent"])
					name = entity["TagComponent"]["Tag"].as<std::string>();

				Entity targetEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformNode = entity["TransformComponent"];
				if (transformNode)
				{
					// Every Entity has the TransformComponent.
					auto& tc = targetEntity.GetComponent<TransformComponent>();
					tc.Position = transformNode["Position"].as<glm::vec3>();
					tc.Rotation = transformNode["Rotation"].as<glm::vec3>();
					tc.Scale	= transformNode["Scale"].as<glm::vec3>();
				}

				auto spriteRendererNode = entity["SpriteRendererComponent"];
				if (spriteRendererNode)
				{
					auto& src = targetEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererNode["Color"].as<glm::vec4>();
				}

				auto circleRendererNode = entity["CircleRendererComponent"];
				if (circleRendererNode)
				{
					auto& crc = targetEntity.AddComponent<CircleRendererComponent>();
					crc.Color		= circleRendererNode["Color"].as<glm::vec4>();
					crc.Thickness	= circleRendererNode["Thickness"].as<float>();
					crc.Fade		= circleRendererNode["Fade"].as<float>();
				}

				auto cameraNode = entity["CameraComponent"];
				if (cameraNode)
				{
					auto& cameraComp = targetEntity.AddComponent<CameraComponent>();

					auto camera = cameraNode["Camera"];

					cameraComp.Camera.SetProjectionType((SceneCamera::ProjectionType)camera["ProjectionType"].as<int>());

					cameraComp.Camera.SetPerspectiveFOV(camera["PerspectiveFOV"].as<float>());
					cameraComp.Camera.SetPerspectiveNearClip(camera["PerspectiveNear"].as<float>());
					cameraComp.Camera.SetPerspectiveFarClip(camera["PerspectiveFar"].as<float>());

					cameraComp.Camera.SetOrthographicSize(camera["OrthographicSize"].as<float>());
					cameraComp.Camera.SetOrthographicNearClip(camera["OrthographicNear"].as<float>());
					cameraComp.Camera.SetOrthographicFarClip(camera["OrthographicFar"].as<float>());

					cameraComp.Primary = cameraNode["Primary"].as<bool>();
					cameraComp.FixedAspectRatio = cameraNode["FixedAspectRatio"].as<bool>();
				}

				auto rigidbody2DNode = entity["Rigidbody2DComponent"];
				if (rigidbody2DNode)
				{
					auto& rb2dComp = targetEntity.AddComponent<Rigidbody2DComponent>();
					rb2dComp.Type			= RigidBody2DBodyTypeFromString(rigidbody2DNode["BodyType"].as<std::string>());
					rb2dComp.FixedRotation	= rigidbody2DNode["FixedRotation"].as<bool>();
				}

				auto boxCollider2DNode = entity["BoxCollider2DComponent"];
				if (boxCollider2DNode)
				{
					auto& bc2dComp = targetEntity.AddComponent<BoxCollider2DComponent>();
					bc2dComp.Offset					= boxCollider2DNode["Offset"].as<glm::vec2>();
					bc2dComp.Size					= boxCollider2DNode["Size"].as<glm::vec2>();
					bc2dComp.Density				= boxCollider2DNode["Density"].as<float>();
					bc2dComp.Friction				= boxCollider2DNode["Friction"].as<float>();
					bc2dComp.Restitution			= boxCollider2DNode["Restitution"].as<float>();
					bc2dComp.RestitutionThreshold	= boxCollider2DNode["RestitutionThreshold"].as<float>();
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
