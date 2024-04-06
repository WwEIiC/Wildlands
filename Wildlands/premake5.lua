project "Wildlands"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	pchheader "WLPCH.h"						--even this located on a different path relative to project.
	pchsource "src/WLPCH.cpp"		--the relative path to this lua file(used for vs to create pch).

	--the files this project needed to load.
	files {
		"src/**.h",
		"src/**.cpp",
		"Externals/glm/glm/**.hpp",
		"Externals/glm/glm/**.inl",
		"Externals/stb_image/**.h",
		"Externals/stb_image/**.cpp",
		"Externals/entt/**.hpp"
	}

	includedirs {
		"%{IncludeDirs.Wildlands_Src}",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb_image}",
		"%{IncludeDirs.entt}"
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
