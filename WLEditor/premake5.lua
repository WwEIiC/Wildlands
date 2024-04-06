project "WLEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp"
	}

	includedirs {
		"%{IncludeDirs.Wildlands_Src}",
		"%{IncludeDirs.Sandbox_Src}",
		"%{IncludeDirs.SPDLOG}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.entt}"
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

