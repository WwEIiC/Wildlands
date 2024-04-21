include "Dependencies.lua"

workspace "Wildlands"
	architecture "x86_64"

	configurations { "Debug", "Release", "Dist" }
	flags { "MultiProcessorCompile" }
	startproject "WLEditor"

--eg. Debug-windows-x64
outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Premake will looks for a file name "premake5.lua" in the path specified.
group "Dependences"
	include "Wildlands/Externals/GLFW"
	include "Wildlands/Externals/Glad"
	include "Wildlands/Externals/ImGui"
	include "Wildlands/Externals/yaml_cpp"
	include "Wildlands/Externals/box2d"
group ""

include "Wildlands"
include "WLEditor"
include "Sandbox"