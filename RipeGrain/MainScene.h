#pragma once
#include "BoxCollider.h"
#include "SceneManager.h"
#include "DynamicAABB.h"

class MovementAnimator
{
private:
	float t = 1.0f;
	DirectX::XMVECTOR strt;
	DirectX::XMVECTOR dest;
public:
	void Cancel(RipeGrain::SceneObject* object , const RipeGrain::BoxCollider& c1 , const RipeGrain::BoxCollider& c2)
	{
		while (c1.IsCollidingWith(c2))
		{
			object->SetPosition(DirectX::XMVectorLerp(strt, dest, t -= 0.02));
		}
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

class VillageScene;

class MainScene final: public RipeGrain::Scene
{
private:
	RipeGrain::BoxCollider collider;
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
		ninja.AddSprite(CreateSprite(Image{ "D:/ASSET/ninja_walk_50_frames.png" }, 106, 178));
		ninja2 = ninja;
		ninja5 = ninja4 = ninja3 = ninja3 = ninja2;

		ninja.SetPosition(300, 100);
		ninja2.SetPosition(400, 400);
		ninja3.SetPosition(150, 250);
		ninja4.SetPosition(250, 350);
		ninja5.SetPosition(350, 350);

		AddObject(&ninja);
		AddObject(&ninja2);
		AddObject(&ninja3);
		AddObject(&ninja4);
		AddObject(&ninja5);
	}
public:
	void Initialize() override
	{
		using ColliderT = RipeGrain::BoxCollider;
		auto Col = ColliderT{ &ninja , 53 , 53 , 89 , 89 };
		using TCollider = RipeGrain::EventCollidersAdded;
		RegisterEvent(
			std::make_unique<
			RipeGrain::EventObject<TCollider>>(
				RipeGrain::CreateEventObject<TCollider>(
					TCollider{ std::vector{ 
						Col,
						ColliderT{ &ninja2 , 53 , 53 , 89 , 89 },
						ColliderT{ &ninja3 , 53 , 53 , 89 , 89 },
						ColliderT{ &ninja4 , 53 , 53 , 89 , 89 },
						ColliderT{ &ninja5 , 53 , 53 , 89 , 89 },
					}}
				)
			)
		);
	}
	void Update() override
	{
		if (!position_animator.ReachedDestination())
		{
			position_animator.Update(&ninja);
			walking_animator.Animate(ninja.GetSprites()[0]);
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
			for (auto& [c , lst] : data.collision_list)
			{
				if (c.GetInlineObject() == &ninja)
				{
					position_animator.Cancel(&ninja , c , lst.front());
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
				if (position_animator.ReachedDestination()) position_animator.SetDestination(ninja.GetPosition(), DirectX::XMVectorSetY(ninja.GetPosition(), ninja.GetY() - dist));
				break;
			case 'A':
				if (position_animator.ReachedDestination()) position_animator.SetDestination(ninja.GetPosition(), DirectX::XMVectorSetX(ninja.GetPosition(), ninja.GetX() - dist));
				break;
			case 'S':
				if (position_animator.ReachedDestination()) position_animator.SetDestination(ninja.GetPosition(), DirectX::XMVectorSetY(ninja.GetPosition(), ninja.GetY() + dist));
				break;
			case 'D':
				if (position_animator.ReachedDestination()) position_animator.SetDestination(ninja.GetPosition(), DirectX::XMVectorSetX(ninja.GetPosition(), ninja.GetX() + dist));
				break;
			case 'X':
				SetBasePosition(DirectX::XMVectorSubtract(GetBasePosition() , DirectX::XMVectorSet(25 , 0 , 0 , 1)));
				break;
			case 'Z':
				SetBasePosition(DirectX::XMVectorAdd(GetBasePosition(), DirectX::XMVectorSet(25, 0, 0, 1)));
				break;
			case 'Y':
				//LoadScene<VillageScene>(150 , 250);
				break;
			}
		}
	}
private:
	RipeGrain::SceneObject ninja , ninja2 , ninja3 , ninja4 , ninja5;
	MovementAnimator position_animator;
	RipeGrain::SpriteSheetAnimator walking_animator;
};

class VillageScene final : public RipeGrain::Scene
{
public:
	VillageScene(CoreEngine& engine, RipeGrain::SceneLoader& scene_loader) : VillageScene(engine, scene_loader, 150 , 150){}
	VillageScene(CoreEngine& engine, RipeGrain::SceneLoader& scene_loader , int x ,int y) : RipeGrain::Scene(engine, scene_loader)
	{
		//obj = AddObject<RipeGrain::SceneObject>();
		//obj->AddSprite(obj->CreateSprite(Image{"D:/SeqDownLogo.bmp"}));
		//obj->SetPosition(x, y);
	}
private:
	RipeGrain::SceneObject* obj;
};