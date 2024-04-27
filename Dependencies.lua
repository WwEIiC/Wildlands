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
IncludeDirs["yaml_cpp"] 		= "%{wks.location}/Wildlands/Externals/yaml_cpp/include"
IncludeDirs["ImGuizmo"] 		= "%{wks.location}/Wildlands/Externals/ImGuizmo"
IncludeDirs["Box2D"] 			= "%{wks.location}/Wildlands/Externals/box2d/include"
IncludeDirs["Mono"] 			= "%{wks.location}/Wildlands/Externals/Mono/include"

LibraryDirs = {}
LibraryDirs["Mono"] = "%{wks.location}/Wildlands/Externals/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["Mono"] = "%{LibraryDirs.Mono}/libmono-static-sgen.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"