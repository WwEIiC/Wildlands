project "Wildlands"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

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
		"Externals/entt/**.hpp",
		"Externals/ImGuizmo/ImGuizmo.h",
		"Externals/ImGuizmo/ImGuizmo.cpp"
	}

	includedirs {
		"%{IncludeDirs.Wildlands_Src}",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb_image}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.yaml_cpp}",
		"%{IncludeDirs.ImGuizmo}",
		"%{IncludeDirs.Box2D}",
		"%{IncludeDirs.Mono}",
		"%{IncludeDirs.filewatch}"
	}

	links {
		"GLFW",			--link the GLFW project(make by premake5.lua in ./Wildlands/Externals/GLFW) to Wildlands.
		"Glad",
		"ImGui",
		"Yaml_cpp",
		"Box2d",
		"%{Library.Mono}",
		"opengl32.lib"
	}

	defines{
		"_CRT_SECURE_NO_WARNINGS",		-- get rid of some build warnings since some library implement use functions like sscanf instead of using sscanf_s.
		"YAML_CPP_STATIC_DEFINE"		-- build yaml_cpp as static library.
	}

	filter "files:Externals/ImGuizmo/**.cpp"	-- we use ImGuizmo as the source code of Wildlands, but the .cpp files of ImGuizmo aren't need to use WLPCH.
	flags {"NoPCH"}

	filter "system:windows"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",		--don't include any gl library because glad will provide.
			"_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING"		--get rid of some warnings when use C++20.
		}

		links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}",
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
