#pragma once
#include "Event.h"

namespace Wildlands
{
	class  WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() {}

		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
	};
	class  WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() {}

		EVENT_CLASS_TYPE(WindowLostFocus)
		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
	};

	class  WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
	private:
		uint32_t m_Width, m_Height;
	};

	class  WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(uint32_t x, uint32_t y)
			: m_X(x), m_Y(y) {}

		inline uint32_t GetX() const { return m_X; }
		inline uint32_t GetY() const { return m_Y; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMovedEvent: " << m_X << ", " << m_Y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowMoved)
		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
	private:
		uint32_t m_X, m_Y;
	};

	class  WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryApplication))
	};

}
