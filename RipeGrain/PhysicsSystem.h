#pragma once
#include <ranges>
#include "DynamicAABB.h"
#include "SpatialHashGrid.h"
#include "EngineComponent.h"

namespace RipeGrain
{
	class PhysicsSystem : public EngineEventRaiser, public EngineEventSubscriber
	{
	private:
		std::unique_ptr<SpatialHashGrid> hash_grid;
	private:
		std::vector<std::pair<BoxCollider , BoxCollider>> colliders;
		std::vector<std::pair<BoxCollider, std::vector<BoxCollider>>> collision_list;
	public:
		void OnEventReceive(Event& ev) override
		{
			if (ev.event_type_index == typeid(EventCollidersAdded))
			{
				auto data = GetEventData<EventCollidersAdded>(ev);

				int last_area = 0;
				BoxCollider last_col;

				for (const auto& c : data.colliders)
				{
					if (c.GetArea() > last_area)
					{
						last_area = c.GetArea();
						last_col = c.GetStaticCollider();
					}
					colliders.emplace_back(c, c.GetStaticCollider());
				}

				int side = (std::max)(last_col.GetRight() - last_col.GetLeft(), last_col.GetBottom() - last_col.GetTop());
				hash_grid = std::make_unique<SpatialHashGrid>(side);
			}
			else if (ev.event_type_index == typeid(EventSceneLoaded))
			{
				if(hash_grid)
					hash_grid->Clear();
			}
		}
	public:
		void OnUpdate() override
		{
			if (hash_grid != nullptr)
			{
				hash_grid->Clear();
				collision_list.clear();

				for (auto& [collider, static_collider] : colliders)
				{
					//hash_grid->Remove(static_collider);
					hash_grid->Insert(collider.GetStaticCollider() , collider);
					static_collider = collider.GetStaticCollider();
				}

				struct EqualBounds
				{
					bool operator()(const BoxCollider& c1, const BoxCollider& c2) const
					{
						return c1.GetLeft() == c2.GetLeft() && c1.GetRight() == c2.GetRight() && c1.GetTop() == c2.GetTop() && c1.GetBottom() == c2.GetBottom();
					}
				};
				//std::unordered_map<BoxCollider, std::unordered_set<BoxCollider, BoxColliderHasher , EqualBounds>, BoxColliderHasher , EqualBounds> collision_list;
				
				for (auto& [collider, static_collider] : colliders)
				{
					auto neighbours = hash_grid->FindNear(static_collider);
					std::vector<BoxCollider> temp_lst;
					for (auto& c : neighbours)
					{
						if (static_collider.IsCollidingWith(c))
						{
							temp_lst.push_back(c);
						}
					}

					if (!temp_lst.empty())
					{
						collision_list.emplace_back(collider , std::move(temp_lst));
					}
				}
			
				if (!collision_list.empty())
				{
					RaiseEvent(CreateEventObject(EventCollisionDetected{ collision_list }));
				}
			}
		}
	};
}