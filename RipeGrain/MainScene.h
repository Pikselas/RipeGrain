#pragma once
#include "Audio.h"
#include "UILayer.h"
#include "BoxCollider.h"
#include "SceneManager.h"
#include "MapLoader.h"

/////////////////
class Snight;
class StartScene;
/////////////////

class MainScene final: public RipeGrain::Scene
{
public:
	void Initialize() override
	{

		LoadScene<StartScene>();
		//LoadScene<Snight>();
		return;
	}
	void Update() override
	{
		Scene::Update();
	}
	void OnEventReceive(RipeGrain::Event& ev) override
	{
	}
};

class StartScene : public RipeGrain::Scene
{
	RipeGrain::SceneObject poster;
	RipeGrain::UILayer interface_layer;
	RipeGrain::ObjectsLayer objects_layer;
private:
	void Initialize() override
	{
		auto s = CreateSprite(Image{ "sneo_files/poster_2.png" });
		poster.AddSprite(s);
		poster.SetPosition(s.GetWidth() / 2 , s.GetHeight() / 2);
		objects_layer.AddObject(&poster);
		AddLayer(&objects_layer);
		SetViewPortSize(718, 432);
		AddLayer(&interface_layer);

		FontFamily cake_cafe("sneo_files/fonts/Cakecafe.ttf");

		Font cake_cafe_font_10(cake_cafe, 10);

		Image parent_frame_img(150, 100);
		parent_frame_img.Clear({ .b = 26 , .g = 24 , .r = 3 , .a = 100 });

		Image child_frame_img(130, 80);
		child_frame_img.Clear({ .b = 25 , .g = 7 , .r = 9 , .a = 125 });

		Image button_img(100, 50);
		button_img.Clear({ .b = 120 , .g = 255 , .r = 217 , .a = 200 });

		RipeGrain::SetTextAtMiddle(button_img, {}, L"START GAME", cake_cafe_font_10);

		auto top_frame = interface_layer.AddUIFrame(RipeGrain::UIComponentDescription
			{
				.position_x = 50,
				.position_y = 50,
				.ui_sprite = CreateSprite(parent_frame_img)
			});

		auto child_frame = top_frame->AddComponent(RipeGrain::UIComponentDescription
			{
				.position_x = 10,
				.position_y = 10,
				.ui_sprite = CreateSprite(child_frame_img)
			});

		auto strt_btn = child_frame->AddComponent(RipeGrain::UIComponentDescription
			{
				.position_x = 20,
				.position_y = 20,
				.ui_sprite = CreateSprite(button_img)
			});

		strt_btn->on_mouse = [this](RipeGrain::EventMouseInput ev) 
			{
				if (ev.type == RipeGrain::EventMouseInput::Type::LeftPress)
				{
					LoadScene<Snight>();
				}
			};

		RegisterEvent(RipeGrain::CreateUIRegisterEvent(&interface_layer));
	}
	void OnEventReceive(RipeGrain::Event& ev) override
	{
		if (ev.event_type_index == typeid(RipeGrain::EventKeyBoardInput))
		{
			switch (RipeGrain::GetEventData<RipeGrain::EventKeyBoardInput>(ev).key_code)
			{
			case VK_SPACE:
				LoadScene<Snight>();
				break;
			}
		}
	}
};

