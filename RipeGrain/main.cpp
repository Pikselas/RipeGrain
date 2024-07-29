#include "Engine.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	RipeGrain::Engine engine;
	engine.Run();
	return 0;
}