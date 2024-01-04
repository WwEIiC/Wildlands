#pragma once
#include "Event.h"

namespace Wildlands
{
	class WL_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			:m_MouseX(x), m_MouseY(y) 
		{
		}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryInput)
							| static_cast<int>(EventCategory::EventCategoryMouse))

	private:
		float m_MouseX, m_MouseY;
	};

	class WL_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			:m_XOffset(xOffset), m_YOffset(yOffset)
		{
		}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryInput)
							| static_cast<int>(EventCategory::EventCategoryMouse))

	private:
		float m_XOffset, m_YOffset;
	};

	class WL_API MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryInput)
							| static_cast<int>(EventCategory::EventCategoryMouse))

	protected:
		MouseButtonEvent(int button)
			:m_Button(button)
		{
		}

		int m_Button;
	};

	class WL_API MouseButtonDownEvent : public MouseButtonEvent
	{
	public:
		MouseButtonDownEvent(int mousebutton)
			:MouseButtonEvent(mousebutton)
		{
		}

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonDownEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonDown)
	};

	class WL_API MouseButtonUpEvent : public MouseButtonEvent
	{
	public:
		MouseButtonUpEvent(int mousebutton)
			:MouseButtonEvent(mousebutton)
		{
		}

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonUpEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonUp)
	};

}
