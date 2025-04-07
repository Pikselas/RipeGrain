#include "Engine.h"
#include "UISystem.h"
#include "AudioSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "RepulsiveEngine/StandardWindow.h"

#include "MainScene.h"
#include "DynamicSceneLoader.h"
#include "RpsLoader.h"

auto GetProgramDir()
{
	char buffer[MAX_PATH];
	GetModuleFileName(nullptr, buffer, 100);
	std::filesystem::path path = buffer;
	return path.parent_path();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	RipeGrain::RipeGrainSettings rps;
	rps.Load("Startup.rps");
	CoreEngine render_engine;
	StandardWindow window(rps.GetValue("Title"));
	DynamicSceneLoader::Load( GetProgramDir() / rps.GetValue("Load"));
	
	window.keyboard.EnableKeyRepeat();

	{
		RipeGrain::Engine engine;
		engine.ConfigureWith<RipeGrain::UISystem>();
		engine.ConfigureWith<RipeGrain::AudioSystem>();
		engine.ConfigureWith<RipeGrain::PhysicsSystem>();
		engine.ConfigureWith<RipeGrain::InputSystem>(window.mouse);
		engine.ConfigureWith<RipeGrain::SceneManager>(render_engine);
		engine.ConfigureWith<RipeGrain::InputSystem>(window.keyboard);
		engine.ConfigureWith<RipeGrain::RenderSystem>(render_engine, window);
		engine.ConfigureWith<RipeGrain::SceneLoader>().LoadScene(DynamicSceneLoader::GetScene(), DynamicSceneLoader::DeleteScene);

		while (window.IsOpen())
		{
			engine.Run();
			Window::DispatchWindowEventsNonBlocking();
		}
	}

	DynamicSceneLoader::Unload();
	return 0;
}