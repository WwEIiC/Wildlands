workspace "Wildlands"
	architecture "x64"

	configurations { "Debug", "Release", "Dist" }
	flags { "MultiProcessorCompile" }
	startproject "Sandbox"

--eg. Debug-windows-x64
outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--An include directories set, relative to the root folder(.lua as well).
IncludeDirs = {}
IncludeDirs["SPDLOG"] = "Wildlands/Externals/spdlog/include"
IncludeDirs["GLFW"] = "Wildlands/Externals/GLFW/include"
IncludeDirs["Glad"] = "Wildlands/Externals/Glad/include"
IncludeDirs["ImGui"] = "Wildlands/Externals/ImGui"
IncludeDirs["glm"] = "Wildlands/Externals/glm"
IncludeDirs["stb_image"] = "Wildlands/Externals/stb_image"

--Premake will looks for a file name "premake5.lua" in the path specified.
group "Dependences"
	include "Wildlands/Externals/GLFW"
	include "Wildlands/Externals/Glad"
	include "Wildlands/Externals/ImGui"

group ""

project "Wildlands"
	location "Wildlands"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	pchheader "WLPCH.h"						--even this located on a different path relative to project.
	pchsource "Wildlands/src/WLPCH.cpp"		--the relative path to this lua file(used for vs to create pch).

	--the files this project needed to load.
	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Externals/glm/glm/**.hpp",
		"%{prj.name}/Externals/glm/glm/**.inl",
		"%{prj.name}/Externals/stb_image/**.h",
		"%{prj.name}/Externals/stb_image/**.cpp"
	}

	includedirs {
		"%{prj.name}/src",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb_image}"
	}

	links {
		"GLFW",			--link the GLFW project(make by premake5.lua in ./Wildlands/Externals/GLFW) to Wildlands.
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	defines{
		"_CRT_SECURE_NO_WARNINGS"		-- get rid of some build warnings since some library implement use functions like sscanf instead of using sscanf_s.
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",		--don't include any gl library because glad will provide.
			"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING"		--get rid of some warnings when use C++20.
		}

		-- postbuildcommands { 
		-- 	("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputDir .. "/Sandbox/\"")
		-- }

	filter "configurations:Debug"
		defines {"WL_DEBUG"}
		runtime "Debug"
		symbols "on"			-- make sure the debug symbols can be used.

	filter "configurations:Release"
		defines {"WL_RELEASE"}
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines {"WL_DIST"}
		runtime "Release"
		optimize "on"



---------------------------------------------------------------------------------------------------------------------------------------------------
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/assets/Shaders/**.frag",
		"%{prj.name}/assets/Shaders/**.vert",
		"%{prj.name}/assets/Textures/**.png"
	}

	includedirs {
		"Wildlands/src",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImGui}"
	}

	--link the Wildlands project to this project.
	links {
		"Wildlands"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
			"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING"		--get rid of some warnings when use C++20.
		}

	filter "configurations:Debug"
		defines {"WL_DEBUG"}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {"WL_RELEASE"}
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines {"WL_DIST"}
		runtime "Release"
		optimize "on"
