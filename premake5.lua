workspace "Wildlands"
	architecture "x64"

	configurations { "Debug", "Release", "Dist" }
	startproject "Sandbox"

--eg. Debug-windows-x64
outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--An include directories set, relative to the root folder(.lua as well)\
IncludeDirs = {}
IncludeDirs["SPDLOG"] = "Wildlands/Externals/spdlog/include"
IncludeDirs["GLFW"] = "Wildlands/Externals/GLFW/include"
IncludeDirs["Glad"] = "Wildlands/Externals/Glad/include"
IncludeDirs["ImGui"] = "Wildlands/Externals/ImGui"
IncludeDirs["glm"] = "Wildlands/Externals/glm"

--Premake will looks for a file name "premake5.lua" in the path specified
include "Wildlands/Externals/GLFW"
include "Wildlands/Externals/Glad"
include "Wildlands/Externals/ImGui"

project "Wildlands"
	location "Wildlands"
	kind "SharedLib"
	language "C++"
	staticruntime "Off"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	pchheader "WLPCH.h"						--even this located on a different path relative to project
	pchsource "Wildlands/src/WLPCH.cpp"		--the relative path to this lua file(used for vs to create pch)

	--the files this project needed to load
	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Externals/glm/glm/**.hpp",
		"%{prj.name}/Externals/glm/glm/**.inl"
	}

	includedirs {
		"%{prj.name}/src",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}"
	}

	links {
		"GLFW",			--link the GLFW project(make by premake5.lua in ./Wildlands/Externals/GLFW) to Wildlands
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
			"WL_BUILD_DLL",
			"GLFW_INCLUDE_NONE"		--don't include any gl library because glad will provide
		}

		postbuildcommands { 
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputDir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines {"WL_DEBUG"}
		runtime "Debug"
		symbols "On"			-- make sure the debug symbols can be used

	filter "configurations:Release"
		defines {"WL_RELEASE"}
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines {"WL_DIST"}
		runtime "Release"
		optimize "On"


---------------------------------------------------------------------------------------------------------------------------------------------------
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "Off"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Wildlands/src",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.glm}"
	}

	--link the Wildlands project to this project
	links {
		"Wildlands"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines {"WL_DEBUG"}
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines {"WL_RELEASE"}
		runtime "Release"
		optimize "On"
	filter "configurations:Dist"
		defines {"WL_DIST"}
		buildoptions "/MD"
		optimize "On"
