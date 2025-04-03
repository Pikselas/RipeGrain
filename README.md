## TOY Framework for creating 2D games

### Engine configuration 
> Edit [main.cpp](RipeGrain/main.cpp)
```C++
#include "Engine.h"
#include "UISystem.h"
#include "AudioSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "RepulsiveEngine/StandardWindow.h"

#include "MainScene.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CoreEngine render_engine;
	RipeGrain::Engine engine;
	StandardWindow window("RipeGrain - The Engine");
	
	window.keyboard.EnableKeyRepeat();

	engine.ConfigureWith<RipeGrain::UISystem>();
	engine.ConfigureWith<RipeGrain::AudioSystem>();
	engine.ConfigureWith<RipeGrain::PhysicsSystem>();
	engine.ConfigureWith<RipeGrain::InputSystem>(window.mouse);
	engine.ConfigureWith<RipeGrain::SceneManager>(render_engine);
	engine.ConfigureWith<RipeGrain::InputSystem>(window.keyboard);
	engine.ConfigureWith<RipeGrain::RenderSystem>(render_engine , window);
	engine.ConfigureWith<RipeGrain::SceneLoader>().LoadScene<MainScene>();

	while (window.IsOpen())
	{
		engine.Run();
		Window::DispatchWindowEventsNonBlocking();
	}
	return 0;
}
```
### Creating scene
