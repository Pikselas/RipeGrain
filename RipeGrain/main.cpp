#include "Engine.h"
#include "UISystem.h"
#include "AudioSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"
#include "PhysicsSystem.h"
#include "RepulsiveEngine/StandardWindow.h"

__declspec(dllexport) void initialize_engine();
__declspec(dllexport) void play_scene(const char* title, void* scene, void(*deleter)(void*));

auto GetProgramDir()
{
	char buffer[MAX_PATH];
	GetModuleFileName(GetModuleHandle(nullptr), buffer, 100);
	std::filesystem::path path = buffer;
	return path.parent_path();
}

std::unique_ptr<CoreEngine> render_engine;

void initialize_engine()
{
	render_engine = std::make_unique<CoreEngine>();
}

void play_scene(const char* title , void * scene , void(*deleter)(void*))
{

	/*CoreEngine render_engine;*/
	StandardWindow window(title);
	
	window.keyboard.EnableKeyRepeat();

	{
		RipeGrain::Engine engine;
		engine.ConfigureWith<RipeGrain::UISystem>();
		engine.ConfigureWith<RipeGrain::AudioSystem>();
		engine.ConfigureWith<RipeGrain::PhysicsSystem>();
		engine.ConfigureWith<RipeGrain::InputSystem>(window.mouse);
		engine.ConfigureWith<RipeGrain::SceneManager>(*render_engine);
		engine.ConfigureWith<RipeGrain::InputSystem>(window.keyboard);
		engine.ConfigureWith<RipeGrain::RenderSystem>(*render_engine, window);
		engine.ConfigureWith<RipeGrain::SceneLoader>().LoadScene(reinterpret_cast<RipeGrain::Scene*>(scene), deleter);

		while (window.IsOpen())
		{
			engine.Run();
			Window::DispatchWindowEventsNonBlocking();
		}
	}
}