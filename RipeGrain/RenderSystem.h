#pragma once
#include "RepulsiveEngine/CoreEngine.h"
#include "RepulsiveEngine/CustomWindow.h"

#include "EngineComponent.h"

namespace RipeGrain
{
	class RenderSystem : public EngineEventSubscriber
	{
	private:
		CoreEngine& renderer;
	private:
		WindowRenderer window_render_surface;
	private:
		std::list<std::unique_ptr<SceneObject>>* objects = nullptr;
	public:
		RenderSystem(CoreEngine& renderer ,CustomWindow& window): renderer(renderer), window_render_surface(renderer.CreateRenderer(window))
		{
			renderer.SetRenderDevice(window_render_surface);
		}
	public:	
		void OnUpdate() override
		{
			renderer.ClearFrame(window_render_surface);
			if (objects)
			{
				for (auto& object : *objects)
				{
					auto pos = object->GetPosition();
					for (auto sprite : object->GetSprites())
					{
						sprite.SetPosition(DirectX::XMVectorAdd(sprite.GetPosition(), pos));
						sprite.Draw(renderer);
					}
				}
			}
			window_render_surface.RenderFrame();
		}

		void OnEventReceive(Event& event_data) override
		{
			if (event_data.event_type_index == typeid(EventSceneLoaded))
			{
				auto data = GetEventData<EventSceneLoaded>(event_data);
				objects = data.objects;
			}
		}
	};
}