workspace "Wildlands"
	architecture "x64"

	configurations { "Debug", "Release", "Dist" }

--eg. Debug-windows-x64
outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Wildlands"
	location "Wildlands"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputDir .."/%{prj.name}")
	objdir ("bin-int/" .. outputDir .."/%{prj.name}")

	--the files this project needed to load
	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"%{prj.name}/Externals/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
			"WL_BUILD_DLL"
		}

		postbuildcommands { 
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputDir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines {"WL_DEBUG"}
		symbols "On"			-- make sure the debug symbols can be used

	filter "configurations:Release"
		defines {"WL_RELEASE"}
		optimize "On"

	filter "configurations:Dist"
		defines {"WL_DIST"}
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
		"Wildlands/Externals/spdlog/include"
	}

	--link the Wildlands project to this project
	links {
		"Wildlands"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines{
			"WL_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines {"WL_DEBUG"}
		symbols "On"

	filter "configurations:Release"
		defines {"WL_RELEASE"}
		optimize "On"
	filter "configurations:Dist"
		defines {"WL_DIST"}
		optimize "On"