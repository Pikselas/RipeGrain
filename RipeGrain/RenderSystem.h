#pragma once
#include <chrono>
#include "RepulsiveEngine/CoreEngine.h"
#include "RepulsiveEngine/CustomWindow.h"

#include "EngineServices.h"
#include "EngineComponent.h"
#include "SceneManager.h"

namespace RipeGrain
{
	class RenderSystem : public EngineEventSubscriber, public RenderService
	{
	private:
		CoreEngine& renderer;
		CustomWindow& render_window;
	private:
		std::unique_ptr<WindowRenderer> window_render_surface;
	private:
		const Scene* scene = nullptr;
	private:
		float last_frame_delta = 0.0f;
	public:
		RenderSystem(CoreEngine& renderer ,CustomWindow& window): renderer(renderer),render_window(window), window_render_surface(std::make_unique<WindowRenderer>(renderer.CreateRenderer(window)))
		{
			renderer.SetRenderDevice(*window_render_surface);
		}
	public:
		float GetFrameDelta() const override
		{
			return last_frame_delta;
		}
	public:	
		void OnUpdate() override
		{
			auto start_time = std::chrono::high_resolution_clock::now();
			renderer.ClearFrame(*window_render_surface);
			if (scene)
			{
				scene->Render(renderer);
			}
			window_render_surface->RenderFrame();
			last_frame_delta = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start_time).count();
		}

		void OnEventReceive(Event& event_data) override
		{
			if (event_data.event_type_index == typeid(EventSceneLoaded))
			{
				scene = GetEventData<EventSceneLoaded>(event_data).scene.get();
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