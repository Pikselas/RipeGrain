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
		std::function<void(Scene&)> OnSceneLoaded;
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
			current_scene = std::make_unique<T>(sprite_engine, *this, params...);
			if (OnSceneLoaded)
				OnSceneLoaded(*current_scene);
		}
		void RegisterEvent(std::unique_ptr<Event> ev)
		{
			SceneEventRaiser(std::move(ev));
		}
	};

	class Scene
	{
	private:
		CoreEngine& sprite_engine;
		SceneLoader& scene_loader;
	private:
		DirectX::XMVECTOR base_position;
	private:
		std::list<std::unique_ptr<SceneObject>> objects;
	public:
		using SceneObjectRef = std::list<std::unique_ptr<SceneObject>>::iterator;
	public:
		Scene(CoreEngine& engine , SceneLoader& scene_loader) : sprite_engine(engine) , scene_loader(scene_loader) , base_position(DirectX::XMVectorZero()) {}
		virtual ~Scene() = default;
	public:
		template<CSceneObject SceneObjectT>
		SceneObjectT* AddObject()
		{
			return dynamic_cast<SceneObjectT*>(objects.emplace_back(std::make_unique<SceneObjectT>(sprite_engine)).get());
		}
		inline void RegisterEvent(std::unique_ptr<Event> ev)
		{
			scene_loader.RegisterEvent(std::move(ev));
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
			scene_loader.LoadScene<T>(params...);
		}
	public:
		virtual void Update() 
		{
			for (auto& object : objects)
				object->Update();
		}
		virtual void Initialize()
		{}
		virtual void OnEventReceive(Event& ev)
		{}
	public:
		const DirectX::XMVECTOR& GetBasePosition() const
		{
			return base_position;
		}
		std::list<std::unique_ptr<SceneObject>>& GetObjectList()
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
			scene_loader.OnSceneLoaded = [this](Scene& scene)
				{
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