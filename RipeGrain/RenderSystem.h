#pragma once
#include "RepulsiveEngine/CoreEngine.h"
#include "RepulsiveEngine/CustomWindow.h"

#include "EngineComponent.h"

namespace RipeGrain
{
	class RenderSystem : public EngineEventSubscriber
	{
	private:
		CoreEngine renderer;
	private:
		WindowRenderer window_render_surface;
	private:
		std::vector<ImageSprite> sprites;
	public:
		RenderSystem(CustomWindow& window): window_render_surface(renderer.CreateRenderer(window))
		{
			renderer.SetRenderDevice(window_render_surface);
		}
	public:	
		void OnUpdate() override
		{
			renderer.ClearFrame(window_render_surface);

			for (auto& sprite : sprites)
				sprite.Draw(renderer);

			window_render_surface.RenderFrame();
		}

		void OnEventReceive(Event& event_data) override
		{
			if (event_data.event_type_index == typeid(EventNewObject))
			{
				auto data = GetEventData<EventNewObject>(event_data);
				Image img{150 , 150};
				img.Clear({ 255 , 167 , 129 , 60 });
				auto sprite = renderer.CreateSprite(img);
				sprite.SetPosition(DirectX::XMVectorSet(data.x, data.y, data.z , 1));
				sprites.emplace_back(sprite);
			}
		}
	};
}