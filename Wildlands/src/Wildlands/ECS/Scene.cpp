#include "WLPCH.h"
#include "Scene.h"

#include "Wildlands/ECS/Components.h"
#include "Wildlands/ECS/Entity.h"
#include "Wildlands/ECS/ScriptableEntity.h"

#include "Wildlands/Renderer/Renderer2D.h"

#include "Wildlands/Scripting/ScriptEngine.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Wildlands
{
	static b2BodyType RigidbodyTypeToB2BodyType(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static: return b2BodyType::b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2BodyType::b2_kinematicBody;
		}
		WL_CORE_ASSERT(false, "Unkown body type");
		return b2BodyType::b2_staticBody;
	}

	template<typename... Components>
	static void CopyComponent(Entity dstEntity, Entity srcEntity)
	{
		([&]()
			{
				if (!srcEntity.HasComponent<Components>()) { return; }
				dstEntity.AddOrReplaceComponent<Components>(srcEntity.GetComponent<Components>());
			}(), ...);
	}

	template<typename... Components>
	static void CopyComponent(ComponentGroup<Components...>, Entity dstEntity, Entity srcEntity)
	{
		CopyComponent<Components...>(dstEntity, srcEntity);
	}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		// Copy viewport data.
		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		// Copy entities into new scene.
		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto eneityID : idView)
		{
			Entity srcEntity = { eneityID, other.get() };
			UUID uuid = srcEntity.GetUUID();
			const std::string& name = srcEntity.GetComponent<TagComponent>().Tag;
			Entity dstEntity = newScene->CreateEntityWithUUID(uuid, name);

			// Copy all components into new entity.
			CopyComponent(AllComponents{}, dstEntity, srcEntity);
		}
		return newScene;
	}

	void Scene::OnRuntimeStart()
	{
		// Physics
		Physics2DStart();

		// Scripting
		{
			ScriptEngine::OnRuntimeStart(this);
			// Instantiate all script entities

			auto view = m_Registry.view<ScriptComponent>();
			for (auto entityID : view)
			{
				Entity entity = { entityID, this };
				ScriptEngine::CreateEntity(entity);
			}
		}
	}
	void Scene::OnRuntimeStop()
	{
		// Physics
		Physics2DStop();

		// Scipting
		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		// Physics
		Physics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		// Physics
		Physics2DStop();
	}

	void Scene::UpdateEditor(Timestep ts, EditorCamera& camera)
	{
		// Render
		RenderScene(camera);
	}
	void Scene::UpdateRuntime(Timestep ts)
	{
		// -- Update Order: Scripts --> Physics --> Render

		// ------- C# Entity ----------
		auto view = m_Registry.view<ScriptComponent>();
		for (auto entityID : view)
		{
			Entity entity = { entityID, this };
			ScriptEngine::UpdateEntity(entity, ts);
		}

		// ------ Native Scripts --------
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& NSComp)
				{
					if (!NSComp.Instance)
					{
						NSComp.Instance = NSComp.InstantiateScript();
						NSComp.Instance->m_Entity = Entity(entity, this);

						NSComp.Instance->OnCreate();
					}
					NSComp.Instance->OnUpdate(ts);
				});
		}

		// -------- Physics --------
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// update tranform from physics.
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto entityID : view)
			{
				Entity entity = { entityID, this };
				auto& transformComp = entity.GetComponent<TransformComponent>();
				auto& rb2dComp = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = static_cast<b2Body*>(rb2dComp.RuntimeRigidbody);
				const b2Vec2& position = body->GetPosition();
				transformComp.Position.x = position.x;
				transformComp.Position.y = position.y;
				transformComp.Rotation.z = body->GetAngle();
			}
		}

		// -------- Render --------
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = (view.get<TransformComponent, CameraComponent>(entity));
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			// Draw Sprites.
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entityID : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entityID);

					Renderer2D::DrawSprite(transform, sprite, (uint32_t)entityID);
				}
			}

			// Draw Circles.
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entityID : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entityID);

					Renderer2D::DrawCircle(transform, circle.Color, circle.Thickness, circle.Fade, (uint32_t)entityID);
				}
			}

			Renderer2D::EndScene();
		}
	}
	void Scene::UpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		// -------- Physics --------
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

			// update tranform from physics.
			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto entityID : view)
			{
				Entity entity = { entityID, this };
				auto& transformComp = entity.GetComponent<TransformComponent>();
				auto& rb2dComp = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = static_cast<b2Body*>(rb2dComp.RuntimeRigidbody);
				const b2Vec2& position = body->GetPosition();
				transformComp.Position.x = position.x;
				transformComp.Position.y = position.y;
				transformComp.Rotation.z = body->GetAngle();
			}
		}

		// -------- Render --------
		RenderScene(camera);
	}


	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity(m_Registry.create(), this);

		entity.AddComponent<IDComponent>(uuid);

		std::string tag = name.empty() ? "Entity" : name;
		entity.AddComponent<TagComponent>(tag);

		entity.AddComponent<TransformComponent>();

		m_EntityMap[uuid] = entity;
		return entity;
	}

	void Scene::DestoryEntity(Entity entity)
	{
		m_Registry.destroy(entity);
		m_EntityMap.erase(entity.GetUUID());
	}

	Entity Scene::DupilcateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());

		// Copy all components into new entity.
		CopyComponent(AllComponents{}, newEntity, entity);

		return newEntity;
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComp = view.get<CameraComponent>(entity);
			if (!cameraComp.FixedAspectRatio)
			{
				cameraComp.Camera.SetViewportSize(width, height);
			}
		}
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };

		return {};
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto cameraComp = view.get<CameraComponent>(entity);
			if (cameraComp.Primary)
				return Entity(entity, this);
		}
		return Entity{};
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw Sprites.
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entityID : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entityID);

				Renderer2D::DrawSprite(transform, sprite, (uint32_t)entityID);
			}
		}

		// Draw Circles.
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entityID : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entityID);

				Renderer2D::DrawCircle(transform, circle.Color, circle.Thickness, circle.Fade, (uint32_t)entityID);
			}
		} 

		Renderer2D::EndScene();
	}

	void Scene::Physics2DStart()
	{
		// Create the physics world and all items inside the world.
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto entityID : view)
		{
			Entity entity = { entityID, this };
			auto& transformComp = entity.GetComponent<TransformComponent>();
			auto& rb2dComp = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type	= RigidbodyTypeToB2BodyType(rb2dComp.Type);
			bodyDef.angle	= transformComp.Rotation.z;
			bodyDef.position.Set(transformComp.Position.x, transformComp.Position.y);
			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2dComp.FixedRotation);

			rb2dComp.RuntimeRigidbody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& boxColliderComp = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape shape;
				shape.SetAsBox(boxColliderComp.Size.x * 0.5f * transformComp.Scale.x, boxColliderComp.Size.y * 0.5f * transformComp.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape					= &shape;
				fixtureDef.density					= boxColliderComp.Density;
				fixtureDef.friction					= boxColliderComp.Friction;
				fixtureDef.restitution				= boxColliderComp.Restitution;
				fixtureDef.restitutionThreshold		= boxColliderComp.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& circleColliderComp = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(circleColliderComp.Offset.x, circleColliderComp.Offset.y);
				circleShape.m_radius = transformComp.Scale.x * circleColliderComp.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape				= &circleShape;
				fixtureDef.density				= circleColliderComp.Density;
				fixtureDef.friction				= circleColliderComp.Friction;
				fixtureDef.restitution			= circleColliderComp.Restitution;
				fixtureDef.restitutionThreshold = circleColliderComp.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::Physics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}


#pragma region TODO::implement this in a better way
	template<typename Comp>
	void Scene::OnComponentAdded(Entity entity, Comp& component) {}
	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) {}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {}
	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) {}
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component){}
	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component){}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component){}
	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component) {}
	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {}
	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component) {}
#pragma endregion
}
