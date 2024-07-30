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
		std::queue<std::unique_ptr<Event>> event_queue;
	public:
		Engine() = default;
	public:
		template<CEngineComponent Component , typename ... ParamsT>
		Component& ConfigureWith(ParamsT&... params)
		{
			auto component = std::make_unique<Component>(params...);
			
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
				for (auto& compoenent : event_subscribers)
					compoenent->OnEventReceive(*event_queue.front());
				event_queue.pop();
			}

			for (auto& component : components)
				component->OnUpdate();
		}
	};
}