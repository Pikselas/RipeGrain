#pragma once
#include <memory>
#include "UIComponent.h"
#include "EngineComponent.h"
namespace RipeGrain
{
	class UISystem : public EngineEventRaiser , public EngineEventSubscriber
	{
	public:
		void OnEventReceive(Event& ev) override
		{

		}
		void OnUpdate() override
		{

		}
	};
}