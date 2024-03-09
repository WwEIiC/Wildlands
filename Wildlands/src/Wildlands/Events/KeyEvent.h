#pragma once
#include "Event.h"

namespace Wildlands
{
	class  KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryInput) 
			| static_cast<int>(EventCategory::EventCategoryKeyboard))
	protected:
		KeyEvent(int keycode)
			: m_KeyCode(keycode)
		{
		}

		int m_KeyCode;
	};

	class  KeyDownEvent : public KeyEvent
	{
	public:
		KeyDownEvent(int keycode, int repeatcount)
			: KeyEvent(keycode), m_RepeatCount(repeatcount)
		{
		}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyDownEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyDown)
			
	protected:
		int m_RepeatCount;
	};

	class  KeyUpEvent : public KeyEvent
	{
	public:
		KeyUpEvent(int keycode)
			: KeyEvent(keycode)
		{
		}

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyUpEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyUp)
	};
	
	//Arised when user type in something like text box
	class  KeyTypeEvent : public KeyEvent
	{
	public:
		KeyTypeEvent(int keycode)
			: KeyEvent(keycode)
		{
		}

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyDownEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyType)
	};

}
