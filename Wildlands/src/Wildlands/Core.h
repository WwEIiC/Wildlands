#pragma once

#ifdef WL_PLATFORM_WINDOWS
	#ifdef WL_BUILD_DLL
		#define WL_API _declspec(dllexport)
	#else
		#define WL_API _declspec(dllimport)
	#endif//WL_BUILD_DLL
#else
	#error Wildlands only supports Windows now!
#endif//WL_PLATFORM_WINDOW