class Sneo final : public RipeGrain::SceneObject
{
private:
	Texture front_, side_,back_;
private:
	DirectX::XMVECTOR direction;
private:
	std::unique_ptr<RipeGrain::SpriteSheetAnimator> front_animator;
public:
	void Init(ResourceEngine& engine)
	{
		front_ = engine.CreateTexture(Image{"sneo_files/character/77x189_43.png"});
		side_ = engine.CreateTexture(Image{"sneo_files/character/77x189_43_side.png"});
		back_ = engine.CreateTexture(Image{"sneo_files/character/77x189_43_back.png"});

		AddSprite(engine.CreateSprite(front_, 77, 189));
		front_animator = std::make_unique<RipeGrain::SpriteSheetAnimator>([]() {
			std::vector<std::pair<unsigned int, unsigned int>> p;
			for (int i = 0; i < 43; ++i)
				p.emplace_back(i * 77, 0); 
			return p;
			}(), 500);
	}
public:
	void PlayWalkAnimation()
	{
		front_animator->Animate(GetSprites().front());
	}
public:
	void SetDirectionBack()
	{
		direction = DirectX::XMVectorSet(0, 1, 1, 0);
		GetSprites().front().SetTexture(front_);
		GetSprites().front().SetTransformation(DirectX::XMMatrixIdentity());
	}
	void SetDirectionLeft()
	{
		direction = DirectX::XMVectorSet(-1, 0, 0, 0);
		GetSprites().front().SetTexture(side_);
		GetSprites().front().SetTransformation(DirectX::XMMatrixIdentity());
	}
	void SetDirectionRight()
	{
		direction = DirectX::XMVectorSet(1, 0, 0, 0);
		GetSprites().front().SetTexture(side_);
		GetSprites().front().SetTransformation(DirectX::XMMatrixRotationY(DirectX::XM_PI));
	}
	void SetDirectionFront()
	{
		direction = DirectX::XMVectorSet(0, -1, -1, 0);
		GetSprites().front().SetTexture(back_);
		GetSprites().front().SetTransformation(DirectX::XMMatrixIdentity());
	}
public:
	DirectX::XMVECTOR getDirection() const
	{
		return direction;
	}
};
class WindSprite
{
private:
	ImageSprite sprite;
	std::vector<Texture> texture_batch;
public:
	void Load(ResourceEngine& engine)
	{
		for (int i = 0; i <= 14; ++i)
		{
			texture_batch.push_back(engine.CreateTexture(Image{ "sneo_files/effects/wind." + std::to_string(i) + ".png" }));
		}
		auto& t = texture_batch.front();
		sprite = engine.CreateSprite(t, t.GetWidth(), t.GetHeight());
	}
	const std::vector<Texture>& GetBatch() const
	{
		return texture_batch;
	}
	ImageSprite GetSprite() const
	{
		return sprite;
	}
};
class WindEffect final : public RipeGrain::SceneObject
{
private:
	RipeGrain::MovementAnimator positon_animator;
	std::unique_ptr<RipeGrain::TextureBatchAnimator> animator;
	std::function<void()> callback;
public:
	WindEffect(const WindSprite& w_sprite, DirectX::XMVECTOR s, DirectX::XMVECTOR d)
	{	
		animator = std::make_unique<RipeGrain::TextureBatchAnimator>(w_sprite.GetBatch(), 1000);
		AddSprite(w_sprite.GetSprite());
		positon_animator.SetDestination(s, d, 0.007);
	}
	void SetExpireCallback(std::function<void()> cb)
	{
		callback = cb;
	}
public:
	void Update() override
	{
		if (!positon_animator.ReachedDestination())
		{
			animator->Animate(GetSprites().front());
			positon_animator.Update(this);
		}
		else
		{
			callback();
		}
	}
};

class ChestSprite
{
	friend class Chest;
private:
	Texture open_;
	Texture close_;
	ImageSprite sprite_;
public:
	void Load(ResourceEngine& engine)
	{
		open_ = engine.CreateTexture(Image{R"(C:\Users\Aritra Maji\Downloads\chest_open.png)"});
		close_ = engine.CreateTexture(Image{ R"(C:\Users\Aritra Maji\Downloads\chest_closed.png)"});
		sprite_ = engine.CreateSprite(close_ , close_.GetWidth() , close_.GetHeight());
	}
};
class Chest : public RipeGrain::SceneObject
{
private:
	ChestSprite tex;
public:
	Chest(ChestSprite tex) : tex(tex) 
	{
		AddSprite(tex.sprite_);
	}
public:
	void Open()
	{
		GetSprites()[0].SetTexture(tex.open_);
	}
	void Close()
	{
		GetSprites()[0].SetTexture(tex.close_);
	}
};

