#pragma once

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

#include <stdint.h>
#define BIT(x) (1 << x)
#define BIND_EVENT_FUNC(func) std::bind(&func, this, std::placeholders::_1)

#include <memory>

namespace Wildlands
{
	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T>
	using Unique = std::unique_ptr<T>;
}

