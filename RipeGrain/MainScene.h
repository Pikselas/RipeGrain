#pragma once
#include "SceneManager.h"
class MainScene;

class VillageScene : public RipeGrain::Scene
{
public:
	VillageScene(CoreEngine& engine, RipeGrain::SceneLoader& scene_loader) : RipeGrain::Scene(engine, scene_loader)
	{
		auto sp = AddSprite(Image{ "D:/SeqDownLogo.bmp" });
		sp.SetPosition(250, 250);

		OnKeyBoardInput = [this](RipeGrain::EventKeyBoardInput ev) { if (ev.key_code == 'X' && ev.type == RipeGrain::EventKeyBoardInput::Type::KeyPress) this->scene_loader.LoadScene<MainScene>(); };
	}
};

class MainScene : public RipeGrain::Scene
{
public:
	MainScene(CoreEngine& engine , RipeGrain::SceneLoader& scene_loader) : RipeGrain::Scene(engine , scene_loader)
	{
		ninja = AddSprite(Image{"D:/ASSET/ninja_walk_50_frames.png"} , 106 , 178);
		OnKeyBoardInput = [this](RipeGrain::EventKeyBoardInput ev) { KeyboardInput(ev); };

		ninja->SetPosition(100, 100);
	}
public:
	void Update() override
	{
		
	}
private:
	void KeyboardInput(RipeGrain::EventKeyBoardInput ev)
	{
		if (ev.type == RipeGrain::EventKeyBoardInput::Type::KeyPress)
		{
			constexpr int dist = 2;
			switch (ev.key_code)
			{
			case 'W':
				ninja->SetY(ninja->GetY() - dist);
				break;
			case 'A':
				ninja->SetX(ninja->GetX() - dist);
				break;
			case 'S':
				ninja->SetY(ninja->GetY() + dist);
				break;
			case 'D':
				ninja->SetX(ninja->GetX() + dist);
				break;
			case 'X':
				scene_loader.LoadScene<VillageScene>();
				break;
			}
		}
	}
private:
	std::optional<RipeGrain::SceneObject> ninja;
};
