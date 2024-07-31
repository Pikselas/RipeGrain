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
		EventObject() : Event(typeid(EventType)) {}
	};

	template<typename T>
	EventObject<T> CreateEventObject(T data)
	{
		EventObject<T> object;
		object.data = data;
		return object;
	}

	template<typename T> 
	T GetEventData(Event& event_data)
	{
		return reinterpret_cast<EventObject<T>&>(event_data).data;
	}
}