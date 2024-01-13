#include "WLPCH.h"

//include all the implementations of imgui. 
//get rid of some warnings since the impl use some unsafe function like sscanf instead of using sscanf_s.
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.cpp"