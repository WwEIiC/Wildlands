#include "WLPCH.h"
#include "ScriptEngine.h"

#include "Wildlands/Scripting/ScriptGlue.h"

#include "Wildlands/Core/Application.h"
#include "Wildlands/Core/FileSystem.h"

#include "FileWatch.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>

namespace Wildlands
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{ "System.Single",		ScriptFieldType::Float },
		{ "System.Double",		ScriptFieldType::Double },
		{ "System.Boolean",		ScriptFieldType::Bool },
		{ "System.Char",		ScriptFieldType::Char },
		{ "System.Int16",		ScriptFieldType::Short },
		{ "System.Int32",		ScriptFieldType::Int },
		{ "System.Int64",		ScriptFieldType::Long },
		{ "System.Byte",		ScriptFieldType::Byte },
		{ "System.UInt16",		ScriptFieldType::UShort },
		{ "System.UInt32",		ScriptFieldType::UInt },
		{ "System.UInt64",		ScriptFieldType::ULong },

		{ "Wildlands.Vector2",	ScriptFieldType::Vector2 },
		{ "Wildlands.Vector3",	ScriptFieldType::Vector3 },
		{ "Wildlands.Vector4",	ScriptFieldType::Vector4 },

		{ "Wildlands.Entity",	ScriptFieldType::Entity },
	};

	namespace Utils
	{
		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false)
		{
			ScopeBuffer fileData = FileSystem::ReadFileBinary(assemblyPath);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData.As<char>(), (uint32_t)fileData.Size(), 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				// Log some error message using the errorMessage data
				return nullptr;
			}

			if (loadPDB)
			{
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");

				if (std::filesystem::exists(pdbPath))
				{
					ScopeBuffer pdbFileData = FileSystem::ReadFileBinary(pdbPath);
					mono_debug_open_image_from_memory(image, pdbFileData.As<const mono_byte>(), (int)pdbFileData.Size());
					WL_CORE_INFO("Loaded PDB {}", pdbPath);
				}
			}

			std::string pathString = assemblyPath.string();
			MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
			mono_image_close(image);

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				WL_CORE_TRACE("{}.{}", nameSpace, name);
			}
		}

		ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
			{
				WL_CORE_ERROR("Unknown type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}

		const char* ScriptFieldTypeToString(ScriptFieldType type)
		{
			switch (type)
			{
			case ScriptFieldType::Float:   return "Float";
			case ScriptFieldType::Double:  return "Double";
			case ScriptFieldType::Bool:    return "Bool";
			case ScriptFieldType::Char:    return "Char";
			case ScriptFieldType::Byte:    return "Byte";
			case ScriptFieldType::Short:   return "Short";
			case ScriptFieldType::Int:     return "Int";
			case ScriptFieldType::Long:    return "Long";
			case ScriptFieldType::UByte:   return "UByte";
			case ScriptFieldType::UShort:  return "UShort";
			case ScriptFieldType::UInt:    return "UInt";
			case ScriptFieldType::ULong:   return "ULong";
			case ScriptFieldType::Vector2: return "Vector2";
			case ScriptFieldType::Vector3: return "Vector3";
			case ScriptFieldType::Vector4: return "Vector4";
			case ScriptFieldType::Entity:  return "Entity";
			}
			return "<Invalid>";
		}

		inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
		{
			if (fieldType == "None")    return ScriptFieldType::None;
			if (fieldType == "Float")   return ScriptFieldType::Float;
			if (fieldType == "Double")  return ScriptFieldType::Double;
			if (fieldType == "Bool")    return ScriptFieldType::Bool;
			if (fieldType == "Char")    return ScriptFieldType::Char;
			if (fieldType == "Byte")    return ScriptFieldType::Byte;
			if (fieldType == "Short")   return ScriptFieldType::Short;
			if (fieldType == "Int")     return ScriptFieldType::Int;
			if (fieldType == "Long")    return ScriptFieldType::Long;
			if (fieldType == "UByte")   return ScriptFieldType::UByte;
			if (fieldType == "UShort")  return ScriptFieldType::UShort;
			if (fieldType == "UInt")    return ScriptFieldType::UInt;
			if (fieldType == "ULong")   return ScriptFieldType::ULong;
			if (fieldType == "Vector2") return ScriptFieldType::Vector2;
			if (fieldType == "Vector3") return ScriptFieldType::Vector3;
			if (fieldType == "Vector4") return ScriptFieldType::Vector4;
			if (fieldType == "Entity")  return ScriptFieldType::Entity;

			return ScriptFieldType::None;
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		std::filesystem::path CoreAssemblyFilepath;
		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;

		std::filesystem::path AppAssemblyFilepath;
		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityScriptInstances;
		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

		// File watch to decide reload assembly
		Unique<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AppAssemblyFileChanged = false;

#ifdef WL_DEBUG
		bool EnableDebugging = true;
#else
		bool EnableDebugging = false;
#endif

		// Runtime
		Scene* SceneContext = nullptr;
	};

	static ScriptEngineData* s_Data = nullptr;

	static void OnAppAssemblyFileChanged(const std::string& path, const filewatch::Event change_type)
	{
		bool changed = s_Data->AppAssemblyFileChanged;
		if (!s_Data->AppAssemblyFileChanged && change_type == filewatch::Event::modified)
		{
			s_Data->AppAssemblyFileChanged = true;

			Application::Get().SubmitToMainThread([](){
				s_Data->AppAssemblyFileWatcher.reset();
				ScriptEngine::ReloadAssembly();
			});
		}
	}

	// ===========================================================================
	// ======================= ScriptEngine ======================================
	// ===========================================================================
	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();
		ScriptGlue::RegisterFunctions();

		bool succeed = LoadAssembly("resources/Scripts/Wildlands-ScriptCore.dll");
		if (!succeed)
		{
			WL_CORE_ERROR("[ScriptEngine] failed to load Wildlands-ScriptCore assembly");
			return;
		}
		succeed = LoadAppAssembly("SandboxProject/Assets/Scripts/Binaries/Sandbox.dll");
		if (!succeed)
		{
			WL_CORE_ERROR("[ScriptEngine] failed to load app assembly");
			return;
		}
		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		s_Data->EntityClass = ScriptClass("Wildlands", "Entity", true);
	}

	void ScriptEngine::Close()
	{
		CloseMono();
		delete s_Data;
	}


	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		if (s_Data->EnableDebugging)
		{
			const char* argv[2] = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};

			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}


		MonoDomain* rootDomain = mono_jit_init("WildlandsJITRuntime");
		WL_CORE_ASSERT(rootDomain, "InitMono failed :: rootDomain doesn't exist.");

		// Store the root domain pointer
		s_Data->RootDomain = rootDomain;

		if (s_Data->EnableDebugging)
			mono_debug_domain_create(s_Data->RootDomain);

		mono_thread_set_main(mono_thread_current());
	}

	void ScriptEngine::CloseMono()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}

	bool ScriptEngine::LoadAssembly(const std::filesystem::path& filepath)
	{
		// Create an App Domain
		char name[] = "WildlandsJITRuntime";
		s_Data->AppDomain = mono_domain_create_appdomain(name, nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		// Move this maybe
		s_Data->CoreAssemblyFilepath = filepath;
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(filepath, s_Data->EnableDebugging);
		if (s_Data->CoreAssembly == nullptr) { return false; }

		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		// Utils::PrintAssemblyTypes(s_Data->CoreAssembly);

		return true;
	}

	bool ScriptEngine::LoadAppAssembly(const std::filesystem::path& filepath)
	{
		s_Data->AppAssemblyFilepath = filepath;
		s_Data->AppAssembly = Utils::LoadMonoAssembly(filepath, s_Data->EnableDebugging);
		if (s_Data->AppAssembly == nullptr) { return false; }

		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
		// Utils::PrintAssemblyTypes(s_Data->AppAssembly);

		s_Data->AppAssemblyFileWatcher = CreateUnique<filewatch::FileWatch<std::string>>(filepath.string(), OnAppAssemblyFileChanged);
		s_Data->AppAssemblyFileChanged = false;

		return true;
	}

	void ScriptEngine::ReloadAssembly()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_Data->AppDomain);

		LoadAssembly(s_Data->CoreAssemblyFilepath);
		LoadAppAssembly(s_Data->AppAssemblyFilepath);
		LoadAssemblyClasses();

		ScriptGlue::RegisterComponents();

		// Retrieve and instantiate class
		s_Data->EntityClass = ScriptClass("Wildlands", "Entity", true);
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_Data->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_Data->SceneContext = nullptr;

		s_Data->EntityScriptInstances.clear();
	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
	}

	/// <summary> set up the script info from the entity defination. </summary>
	void ScriptEngine::CreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntityClassExists(sc.ClassName))
		{
			UUID entityID = entity.GetUUID();

			Ref<ScriptInstance> scriptInstance = CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity);
			s_Data->EntityScriptInstances[entityID] = scriptInstance;

			// Copy field values to the entity.
			if (s_Data->EntityScriptFields.find(entityID) != s_Data->EntityScriptFields.end())
			{
				const ScriptFieldMap& fields = s_Data->EntityScriptFields[entityID];
				for (const auto&[name, fieldInstance] : fields)
					scriptInstance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
			}

			scriptInstance->InvokeOnCreate();
		}
	}

	void ScriptEngine::UpdateEntity(Entity entity, Timestep ts)
	{
		UUID entityUUID = entity.GetUUID();
		if (s_Data->EntityScriptInstances.find(entityUUID) != s_Data->EntityScriptInstances.end())
		{
			Ref<ScriptInstance> instance = s_Data->EntityScriptInstances[entityUUID];
			instance->InvokeOnUpdate((float)ts);
		}
		else
		{
			WL_CORE_ERROR("Entity {} doesn't have a script instance.", entityUUID);
		}

	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_Data->SceneContext;
	}

	Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID)
	{
		auto it = s_Data->EntityScriptInstances.find(entityID);
		if (it == s_Data->EntityScriptInstances.end())
			return nullptr;

		return it->second;
	}

	Ref<ScriptClass> ScriptEngine::GetEntityScriptClass(const std::string& name)
	{
		if (s_Data->EntityClasses.find(name) != s_Data->EntityClasses.end())
			return s_Data->EntityClasses[name];

		return nullptr;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
	{
		UUID entityID = entity.GetUUID();
		if (s_Data->EntityScriptFields.find(entityID) == s_Data->EntityScriptFields.end())
		{
			WL_CORE_ERROR("Entity {} doesn't have any script fields.", entityID);
			s_Data->EntityScriptFields[entityID] = {};
		}

		return s_Data->EntityScriptFields[entityID];
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->CoreAssemblyImage;
	}

	MonoObject* ScriptEngine::GetManagedInstance(UUID uuid)
	{
		WL_CORE_ASSERT(s_Data->EntityScriptInstances.find(uuid) != s_Data->EntityScriptInstances.end(), "Script Instance doesn't exisit.");
		return s_Data->EntityScriptInstances.at(uuid)->GetManagedObject();
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Wildlands", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) != 0)
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, className);

			if (monoClass == entityClass) { continue; }

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity) { continue; }

			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);
			s_Data->EntityClasses[fullName] = scriptClass;


			// This routine is an iterator routine for retrieving the fields in a class.
			// You must pass a gpointer that points to zero and is treated as an opaque handle
			// to iterate over all of the elements. When no more values are available, the return value is NULL.

			int fieldCount = mono_class_num_fields(monoClass);
			WL_CORE_TRACE("{} has {} fields:", className, fieldCount);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					WL_CORE_TRACE("  {} ({})", fieldName, Utils::ScriptFieldTypeToString(fieldType));

					scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}
		}
	}

	// ===========================================================================
	// ======================= ScriptClass =======================================
	// ===========================================================================
	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage, classNamespace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		return mono_runtime_invoke(method, instance, params, &exception);
	}

	// ===========================================================================
	// ======================= ScriptInstance ====================================
	// ===========================================================================
	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);

		// Call Entity constructor
		{
			UUID entityID = entity.GetUUID();
			void* param = &entityID;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_OnUpdateMethod)
		{
			void* param = &ts;
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
		}
	}
	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end()) { return false; }

		const ScriptField& field = it->second;
		mono_field_get_value(m_Instance, field.ClassField, buffer);
		return true;
	}
	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end()) { return false; }

		const ScriptField& field = it->second;
		mono_field_set_value(m_Instance, field.ClassField, (void*)value);
		return true;
	}
}