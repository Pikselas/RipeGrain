#pragma once
#include "BoxCollider.h"
#include "SceneManager.h"

class MovementAnimator
{
private:
	float t = 1.0f;
	DirectX::XMVECTOR strt;
	DirectX::XMVECTOR dest;
public:
	void Cancel(RipeGrain::SceneObject* object)
	{
		object->SetPosition(DirectX::XMVectorLerp(strt , dest , t - 0.1));
		dest = object->GetPosition();
		t = 1.0f;
	}
	void SetDestination(DirectX::XMVECTOR pos_start , DirectX::XMVECTOR pos_end)
	{
		t = 0.0f;
		strt = pos_start;
		dest = pos_end;	
	}
	void Update(RipeGrain::SceneObject* obj)
	{
		if (t >= 1.0f)
			return;
		t += 0.07;
		obj->SetPosition(DirectX::XMVectorLerp(strt, dest , t));
	}
	bool ReachedDestination() const
	{
		return t >= 1.0f;
	}
};

class Ninja final : public RipeGrain::SceneObject
{
public:
	Ninja(CoreEngine& sprite_engine) : RipeGrain::SceneObject(sprite_engine)
	{
		auto sp = CreateSprite(CreateTexture(Image{ "D:/ASSET/ninja_walk_50_frames.png" }), 106, 178);
		AddSprite(sp);
	}
};

class VillageScene;

class MainScene final: public RipeGrain::Scene
{
public:
	MainScene(CoreEngine& engine , RipeGrain::SceneLoader& scene_loader) 
		: 
		RipeGrain::Scene(engine , scene_loader),
		walking_animator([]()
			{
				std::vector<std::pair<unsigned int, unsigned int>> frames;
				for (int i = 1; i < 50; ++i)
					frames.emplace_back(i * 106, 0);
				return frames;
			}(), 1500)
	{	
		ninja = AddObject<Ninja>();
		ninja->SetPosition(100, 100);

		auto ninja2 = AddObject<Ninja>();
		ninja2->SetPosition(400, 400);

		auto collider = RipeGrain::BoxCollider{ ninja , 53 , 53 , 89 , 89 };
		auto collider2 = RipeGrain::BoxCollider{ ninja2 , 53 , 53 , 89 , 89 };

		using TCollider = RipeGrain::EventCollidersAdded;
		RegisterEvent(
			std::make_unique<
			RipeGrain::EventObject<TCollider>>(
				RipeGrain::CreateEventObject<TCollider>(
					TCollider{ std::vector{ collider , collider2 } }
				)
			)
		);
	}
public:
	void Update() override
	{
		if (!position_animator.ReachedDestination())
		{
			position_animator.Update(ninja);
			walking_animator.Animate(ninja->GetSprites()[0]);
		}
		Scene::Update();
	}
	void OnEventReceive(RipeGrain::Event& ev) override
	{
		if (ev.event_type_index == typeid(RipeGrain::EventKeyBoardInput))
		{
			KeyboardInput(RipeGrain::GetEventData<RipeGrain::EventKeyBoardInput>(ev));
		}
		else if (ev.event_type_index == typeid(RipeGrain::EventCollisionDetected))
		{
			auto data = RipeGrain::GetEventData<RipeGrain::EventCollisionDetected>(ev);
			for (auto [a,b] : data.collision_list)
			{
				if (a.GetInlineObject() == ninja || b.GetInlineObject() == ninja)
				{
					position_animator.Cancel(ninja);
				}
			}
		}
	}
private:
	void KeyboardInput(RipeGrain::EventKeyBoardInput ev)
	{
		if (ev.type == RipeGrain::EventKeyBoardInput::Type::KeyPress)
		{
			constexpr int dist = 55;
			switch (ev.key_code)
			{
			case 'W':
				if (position_animator.ReachedDestination()) position_animator.SetDestination(ninja->GetPosition(), DirectX::XMVectorSetY(ninja->GetPosition(), ninja->GetY() - dist));
				break;
			case 'A':
				if (position_animator.ReachedDestination()) position_animator.SetDestination(ninja->GetPosition(), DirectX::XMVectorSetX(ninja->GetPosition(), ninja->GetX() - dist));
				break;
			case 'S':
				if (position_animator.ReachedDestination()) position_animator.SetDestination(ninja->GetPosition(), DirectX::XMVectorSetY(ninja->GetPosition(), ninja->GetY() + dist));
				break;
			case 'D':
				if (position_animator.ReachedDestination()) position_animator.SetDestination(ninja->GetPosition(), DirectX::XMVectorSetX(ninja->GetPosition(), ninja->GetX() + dist));
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
private:
	MovementAnimator position_animator;
	RipeGrain::SpriteSheetAnimator walking_animator;
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