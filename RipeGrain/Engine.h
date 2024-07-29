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
	public:
		Engine() = default;
	public:
		template<CEngineComponent Component , typename ... ParamsT>
		Component& ConfigureWith(ParamsT... params)
		{
			auto component = std::make_unique<Component>(params...);
			
			if constexpr (std::is_base_of_v<EngineEventSubscriber, Component>)
			{

			}
			
			if constexpr (std::is_base_of_v<EngineEventRaiser, Component>)
			{

			}

			auto& comp_ref = *component;
			components.emplace_back(std::move(component));

			return comp_ref;
		}
		void Run()
		{
			for (auto& component : components)
				component->OnUpdate();
		}
	};
}