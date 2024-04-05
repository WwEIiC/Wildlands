#pragma once
//This will just be used by applications that use WildlandsEngine.

#include "Wildlands/Core/Application.h"
#include "Wildlands/Core/Base.h"
#include "Wildlands/Core/Timestep.h"
#include "Wildlands/Core/Log.h"
#include "Wildlands/Core/Input.h"
#include "Wildlands/Core/Layer.h"

#include "Wildlands/ImGui/ImGuiLayer.h"

//----- Events -------------------------------------------------
#include "Wildlands/Events/ApplicationEvent.h"
#include "Wildlands/Events/KeyEvent.h"
#include "Wildlands/Events/MouseEvent.h"
//--------------------------------------------------------------

//----- ECS ----------------------------------------------------
#include "Wildlands/ECS/Scene.h"
#include "Wildlands/ECS/Entity.h"
#include "Wildlands/ECS/ScriptableEntity.h"
#include "Wildlands/ECS/Components.h"
//--------------------------------------------------------------

//----- Renderer -----------------------------------------------
#include "Wildlands/Renderer/Renderer.h"
#include "Wildlands/Renderer/Renderer2D.h"

#include "Wildlands/Renderer/Texture.h"
#include "Wildlands/Renderer/Shader.h"
#include "Wildlands/Renderer/FrameBuffer.h"
#include "Wildlands/Renderer/Buffers.h"
#include "Wildlands/Renderer/VertexArray.h"

#include "Wildlands/Renderer/Cameras.h"
//--------------------------------------------------------------

#include "Wildlands/Renderer/CameraControllers.h"

#include "Wildlands/Common/FileReader.h"
#include "Wildlands/Common/Singleton.h"