class Snight final : public RipeGrain::Scene
{
private:
	Sneo sneo;
	Map scene_map;
private:
	WindSprite wind_sprite;
	ChestSprite chest_sprite;
private:
	RipeGrain::UILayer interface_layer;
	RipeGrain::ObjectsLayer objects_layer;
private:
	std::list<std::pair<Chest , RipeGrain::UIComponent::UIPtr>> chests;
	std::list<WindEffect> wind_effects;
private:
	RipeGrain::MovementAnimator position_animator;
private:
	std::unique_ptr<RipeGrain::Audio> storm_sound;
	std::unique_ptr<RipeGrain::Audio> walking_sound;
	std::unique_ptr<RipeGrain::Audio> back_ground_audio;
private:
	RipeGrain::PlayBackHandle walking_sound_handle;
private:
	void handleKeyBoard(unsigned char key_code)
	{
		constexpr int dist = 25;
		if (key_code == 'A')
		{
			sneo.SetDirectionLeft();
			if (position_animator.ReachedDestination()) position_animator.SetDestination(sneo.GetPosition(), DirectX::XMVectorSetX(sneo.GetPosition(), sneo.GetX() - dist));
		}
		else if (key_code == 'D')
		{
			sneo.SetDirectionRight();
			if (position_animator.ReachedDestination()) position_animator.SetDestination(sneo.GetPosition(), DirectX::XMVectorSetX(sneo.GetPosition(), sneo.GetX() + dist));
		}
		else if (key_code == 'S')
		{
			sneo.SetDirectionBack();
			if (position_animator.ReachedDestination()) position_animator.SetDestination(sneo.GetPosition(), DirectX::XMVectorSetY(sneo.GetPosition(), sneo.GetY() + dist));
		}
		else if (key_code == 'W')
		{
			sneo.SetDirectionFront();
			if (position_animator.ReachedDestination()) position_animator.SetDestination(sneo.GetPosition(), DirectX::XMVectorSetY(sneo.GetPosition(), sneo.GetY() - dist));

		}
		else if (key_code == VK_SPACE)
		{
			auto dir = sneo.getDirection();
			dir = DirectX::XMVectorMultiply(dir, DirectX::XMVectorSet(500, 500, 500, 0));

			auto pos = sneo.GetPosition();
			pos = DirectX::XMVectorSetY(pos, sneo.GetY() - 125);
			wind_effects.emplace_back(wind_sprite, pos, DirectX::XMVectorAdd(pos, dir));
			auto itr = std::prev(wind_effects.end());
			itr->SetExpireCallback([itr, this]()
				{
					objects_layer.RemoveObject(&(*itr));
					wind_effects.erase(itr);
				});
			objects_layer.AddObject(&(*itr));
			RegisterEvent(RipeGrain::CreatePlayBackEvent(*storm_sound, true, nullptr, 1.5f));
		}
	}
public:
	void Initialize() override
	{
		back_ground_audio = std::make_unique<RipeGrain::Audio>("sneo_files/audio/Cinematic Fairy Tale.wav");
		//back_ground_audio->SetLoop();
		//back_ground_audio->Play();
		walking_sound = std::make_unique<RipeGrain::Audio>("sneo_files/audio/steps-in-corridor-104456.wav");
		storm_sound = std::make_unique<RipeGrain::Audio>("sneo_files/audio/atlantic-storm-58999.wav");
		scene_map.Open(getCoreEngine(), "sneo_files/map/training_center.mapig", "sneo_files/map/training_center.png");
		for (auto& c : scene_map.GetMapComponents())
		{
			objects_layer.AddObject(&c);
		}

		sneo.Init(getCoreEngine());
		sneo.SetDirectionFront();
		sneo.SetPosition(350, 350);
		objects_layer.AddObject(&sneo);

		using ColliderT = RipeGrain::BoxCollider;
		using TCollider = RipeGrain::EventCollidersAdded;

		std::vector<RipeGrain::BoxCollider> colliders;
		colliders.emplace_back(&sneo, 77 / 2, 77 / 2, 0, 189 / 2);

		for (auto c : scene_map.GetMapColliders())
		{
			colliders.push_back(c);
		}

		RegisterEvent(RipeGrain::CreatePlayBackEvent(*back_ground_audio));
		RegisterEvent(RipeGrain::CreatePlayBackEvent(*walking_sound, false, &walking_sound_handle, 1.5f));

		RegisterEvent(
				RipeGrain::CreateEventObject<TCollider>(
					TCollider
					{
						colliders
					}));

		wind_sprite.Load(getCoreEngine());
		SetViewPortSize(1100, 750);

		interface_layer.ui_clip_buffer = getCoreEngine().CreateStencilBuffer(1100, 750);

		Image parent_frame_img(200, 150);
		parent_frame_img.Clear({ .b = 26 , .g = 24 , .r = 3 , .a = 100 });

		Image button_img_open(150, 50);
		button_img_open.Clear({ .b = 120 , .g = 255 , .r = 217 , .a = 200 });
		Image button_img_close(150, 50);
		button_img_close.Clear({ .b = 120 , .g = 255 , .r = 217 , .a = 200 });

		FontFamily cake_cafe(R"(sneo_files\fonts\Cakecafe.ttf)");
		
		Font cake_cafe_font_14(cake_cafe, 14);
		Font cake_cafe_font_12(cake_cafe, 12);

		RipeGrain::SetTextAtMiddle(button_img_open, {}, L"OPEN", cake_cafe_font_12);
		RipeGrain::SetTextAtMiddle(button_img_close, {}, L"CLOSE", cake_cafe_font_12);
		RipeGrain::SetTextMiddleHorizontally(parent_frame_img, {}, L"CHOOSE ACTION", cake_cafe_font_14, 10);

		auto top_frame = interface_layer.AddUIFrame(RipeGrain::UIComponentDescription
			{
				.position_x = 50,
				.position_y = 350,
				.ui_sprite = CreateSprite(parent_frame_img)
			});
		auto open_btn = top_frame->AddComponent(RipeGrain::UIComponentDescription
			{
				.position_x = 30,
				.position_y = 30,
				.ui_sprite = CreateSprite(button_img_open)
			});
		auto close_btn = top_frame->AddComponent(RipeGrain::UIComponentDescription
			{
				.position_x = 30,
				.position_y = 85,
				.ui_sprite = CreateSprite(button_img_close)
			});

		top_frame->Hidden = true;

		chest_sprite.Load(getCoreEngine());
		chests.emplace_back(chest_sprite , top_frame);
		
		chests.back().first.SetPosition(470, 525, 525 + 40);


		for (auto& chest : chests)
		{
			objects_layer.AddObject(&chest.first);
		}

		open_btn->on_mouse = [this](RipeGrain::EventMouseInput ev) 
			{
				if (ev.type == RipeGrain::EventMouseInput::Type::LeftPress)
				{
					chests.back().first.Open();
				}
			};
		close_btn->on_mouse = [this](RipeGrain::EventMouseInput ev)
			{
				if (ev.type == RipeGrain::EventMouseInput::Type::LeftPress)
				{
					chests.back().first.Close();
				}
			};

		AddLayer(&objects_layer);
		AddLayer(&interface_layer);

		RegisterEvent(RipeGrain::CreateUIRegisterEvent(&interface_layer));
	}
	void Update() override
	{
		objects_layer.sortSceneObjectsByZ();

		if (!position_animator.ReachedDestination())
		{
			if (walking_sound_handle.IsStopped())
			{
				walking_sound_handle.Play(*walking_sound);
			}
			sneo.PlayWalkAnimation();
			position_animator.Update(&sneo);
			sneo.SetZ(sneo.GetY() + 189 / 2);
			auto window_edge_dist = GetWindowEdgeDistance(objects_layer.GetBasePosition());

			if (sneo.GetY() >= DirectX::XMVectorGetY(window_edge_dist))
			{
				position_animator.Cancel();
				sneo.SetY(DirectX::XMVectorGetY(window_edge_dist));
			}
			if (sneo.GetX() >= DirectX::XMVectorGetX(window_edge_dist))
			{
				position_animator.Cancel();
				sneo.SetX(DirectX::XMVectorGetX(window_edge_dist));
			}
		}
		else if (!walking_sound_handle.IsStopped())
		{
			walking_sound_handle.Stop();
		}

		for (auto& chst : chests)
		{
			auto ch_pos = DirectX::XMVectorAdd(objects_layer.GetBasePosition() , chst.first.GetPosition());
			ch_pos = DirectX::XMVectorAdd(ch_pos, DirectX::XMVectorSet(25 , 25 , 0 , 0));
			chst.second->SetPosition(ch_pos);

			auto sub = DirectX::XMVectorSubtract(sneo.GetPosition(), chst.first.GetPosition());
			sub = DirectX::XMVectorSetZ(sub, 0);
			int dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(sub));
			if (dist <= 100)
			{
				chst.second->Hidden = false;
			}
			else
			{
				chst.second->Hidden = true;
			}
		}

