#pragma once
#include "BoxCollider.h"
#include "EngineComponent.h"

namespace RipeGrain
{
	class PhysicsSystem : public EngineEventRaiser, public EngineEventSubscriber
	{
	private:
		std::vector<BoxCollider> colliders;
	public:
		void OnEventReceive(Event& ev) override
		{
			if (ev.event_type_index == typeid(EventCollidersAdded))
			{
				auto data = GetEventData<EventCollidersAdded>(ev);
				colliders = data.colliders;
			}
		}
	public:
		void OnUpdate() override
		{
			std::sort(colliders.begin(), colliders.end(), [](BoxCollider& a, BoxCollider& b) 
			{
				return a.GetLeft() < b.GetLeft();
			});

			std::vector<std::pair<BoxCollider , BoxCollider>> collision_list;
			for (unsigned int i = 0; i < colliders.size() - 1; ++i)
			{
				if (colliders[i].TestCollision(colliders[i + 1]))
					collision_list.emplace_back(colliders[i], colliders[i + 1]);
			}

			if (!collision_list.empty())
				RaiseEvent(std::make_unique<EventObject<EventCollisionDetected>>(CreateEventObject(EventCollisionDetected{ collision_list })));
		}
	};
}