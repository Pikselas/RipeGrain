#pragma once
#include <queue>
#include "Event.h"

namespace RipeGrain
{
	class EngineComponent
	{
	public:
		virtual void OnUpdate() {};
	public:
		virtual ~EngineComponent() = default;
	};

	class EngineEventRaiser : public virtual EngineComponent
	{
	private:
		std::queue<EngineEventObject>* event_queue = nullptr;
	protected:
		void RaiseEvent(EngineEventObject ev)
		{
			ev->sender = this;
			event_queue->push(std::move(ev));
		}
	public:
		void SetEventQueue(std::queue<EngineEventObject>* queue)
		{
			event_queue = queue;
		}
	};

	class EngineEventSubscriber : public virtual EngineComponent
	{
	public:
		virtual void OnEventReceive(Event& ev) {}
	};
}