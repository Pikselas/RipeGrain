#pragma once
#include "RepulsiveEngine/CoreEngine.h"
#include "RepulsiveEngine/StandardWindow.h"

namespace RipeGrain
{
	class Engine
	{
	private:
		CoreEngine renderer;
		StandardWindow window;
	private:
		WindowRenderer window_render_surface;
	private:
		ImageSprite sprite;
	public:
		Engine() : window("RipeGrain Engine") , window_render_surface(renderer.CreateRenderer(window))
		{
			renderer.SetRenderDevice(window_render_surface);
			Image img("D:/wallpaperflare-cropped.jpg");
			sprite = renderer.CreateSprite(img);
			sprite.SetPosition(DirectX::XMVectorSet(100, 100, 0, 1));
		}
	public:
		void Run()
		{
			while (window.IsOpen())
			{
				renderer.ClearFrame(window_render_surface);
				sprite.Draw(renderer);
				window_render_surface.RenderFrame();
				Window::DispatchWindowEventsNonBlocking();
			}
		}
	};
}