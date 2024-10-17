#pragma once
#include "RepulsiveEngine/CoreEngine.h"
#include "RepulsiveEngine/CustomWindow.h"

#include "EngineComponent.h"
#include "UIComponent.h"
#include "SceneManager.h"

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
		UIComponent main_ui_component;
	private:
		const Scene* scene = nullptr;
	public:
		RenderSystem(CoreEngine& renderer ,CustomWindow& window): renderer(renderer),render_window(window), window_render_surface(std::make_unique<WindowRenderer>(renderer.CreateRenderer(window)))
		{
			renderer.SetRenderDevice(*window_render_surface);
		}
	public:	
		void OnUpdate() override
		{
			renderer.ClearFrame(*window_render_surface);
			if (scene)
			{
				scene->Render(renderer);
			}
			main_ui_component.Render(renderer, 0, 0);
			window_render_surface->RenderFrame();
		}

		void OnEventReceive(Event& event_data) override
		{
			if (event_data.event_type_index == typeid(EventSceneLoaded))
			{
				scene = GetEventData<EventSceneLoaded>(event_data).scene;
			}
			else if (event_data.event_type_index == typeid(EventResizeScreen))
			{
				auto [w,h] = GetEventData<EventResizeScreen>(event_data);
				render_window.ResizeWindow(w, h);
				window_render_surface = std::make_unique<WindowRenderer>(renderer.CreateRenderer(render_window));
				renderer.SetRenderDevice(*window_render_surface);
			}
			else if (event_data.event_type_index == typeid(EventSetUIFrame))
			{
				main_ui_component = GetEventData<EventSetUIFrame>(event_data).component;
			}
		}
	};
}