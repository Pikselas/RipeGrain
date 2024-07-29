#include "Engine.h"
#include "RenderSystem.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	RipeGrain::Engine engine;
	
	auto& window = 
	engine.ConfigureWith<RipeGrain::RenderSystem>().window;
	
	while (window.IsOpen())
	{
		engine.Run();
	}
	return 0;
}