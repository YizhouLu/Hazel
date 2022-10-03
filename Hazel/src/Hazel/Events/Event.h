#pragma once

#include "hzpch.h"
#include "Hazel/Core.h"

namespace Hazel {

// Events in Hazel are currently blocking, meaning when an event occurs it
// immediately gets dispatched and must be dealt with right then and there.
// For the future, a better strategy might be to buffer events in an event
// bus and process them during the "event" part of the update stage.

enum class EventType
{
	None = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	AppTick, AppUpdate, AppRender,
	KeyPressed, KeyReleased,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory
{
	None = 0,
	EventCategoryApplication    = BIT(0),	// 00001
	EventCategoryInput          = BIT(1),	// 00010
	EventCategoryKeyboard       = BIT(2),	// 00100
	EventCategoryMouse          = BIT(3),	// 01000
	EventCategoryMouseButton    = BIT(4)	// 10000
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
							virtual EventType GetEventType() const override { return GetStaticType(); }\
							virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

class HAZEL_API Event {
public:
	bool Handled = false;

	virtual EventType GetEventType() const = 0;	// override by the macro "EVENT_CLASS_TYPE"
	virtual const char* GetName() const = 0;	// override by the macro "EVENT_CLASS_TYPE"
	virtual int GetCategoryFlags() const = 0;	// override by the macro "EVENT_CLASS_CATEGORY"
	virtual std::string ToString() const { return GetName(); } // override by each derived class when applicable

	inline bool IsInCategory(EventCategory category) {
		return GetCategoryFlags() & category;
	}
};

class EventDispatcher {
template<typename T>
using EventFcn = std::function<bool(T&)>;

public:
	EventDispatcher(Event& event)
		: m_Event(event) {}

	template<typename T>
	bool Dispatch(EventFcn<T> fcn) {
		if (m_Event.GetEventType() == T::GetStaticType()) {
			m_Event.Handled = fcn(*(T*)&m_Event);
			return true;
		}
		return false;
	}

private:
	Event& m_Event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e) {
	return os << e.ToString();
}
}
