#include "Engine.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "InputSystem.h"
#include "RepulsiveEngine/StandardWindow.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CoreEngine render_engine;
	RipeGrain::Engine engine;
	RipeGrain::SceneLoader scene_loader;
	StandardWindow window("RipeGrain - The Engine");
	window.keyboard.EnableKeyRepeat();

	engine.ConfigureWith<RipeGrain::InputSystem>(window.mouse);
	engine.ConfigureWith<RipeGrain::SceneManager>(scene_loader);
	engine.ConfigureWith<RipeGrain::InputSystem>(window.keyboard);
	engine.ConfigureWith<RipeGrain::RenderSystem>(render_engine , window);

	RipeGrain::Scene scene1(render_engine);
	
	auto object = scene1.AddSprite(Image{ "D:/ASSET/ninja_walk_50_frames.png" }, 106, 178);
	auto anim_frames = []() -> std::vector<std::pair<unsigned int, unsigned int>>
	{
		std::vector<std::pair<unsigned int, unsigned int>> v;
		for (int i = 0; i < 50; ++i)
		{
			v.emplace_back(i * 106, 0);
		}
		return v;
	}();
	
	object.SetPosition(window.GetWidth() / 2, window.GetHeight() / 2);
	scene1.AddObjectAnimator(std::make_unique<RipeGrain::SpriteSheetAnimator>(object, anim_frames , 4000));

	scene1.OnKeyBoardInput = [object](RipeGrain::EventKeyBoardInput ev) mutable
		{
			int x = object.GetX();
			int y = object.GetY();
			
			switch (ev.key_code)
			{
			case 'W':
				y -= 10;
				break;
			case 'A':
				x -= 10;
				break;
			case 'S':
				y += 10;
				break;
			case 'D':
				x += 10;
			}

			object.SetPosition(x, y);
		};

	scene_loader.Load(scene1);

	while (window.IsOpen())
	{
		engine.Run();
		Window::DispatchWindowEventsNonBlocking();
	}
	return 0;
}