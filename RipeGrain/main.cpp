#include "RepulsiveEngine/StandardWindow.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	StandardWindow window;
	while (window.IsOpen())
	{
		Window::DispatchWindowEventsNonBlocking();
	}
}