#pragma once

#ifdef WL_PLATFORM_WINDOWS
	#ifdef WL_DYNAMIC_LINK
		#ifdef WL_BUILD_DLL
			#define WL_API _declspec(dllexport)
		#else
			#define WL_API _declspec(dllimport)
		#endif//WL_BUILD_DLL
	#else
		#define WL_API 
	#endif
#else
	#error Wildlands only supports Windows now!
#endif//WL_PLATFORM_WINDOW

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

#define BIT(x) (1 << x)
#define BIND_EVENT_FUNC(func) std::bind(&func, this, std::placeholders::_1)



