#pragma once
#include <list>
#include "SceneObject.h"
#include "ObjectAnimator.h"
#include "EngineComponent.h"
#include "RepulsiveEngine/CoreEngine.h"

namespace RipeGrain
{
	template<typename T>
	concept CSceneObject = std::is_base_of_v<SceneObject,T>;
	
	class Scene;

	class SceneLoader
	{
	public:
		std::function<void(Scene& , CoreEngine& engine)> OnSceneLoaded;
		std::function<void(std::unique_ptr<Event>)> SceneEventRaiser;
	private:
		CoreEngine& sprite_engine;
		std::unique_ptr<Scene> current_scene;
	public:
		SceneLoader(CoreEngine& sprite_engine) : sprite_engine(sprite_engine) {}
	public:
		template<typename T, typename... ParamsT>
		void LoadScene(ParamsT&& ... params)
		{
			current_scene = std::make_unique<T>(params...);
			if (OnSceneLoaded)
				OnSceneLoaded(*current_scene , sprite_engine);
		}
		void RegisterEvent(std::unique_ptr<Event> ev)
		{
			SceneEventRaiser(std::move(ev));
		}
	};

	class Scene
	{
	private:
		unsigned int view_port_width;
		unsigned int view_port_height;
	private:
		CoreEngine* sprite_engine = nullptr;
		SceneLoader* scene_loader = nullptr;
	private:
		DirectX::XMVECTOR base_position;
	private:
		std::vector<SceneObject*> objects;
	public:
		Scene() : base_position(DirectX::XMVectorZero()) {}
		virtual ~Scene() = default;
	public:
		void ApplySceneArguments(CoreEngine& engine, SceneLoader& loader)
		{
			sprite_engine = &engine;
			scene_loader = &loader;
		}
	protected:
		unsigned int GetViewPortWidth() const
		{
			return view_port_width;
		}
		unsigned int GetViewPortHeight() const
		{
			return view_port_height;
		}
	public:
		void AddObject(SceneObject* obj)
		{
			objects.emplace_back(obj);
		}
		void RemoveObject(SceneObject* obj)
		{
			objects.erase(std::find(objects.begin() , objects.end() , obj));
		}
		inline void RegisterEvent(std::unique_ptr<Event> ev)
		{
			scene_loader->RegisterEvent(std::move(ev));
		}
		inline void SetBasePosition(int x, int y)
		{
			base_position = DirectX::XMVectorSet(x, y, 0, 1);
		}
		inline void SetBasePosition(DirectX::XMVECTOR pos)
		{
			base_position = pos;
		}
		inline void SetViewPortSize(unsigned int width, unsigned int height)
		{
			view_port_width = width;
			view_port_height = height;
			RegisterEvent(std::make_unique<EventObject<EventResizeScreen>>(CreateEventObject(EventResizeScreen{.width = width , .height = height})));
		}
	protected:
		template<typename T , typename... ParamsT>
		void LoadScene(ParamsT&& ... params)
		{
			scene_loader->LoadScene<T>(params...);
		}
	protected:
		Texture CreateTexture(const Image& img)
		{
			return sprite_engine->CreateTexture(img);
		}
		ImageSprite CreateSprite(const Image& img)
		{
			return sprite_engine->CreateSprite(img);
		}
		ImageSprite CreateSprite(Texture texture)
		{
			return sprite_engine->CreateSprite(texture, texture.GetWidth(), texture.GetWidth());
		}
		ImageSprite CreateSprite(Texture texture , unsigned int width , unsigned int height)
		{
			return sprite_engine->CreateSprite(texture, width , height);
		}
		ImageSprite CreateSprite(const Image& img , unsigned int width , unsigned int height)
		{
			return sprite_engine->CreateSprite(CreateTexture(img) , width , height);
		}
	protected:
		CoreEngine& getCoreEngine()
		{
			return *sprite_engine;
		}
	public:
		virtual void Update() 
		{
			std::sort(objects.begin() , objects.end() , [](SceneObject* obj1, SceneObject* obj2) -> bool
			{
				return DirectX::XMVectorGetZ(obj1->GetPosition()) < DirectX::XMVectorGetZ(obj2->GetPosition());
			});

			for (auto& object : objects)
				object->Update();
		}
		virtual void Initialize()
		{}
		virtual void OnEventReceive(Event& ev)
		{}
	protected:
		DirectX::XMVECTOR GetWindowEdgeDistance() const
		{
			return DirectX::XMVectorSubtract(DirectX::XMVectorSet(view_port_width, view_port_height, 0, 1) , GetBasePosition());
		}
		void ScrollScene(DirectX::XMVECTOR dir, DirectX::XMVECTOR max_size)
		{
			auto pos = DirectX::XMVectorAdd(GetBasePosition(), dir);
			pos = DirectX::XMVectorClamp(pos, DirectX::XMVectorNegate(max_size), DirectX::XMVectorZero());
			SetBasePosition(pos);
		}
	public:
		const DirectX::XMVECTOR& GetBasePosition() const
		{
			return base_position;
		}
		const std::vector<SceneObject*>& GetObjectList() const
		{
			return objects;
		}
	};

	class SceneManager : public EngineEventRaiser , public EngineEventSubscriber
	{
	private:
		Scene* current_scene = nullptr;
	public:
		SceneManager(SceneLoader& scene_loader)
		{
			scene_loader.OnSceneLoaded = [&,this](Scene& scene , CoreEngine& engine)
				{
					scene.ApplySceneArguments(engine, scene_loader);
					onSceneLoad(scene);
				};
			scene_loader.SceneEventRaiser = [this](std::unique_ptr<Event> ev)
				{
					RaiseEvent(std::move(ev));
				};
		}
	private:
		void onSceneLoad(Scene& scene)
		{
			current_scene = &scene;
			auto scene_event = std::make_unique<EventObject<EventSceneLoaded>>(CreateEventObject(EventSceneLoaded{&scene.GetBasePosition() , &scene.GetObjectList()}));
			RaiseEvent(std::move(scene_event));

			scene.Initialize();
		}
	public:
		void OnUpdate() override
		{
			if (current_scene)
			{
				current_scene->Update();
			}
		}
		void OnEventReceive(Event& ev) override
		{
			if (current_scene)
				current_scene->OnEventReceive(ev);
		}
	};
}