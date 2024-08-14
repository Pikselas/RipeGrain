#pragma once
#include "SceneManager.h"

class Ninja final : public RipeGrain::SceneObject
{
public:
	Ninja(CoreEngine& sprite_engine) : RipeGrain::SceneObject(sprite_engine) , 
		walking_animator([]()
			{ 
				std::vector<std::pair<unsigned int, unsigned int>> frames;
				for (int i = 1; i < 50; ++i)
					frames.emplace_back(i * 106 , 0);
				return frames;
			}(), 1500)
	{
		auto sp = CreateSprite(CreateTexture(Image{ "D:/ASSET/ninja_walk_50_frames.png" }), 106, 178);
		AddSprite(sp);
	}
public:
	void Update() override
	{
		walking_animator.Animate(sprites.front());
	}
private:
	RipeGrain::SpriteSheetAnimator walking_animator;
};

class VillageScene;

class MainScene final: public RipeGrain::Scene
{
public:
	MainScene(CoreEngine& engine , RipeGrain::SceneLoader& scene_loader) : RipeGrain::Scene(engine , scene_loader)
	{
		OnKeyBoardInput = [this](RipeGrain::EventKeyBoardInput ev) { KeyboardInput(ev); };
		
		ninja = AddObject<Ninja>();
		ninja->SetPosition(100, 100);
	}
public:
	void Update() override
	{
		Scene::Update();
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
				LoadScene<VillageScene>();
				break;
			case 'Y':
				LoadScene<VillageScene>(150 , 250);
				break;
			}
		}
	}
private:
	Ninja* ninja;
};

class VillageScene final : public RipeGrain::Scene
{
public:
	VillageScene(CoreEngine& engine, RipeGrain::SceneLoader& scene_loader) : VillageScene(engine, scene_loader, 150 , 150){}
	VillageScene(CoreEngine& engine, RipeGrain::SceneLoader& scene_loader , int x ,int y) : RipeGrain::Scene(engine, scene_loader)
	{
		obj = AddObject<RipeGrain::SceneObject>();
		obj->AddSprite(obj->CreateSprite(Image{"D:/SeqDownLogo.bmp"}));
		obj->SetPosition(x, y);
	}
private:
	RipeGrain::SceneObject* obj;
};