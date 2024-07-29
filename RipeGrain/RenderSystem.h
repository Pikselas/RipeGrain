#pragma once
#include "RepulsiveEngine/CoreEngine.h"
#include "RepulsiveEngine/StandardWindow.h"

#include "EngineComponent.h"

namespace RipeGrain
{
	class RenderSystem : public EngineComponent
	{
	private:
		CoreEngine renderer;
	public:
		StandardWindow window;
	private:
		WindowRenderer window_render_surface;
	private:
		ImageSprite sprite;
	public:
		RenderSystem() : window("RipeGrain Engine"), window_render_surface(renderer.CreateRenderer(window))
		{
			renderer.SetRenderDevice(window_render_surface);
			Image img("D:/wallpaperflare-cropped.jpg");
			sprite = renderer.CreateSprite(img);
			sprite.SetPosition(DirectX::XMVectorSet(100, 100, 0, 1));
		}
	public:	
		void RenderObject()
		{
			sprite.Draw(renderer);
		}

		void SetNewFrame()
		{
			window_render_surface.RenderFrame();
			Window::DispatchWindowEventsNonBlocking();
			renderer.ClearFrame(window_render_surface);
		}

		void OnUpdate() override
		{
			RenderObject();
			SetNewFrame();
		}
	};
}