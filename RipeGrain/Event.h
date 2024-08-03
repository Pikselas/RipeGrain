#pragma once
#include <typeindex>
#include <optional>
#include "RepulsiveEngine/ImageSprite.h"
namespace RipeGrain
{

	struct EventSceneLoaded
	{
		std::list<ImageSprite>* sprites;
	};

	struct EventMouseInput
	{
		enum class Type
		{
			Move,
			LeftPress, 
			RightPress,
			LeftDoublePress,
			RightDoublePress
		};
		Type type;
		int x_pos, y_pos;
	};

	struct EventKeyBoardInput
	{
		enum class Type
		{
			KeyPress,
			KeyRelease
		};
		Type type;
		unsigned char key_code;
	};

	class Event
	{
		enum class Type
		{
			CustomEvent
		};
	public:
		Type event_type = Type::CustomEvent;
		std::type_index event_type_index;
	public:
		Event(std::type_index type) : event_type_index(type){}
	};

	template<typename EventType>
	class EventObject : public Event
	{
	public:
		EventType data;
	public:
		EventObject(EventType data) : Event(typeid(EventType)) , data(data) {}
	};

	template<typename T>
	EventObject<T> CreateEventObject(T data)
	{
		return EventObject<T>{data};
	}

	template<typename T> 
	T GetEventData(Event& event_data)
	{
		return reinterpret_cast<EventObject<T>&>(event_data).data;
	}
}