#include "WLPCH.h"
#include "ScriptGlue.h"
#include "Wildlands/Scripting/ScriptEngine.h"

#include "Wildlands/Core/UUID.h"
#include "Wildlands/Core/Input.h"

#include "Wildlands/ECS/Scene.h"
#include "Wildlands/ECS/Entity.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <box2d/b2_body.h>

namespace Wildlands
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;
	#define WL_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Wildlands.InternalCalls::" #Name, Name)

#pragma region Native
	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		WL_CORE_WARN("Value: {0}", *parameter);
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		WL_CORE_WARN("Value: {0}", *parameter);
		return glm::dot(*parameter, *parameter);
	}
#pragma endregion

#pragma region Entity
	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::GetSceneContext();
		WL_CORE_ASSERT(scene, "Scene doesn't exisited.");
		Entity entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);

		if (entity) { return entity.GetUUID(); }
		return 0;
	}

	static MonoObject* Entity_GetScriptInstance(UUID uuid)
	{
		return ScriptEngine::GetManagedInstance(uuid);
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		WL_CORE_ASSERT(scene, "Scene doesn't exisited.");
		Entity entity = scene->GetEntityByUUID(entityID);
		WL_CORE_ASSERT(entity, "Entity doesn't exisited.");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		WL_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end(), "Could not find component type");
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}
#pragma endregion

#pragma region Transform Component
	static void TransformComponent_GetPosition(UUID entityID, glm::vec3* outPosition)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		WL_CORE_ASSERT(scene, "Scene doesn't exisited.");
		Entity entity = scene->GetEntityByUUID(entityID);
		WL_CORE_ASSERT(entity, "Entity doesn't exisited.");

		*outPosition = entity.GetComponent<TransformComponent>().Position;
	}

	static void TransformComponent_SetPosition(UUID entityID, glm::vec3* position)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		WL_CORE_ASSERT(scene, "Scene doesn't exisited.");
		Entity entity = scene->GetEntityByUUID(entityID);
		WL_CORE_ASSERT(entity, "Entity doesn't exisited.");

		entity.GetComponent<TransformComponent>().Position = *position;
	}
#pragma endregion

#pragma region Rigidbody2D Component
	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		WL_CORE_ASSERT(scene, "Scene doesn't exisited.");
		Entity entity = scene->GetEntityByUUID(entityID);
		WL_CORE_ASSERT(entity, "Entity doesn't exisited.");

		WL_CORE_ASSERT(entity, "Entity doesn't have Rigidbody2DComponent.");
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeRigidbody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		WL_CORE_ASSERT(scene, "Scene doesn't exisited.");
		Entity entity = scene->GetEntityByUUID(entityID);
		WL_CORE_ASSERT(entity, "Entity doesn't exisited.");

		WL_CORE_ASSERT(entity, "Entity doesn't have Rigidbody2DComponent.");
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeRigidbody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}
#pragma endregion


#pragma region Input
	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyDown(keycode);
	}
#pragma endregion

#pragma region ScriptGlue
	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
			{
				// example: Wildlands::IDComponent --> Wildlands.IDComponent
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Wildlands.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					WL_CORE_ERROR("Could not find component type {}", managedTypename);
					return;
				}
				s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		WL_ADD_INTERNAL_CALL(NativeLog);
		WL_ADD_INTERNAL_CALL(NativeLog_Vector);
		WL_ADD_INTERNAL_CALL(NativeLog_VectorDot);

		WL_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		WL_ADD_INTERNAL_CALL(Entity_GetScriptInstance);
		WL_ADD_INTERNAL_CALL(Entity_HasComponent);
		WL_ADD_INTERNAL_CALL(TransformComponent_GetPosition);
		WL_ADD_INTERNAL_CALL(TransformComponent_SetPosition);

		WL_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		WL_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		WL_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
#pragma endregion
}
