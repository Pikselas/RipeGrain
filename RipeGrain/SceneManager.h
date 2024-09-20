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
		CoreEngine* sprite_engine = nullptr;
		SceneLoader* scene_loader = nullptr;
	private:
		DirectX::XMVECTOR base_position;
	private:
		std::list<SceneObject*> objects;
	public:
		using SceneObjectRef = std::list<std::unique_ptr<SceneObject>>::iterator;
	public:
		Scene() : base_position(DirectX::XMVectorZero()) {}
		virtual ~Scene() = default;
	public:
		void ApplySceneArguments(CoreEngine& engine, SceneLoader& loader)
		{
			sprite_engine = &engine;
			scene_loader = &loader;
		}
	public:
		void AddObject(SceneObject* obj)
		{
			objects.emplace_back(obj);
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
	public:
		virtual void Update() 
		{
			for (auto& object : objects)
				object->Update();
		}
		virtual void Initialize()
		{

		}
		virtual void OnEventReceive(Event& ev)
		{}
	public:
		const DirectX::XMVECTOR& GetBasePosition() const
		{
			return base_position;
		}
		std::list<SceneObject*>& GetObjectList()
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