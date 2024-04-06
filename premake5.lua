workspace "Wildlands"
	architecture "x86_64"

	configurations { "Debug", "Release", "Dist" }
	flags { "MultiProcessorCompile" }
	startproject "WLEditor"

--eg. Debug-windows-x64
outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--An include directories set, relative to the root folder(.lua as well).
IncludeDirs = {}
IncludeDirs["Wildlands_Src"]	= "%{wks.location}/Wildlands/src"
IncludeDirs["Sandbox_Src"]		= "%{wks.location}/Sandbox/src"
IncludeDirs["SPDLOG"] 			= "%{wks.location}/Wildlands/Externals/spdlog/include"
IncludeDirs["GLFW"] 			= "%{wks.location}/Wildlands/Externals/GLFW/include"
IncludeDirs["Glad"] 			= "%{wks.location}/Wildlands/Externals/Glad/include"
IncludeDirs["ImGui"] 			= "%{wks.location}/Wildlands/Externals/ImGui"
IncludeDirs["glm"] 				= "%{wks.location}/Wildlands/Externals/glm"
IncludeDirs["stb_image"] 		= "%{wks.location}/Wildlands/Externals/stb_image"
IncludeDirs["entt"] 			= "%{wks.location}/Wildlands/Externals/entt/include"

--Premake will looks for a file name "premake5.lua" in the path specified.
group "Dependences"
	include "Wildlands/Externals/GLFW"
	include "Wildlands/Externals/Glad"
	include "Wildlands/Externals/ImGui"

group ""

include "Wildlands"
include "WLEditor"
include "Sandbox"