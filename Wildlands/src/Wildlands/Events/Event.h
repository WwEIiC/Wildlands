#pragma once

#include "WLPCH.h"
#include "Wildlands/Core/Core.h" 


namespace Wildlands
{
	enum class EventType
	{
		None = 0,
		WindowFocus, WindowLostFocus, WindowResize, WindowMoved, WindowClose,
		MouseMoved, MouseButtonDown, MouseButtonUp, MouseScrolled,
		KeyDown, KeyUp
	};

	enum class EventCategory
	{
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4),

	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetEventCategory() const override { return category; }

	class WL_API Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType GetEventType() const = 0;
		virtual int GetEventCategory() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) { return static_cast<int>(category) & GetEventCategory(); }

	protected:
		bool m_Handled = false;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& event)
	{
		return os << event.ToString();
	}

	class WL_API EventDispatcher
	{
		template<typename T>
		using EventFunc = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) 
			: m_Event(event) 
		{
		}

		template<typename T>
		bool Dispatch(EventFunc<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*dynamic_cast<T*>(&m_Event));
				return true;
			}

			return false;
		}
	private:
		Event& m_Event;
	};
}
