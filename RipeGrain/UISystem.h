#pragma once
#include <memory>
#include "UILayer.h"
#include "EngineComponent.h"
namespace RipeGrain
{
	class UISystem : public EngineEventRaiser , public EngineEventSubscriber
	{
	private:
		UILayer* ui_layer = nullptr;
	public:
		void OnEventReceive(Event& ev) override
		{
			if (ev.event_type_index == typeid(EventSceneLoaded))
			{
				ui_layer = nullptr;
			}
			else if (ev.event_type_index == typeid(EventRegisterUI))
			{
				ui_layer = GetEventData<EventRegisterUI>(ev).layer;
			}
			else if (ui_layer)
			{
				if (ev.event_type_index == typeid(EventMouseInput))
				{
					auto m_ev = GetEventData<EventMouseInput>(ev);
					
					m_ev.x_pos -= DirectX::XMVectorGetX(ui_layer->GetBasePosition());
					m_ev.y_pos -= DirectX::XMVectorGetY(ui_layer->GetBasePosition());

					for (auto& component : ui_layer->getComponents())
					{
						if (component.IsInRange(m_ev.x_pos , m_ev.y_pos))
						{
							auto mev_data = m_ev;
							mev_data.x_pos = m_ev.x_pos - component.GetX();
							mev_data.y_pos = m_ev.y_pos - component.GetY();
							component.OnEvent(mev_data);
							//break;
						}
						else
						{
							component.Focused = false;
						}
					}
				}
				else if (ev.event_type_index == typeid(EventKeyBoardInput))
				{
					for (auto& component : ui_layer->getComponents())
					{
						if (component.Focused)
						{
							component.OnEvent(GetEventData<EventKeyBoardInput>(ev));
							return;
						}
					}
				}
			}
		}
		void OnUpdate() override
		{

		}
	};
}