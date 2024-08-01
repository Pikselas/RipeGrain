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
	
	auto sprite1 = render_engine.CreateSprite(render_engine.CreateTexture(Image{ "D:/ASSET/ninja_walk_50_frames.png" }), 106, 178);
	auto anim_frames = []() -> std::vector<std::pair<unsigned int, unsigned int>>
	{
		std::vector<std::pair<unsigned int, unsigned int>> v;
		for (int i = 0; i < 50; ++i)
		{
			v.emplace_back(i * 106, 0);
		}
		return v;
	}();
	
	std::vector<Texture> txs;

	for (int i = 0; i <= 13; ++i)
	{
		txs.emplace_back(render_engine.CreateTexture(Image{ R"(C:\Users\Aritra Maji\Downloads\guard_effect\green_shield_.)" + std::to_string(i) + ".png" }));
	}
	auto sprite2 = render_engine.CreateSprite(txs.front(), txs.front().GetWidth(), txs.front().GetHeight());

	sprite1.SetPosition(DirectX::XMVectorSet(window.GetWidth() / 2, window.GetHeight() / 2, 0, 1));
	sprite2.SetPosition(DirectX::XMVectorSet(window.GetWidth() / 2, window.GetHeight() / 2, 0, 1));

	scene1.AddObjectAnimator(std::make_unique<RipeGrain::SpriteSheetAnimator>(scene1.AddSprite(sprite1), anim_frames , 4000));
	scene1.AddObjectAnimator(std::make_unique<RipeGrain::TextureBatchAnimator>(scene1.AddSprite(sprite2), txs, 2500));

	scene_loader.Load(scene1);

	while (window.IsOpen())
	{
		engine.Run();
		Window::DispatchWindowEventsNonBlocking();
	}
	return 0;
}