#pragma once
#include "Wildlands/Core/Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Wildlands
{
	class  Log
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

//Core log macros
#define WL_CORE_TRACE(...)		::Wildlands::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define WL_CORE_INFO(...)		::Wildlands::Log::GetCoreLogger()->info(__VA_ARGS__);
#define WL_CORE_WARN(...)		::Wildlands::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define WL_CORE_ERROR(...)		::Wildlands::Log::GetCoreLogger()->error(__VA_ARGS__);
#define WL_CORE_FATAL(...)		::Wildlands::Log::GetCoreLogger()->critical(__VA_ARGS__);

//Client log macros
#define WL_TRACE(...)			::Wildlands::Log::GetClientLogger()->trace(__VA_ARGS__);
#define WL_INFO(...)			::Wildlands::Log::GetClientLogger()->info(__VA_ARGS__);
#define WL_WARN(...)			::Wildlands::Log::GetClientLogger()->warn(__VA_ARGS__);
#define WL_ERROR(...)			::Wildlands::Log::GetClientLogger()->error(__VA_ARGS__);
#define WL_FATAL(...)			::Wildlands::Log::GetClientLogger()->critical(__VA_ARGS__);