		const auto width_max = 1200 - GetViewPortWidth();
		const auto height_max = 1200 - GetViewPortHeight();

		const auto max_scene = DirectX::XMVectorSet(width_max, height_max, 0, 1);

		auto edge_dist_y = DirectX::XMVectorGetY(GetWindowEdgeDistance(objects_layer.GetBasePosition()));
		if (sneo.GetY() > edge_dist_y - 200)
		{
			auto d = std::abs(edge_dist_y - 200 - sneo.GetY());
			//SetBasePosition(DirectX::XMVectorAdd(GetBasePosition(), DirectX::XMVectorSet(0, -d, 0, 0)));
			objects_layer.Scroll(DirectX::XMVectorSet(0, -d, 0, 0), max_scene);
		}
		else if (sneo.GetY() < edge_dist_y - 500)
		{
			auto d = std::abs(edge_dist_y - 500 - sneo.GetY());
			objects_layer.Scroll(DirectX::XMVectorSet(0, d, 0, 0), max_scene);
		}
		auto edge_dist_x = DirectX::XMVectorGetX(GetWindowEdgeDistance(objects_layer.GetBasePosition()));
		if (sneo.GetX() > edge_dist_x - 200)
		{
			auto d = std::abs(edge_dist_x - 200 - sneo.GetX());
			//SetBasePosition(DirectX::XMVectorAdd(GetBasePosition(), DirectX::XMVectorSet(0, -d, 0, 0)));
			objects_layer.Scroll(DirectX::XMVectorSet(-d, 0, 0, 0), max_scene);
		}
		else if (sneo.GetX() < edge_dist_x - 500)
		{
			auto d = std::abs(edge_dist_x - 500 - sneo.GetX());
			objects_layer.Scroll(DirectX::XMVectorSet(d, 0, 0, 0), max_scene);
		}

		RipeGrain::Scene::Update();
	}
	void OnEventReceive(RipeGrain::Event& ev) override
	{
		if (ev.event_type_index == typeid(RipeGrain::EventKeyBoardInput))
		{
			auto ev_ = RipeGrain::GetEventData<RipeGrain::EventKeyBoardInput>(ev);
			handleKeyBoard(ev_.key_code);
		}
		else if (ev.event_type_index == typeid(RipeGrain::EventCollisionDetected))
		{
			auto data = RipeGrain::GetEventData<RipeGrain::EventCollisionDetected>(ev);
			for (auto& [c, lst] : data.collision_list)
			{
				if (dynamic_cast<Sneo*>(c.GetInlineObject()) != nullptr)
				{
					position_animator.Cancel(&sneo, c, lst.front());
				}
			}
		}

		RipeGrain::Scene::OnEventReceive(ev);
	};
};