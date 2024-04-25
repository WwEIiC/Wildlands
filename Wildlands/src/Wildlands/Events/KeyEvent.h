#pragma once
#include "Event.h"

namespace Wildlands
{
	class  KeyEvent : public Event
	{
	public:
		inline KeyCode GetKeyCode() { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(static_cast<int>(EventCategory::EventCategoryInput) 
			| static_cast<int>(EventCategory::EventCategoryKeyboard))
	protected:
		KeyEvent(KeyCode keycode)
			: m_KeyCode(keycode)
		{
		}

		KeyCode m_KeyCode;
	};

	class  KeyDownEvent : public KeyEvent
	{
	public:
		KeyDownEvent(KeyCode keycode, bool isRepeat = false)
			: KeyEvent(keycode), m_IsRepeat(isRepeat)
		{
		}

		inline bool IsRepeat() const { return m_IsRepeat; }

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyDownEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyDown)
			
	protected:
		bool m_IsRepeat;
	};

	class  KeyUpEvent : public KeyEvent
	{
	public:
		KeyUpEvent(KeyCode keycode)
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
		KeyTypeEvent(KeyCode keycode)
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
