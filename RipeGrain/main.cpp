#include "Engine.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "RepulsiveEngine/StandardWindow.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CoreEngine render_engine;
	RipeGrain::Engine engine;
	RipeGrain::SceneLoader scene_loader;
	StandardWindow window("RipeGrain - The Engine");

	engine.ConfigureWith<RipeGrain::SceneManager>(scene_loader);
	engine.ConfigureWith<RipeGrain::RenderSystem>(render_engine , window);

	RipeGrain::Scene scene1(render_engine);
	Image img("C:/Users/Aritra Maji/Downloads/ripe_grain_logo.png");
	scene1.AddSprite(img).SetPosition(DirectX::XMVectorSet(window.GetWidth() / 2, window.GetHeight() / 2, 1, 1));

	scene_loader.Load(scene1);
	
	while (window.IsOpen())
	{
		engine.Run();
		Window::DispatchWindowEventsNonBlocking();
	}
	return 0;
}