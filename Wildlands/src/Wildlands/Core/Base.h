#pragma once
#include <memory>

#ifdef WL_DEBUG
	#define WL_ENABLE_ASSERTS
#endif

#ifdef WL_ENABLE_ASSERTS
	#define WL_CORE_ASSERT(x, ...) {\
		if(!(x)) {\
			WL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);\
			__debugbreak();\
		}\
	}

	#define WL_ASSERT(x, ...) {\
		if(!(x)) {\
			WL_ERROR("Assertion Failed: {0}", __VA_ARGS__);\
			 __debugbreak();\
		}\
	}
#else
	#define WL_ASSERT(x, ...)
	#define WL_CORE_ASSERT(x, ...)
#endif//WL_ENABLE_ASSERTS

#define CONNECT_INTERNAL(x,y) x##y
#define CONNECT(x,y) CONNECT_INTERNAL(x,y)

#include <stdint.h>
#define BIT(x) (1 << x)
#define BIND_EVENT_FUNC(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }


namespace Wildlands
{
	template <typename T>
	using Ref = std::shared_ptr<T>;
	template <typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Unique = std::unique_ptr<T>;
	template <typename T, typename ... Args>
	constexpr Unique<T> CreateUnique(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}

#include "Wildlands/Core/Log.h"

