#include "Engine.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "InputSystem.h"
#include "RepulsiveEngine/StandardWindow.h"

#include "MainScene.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CoreEngine render_engine;
	RipeGrain::Engine engine;
	StandardWindow window("RipeGrain - The Engine");
	RipeGrain::SceneLoader scene_loader(render_engine);
	
	window.keyboard.EnableKeyRepeat();

	engine.ConfigureWith<RipeGrain::InputSystem>(window.mouse);
	engine.ConfigureWith<RipeGrain::SceneManager>(scene_loader);
	engine.ConfigureWith<RipeGrain::InputSystem>(window.keyboard);
	engine.ConfigureWith<RipeGrain::RenderSystem>(render_engine , window);

	scene_loader.LoadScene<MainScene>();
	
	while (window.IsOpen())
	{
		engine.Run();
		Window::DispatchWindowEventsNonBlocking();
	}
	return 0;
}