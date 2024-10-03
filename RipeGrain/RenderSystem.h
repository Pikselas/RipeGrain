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
		CustomWindow& render_window;
	private:
		std::unique_ptr<WindowRenderer> window_render_surface;
	private:
		const DirectX::XMVECTOR* base_position = nullptr;
		const std::vector<SceneObject*>* objects = nullptr;
	public:
		RenderSystem(CoreEngine& renderer ,CustomWindow& window): renderer(renderer),render_window(window), window_render_surface(std::make_unique<WindowRenderer>(renderer.CreateRenderer(window)))
		{
			renderer.SetRenderDevice(*window_render_surface);
		}
	public:	
		void OnUpdate() override
		{
			renderer.ClearFrame(*window_render_surface);
			if (objects)
			{
				for (auto& object : *objects)
				{
					auto pos = object->GetPosition();
					for (auto sprite : object->GetSprites())
					{
						sprite.SetPosition(DirectX::XMVectorAdd(DirectX::XMVectorAdd(sprite.GetPosition(), pos),*base_position));
						sprite.Draw(renderer);
					}
				}
			}
			window_render_surface->RenderFrame();
		}

		void OnEventReceive(Event& event_data) override
		{
			if (event_data.event_type_index == typeid(EventSceneLoaded))
			{
				auto data = GetEventData<EventSceneLoaded>(event_data);
				objects = data.objects;
				base_position = data.scene_position;
			}
			else if (event_data.event_type_index == typeid(EventResizeScreen))
			{
				auto [w,h] = GetEventData<EventResizeScreen>(event_data);
				render_window.ResizeWindow(w, h);
				window_render_surface = std::make_unique<WindowRenderer>(renderer.CreateRenderer(render_window));
				renderer.SetRenderDevice(*window_render_surface);
			}
		}
	};
}