#include "Engine.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "RepulsiveEngine/StandardWindow.h"


class Scene
{
public:
	std::string s;
};

class SceneLoader
{
public:
	std::function<void(Scene)> OnSceneLoad = nullptr;
public:
	void Load(Scene scene)
	{
		if (OnSceneLoad)
			OnSceneLoad(scene);
	}
};

class SceneManager : public RipeGrain::EngineEventRaiser
{
public:
	SceneManager(SceneLoader& scene_loader)
	{
		scene_loader.OnSceneLoad = [this](Scene scene)
			{
				OnNewScene(scene);
			};
	}
	void OnNewScene(Scene scene)
	{
		auto ev = std::make_unique<RipeGrain::EventObject<RipeGrain::EventNewObject>>(RipeGrain::CreateEventObject(RipeGrain::EventNewObject{150 , 150 , 1}));
		RaiseEvent(std::move(ev));
	}
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SceneLoader scene_loader;
	RipeGrain::Engine engine;
	StandardWindow window("RipeGrain - The Engine");

	engine.ConfigureWith<RipeGrain::RenderSystem>(window);
	engine.ConfigureWith<SceneManager>(scene_loader);

	scene_loader.Load(Scene{});
	
	while (window.IsOpen())
	{
		engine.Run();
		Window::DispatchWindowEventsNonBlocking();
	}
	return 0;
}