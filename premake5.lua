workspace "Wildlands"
	architecture "x64"

	configurations { "Debug", "Release", "Dist" }

--eg. Debug-windows-x64
outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--An include directories set, relative to the root folder(.lua as well)\
IncludeDirs = {}
IncludeDirs["SPDLOG"] = "Wildlands/Externals/spdlog/include"
IncludeDirs["GLFW"] = "Wildlands/Externals/GLFW/include"

--Premake will looks for a file name "premake5.lua" in the path specified
include "Wildlands/Externals/GLFW"

project "Wildlands"
	location "Wildlands"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	pchheader "WLPCH.h"						--even this located on a different path relative to project
	pchsource "Wildlands/src/WLPCH.cpp"		--the relative path to this lua file(used for vs to create pch)

	--the files this project needed to load
	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"%{prj.name}/src",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.GLFW}"
	}

	links {
		"GLFW",			--link the GLFW project(make by premake5.lua in ./Wildlands/Externals/GLFW) to Wildlands
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
			"WL_BUILD_DLL"
		}

		postbuildcommands { 
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputDir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines {
			"WL_DEBUG",
			"WL_ENABLE_ASSERTS"
		}
		buildoptions "/MDd"
		symbols "On"			-- make sure the debug symbols can be used

	filter "configurations:Release"
		defines {"WL_RELEASE"}
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines {"WL_DIST"}
		buildoptions "/MD"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Wildlands/src",
		"%{IncludeDirs.SPDLOG}"
	}

	--link the Wildlands project to this project
	links {
		"Wildlands"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines {"WL_DEBUG"}
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines {"WL_RELEASE"}
		buildoptions "/MD"
		optimize "On"
	filter "configurations:Dist"
		defines {"WL_DIST"}
		buildoptions "/MD"
		optimize "On"


		
workspace "Wildlands"
	startproject "Sandbox"