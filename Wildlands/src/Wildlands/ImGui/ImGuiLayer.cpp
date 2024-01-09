#include "WLPCH.h"

#include "imgui.h"
#include "Wildlands/Platforms/OpenGL/ImGuiRenderer_OpenGL.h"

#include "ImGuiLayer.h"
#include "Wildlands/Events/ApplicationEvent.h"
#include "Wildlands/Events/MouseEvent.h"
#include "Wildlands/Events/KeyEvent.h"

#include <GLFW/glfw3.h>

namespace Wildlands
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}
	ImGuiLayer::~ImGuiLayer()
	{
	}
	void ImGuiLayer::Attach()
	{
	}
	void ImGuiLayer::Detach()
	{
	}
	void ImGuiLayer::Update()
	{
	}
	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseMoved));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrolled));
		dispatcher.Dispatch<MouseButtonDownEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonDown));
		dispatcher.Dispatch<MouseButtonUpEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonUp));
		dispatcher.Dispatch<KeyDownEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyDown));
		dispatcher.Dispatch<KeyUpEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyUp));
		dispatcher.Dispatch<KeyTypeEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyType));
	}

#pragma region Event Handling
	bool ImGuiLayer::OnWindowResize(WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2(event.GetWidth(), event.GetHeight());
		return false;
	}
	bool ImGuiLayer::OnMouseMoved(MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MousePos = ImVec2(event.GetX(), event.GetY());
		return false;
	}
	bool ImGuiLayer::OnMouseScrolled(MouseScrolledEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += event.GetXOffset();
		io.MouseWheel += event.GetYOffset();

		return false;//let it broadcast to other layers
	}
	bool ImGuiLayer::OnMouseButtonDown(MouseButtonDownEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[event.GetMouseButton()] = true;

		return false;//let it broadcast to other layers
	}
	bool ImGuiLayer::OnMouseButtonUp(MouseButtonUpEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[event.GetMouseButton()] = false;

		return false;//let it broadcast to other layers
	}
	bool ImGuiLayer::OnKeyDown(KeyDownEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.GetKeyCode()] = true;

		//also some special keys
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		return false;
	}
	bool ImGuiLayer::OnKeyUp(KeyUpEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.GetKeyCode()] = false;

		return false;
	}
	bool ImGuiLayer::OnKeyType(KeyTypeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		int keycode = event.GetKeyCode();
		if (keycode > 0 && keycode <= UINT16_MAX)
			io.AddInputCharacter((ushort)keycode);

		return false;
	}
#pragma endregion
}
