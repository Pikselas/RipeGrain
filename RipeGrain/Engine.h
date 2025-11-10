#pragma once
#include <list>
#include <memory>
#include <type_traits>
#include "EngineComponent.h"

namespace RipeGrain
{
	template<typename T>
	concept CEngineComponent = std::is_base_of_v<EngineComponent, T>;

	class Engine
	{
	private:
		std::list<std::unique_ptr<EngineComponent>> components;
	private:
		std::list<EngineEventSubscriber*> event_subscribers;
	private:
		std::queue<EngineEventObject> event_queue;
	private:
		std::unordered_map<std::type_index, EngineComponent*> queried_components;
	public:
		Engine() = default;
	public:
		template<CEngineComponent Component , typename ... ParamsT>
		Component& ConfigureWith(ParamsT&&... params)
		{
			auto component = std::make_unique<Component>(std::forward<ParamsT>(params)...);
			
			if constexpr (std::is_base_of_v<EngineEventSubscriber, Component>)
			{
				event_subscribers.push_back(component.get());
			}
			
			if constexpr (std::is_base_of_v<EngineEventRaiser, Component>)
			{
				component->SetEventQueue(&event_queue);
			}

			auto& comp_ref = *component;
			components.emplace_back(std::move(component));

			return comp_ref;
		}	
		void Run()
		{
			while (!event_queue.empty())
			{
				for (auto& component : event_subscribers)
				{
					auto& ev = *event_queue.front().get();
					if (dynamic_cast<EngineEventSubscriber*>(ev.sender) != component)
					{
						component->OnEventReceive(ev);
					}
				}
				event_queue.pop();
			}

			for (auto& component : components)
				component->OnUpdate();
		}
	public:
		~Engine() = default;
	public:
		template<typename T>
		T* QueryComponent()
		{
			if (auto ptr = queried_components.find(typeid(T)); ptr != queried_components.end())
			{
				return dynamic_cast<T*>(ptr->second);
			}

			for (auto& component : components)
			{
				if (auto casted = dynamic_cast<T*>(component.get()); casted != nullptr)
				{
					queried_components[typeid(T)] = component.get();
					return casted;
				}
			}
			return nullptr;
		}
	};
}