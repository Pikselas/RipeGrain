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
	};

	class Scene
	{
	public:
		std::function<void(EventMouseInput)> OnMouseInput;
		std::function<void(EventKeyBoardInput)> OnKeyBoardInput;
	private:
		CoreEngine& sprite_engine;
		SceneLoader& scene_loader;
	private:
		std::list<std::unique_ptr<SceneObject>> objects;
	public:
		using SceneObjectRef = std::list<std::unique_ptr<SceneObject>>::iterator;
	public:
		Scene(CoreEngine& engine , SceneLoader& scene_loader) : sprite_engine(engine) , scene_loader(scene_loader) {}
		virtual ~Scene() = default;
	public:
		template<CSceneObject SceneObjectT>
		SceneObjectT* AddObject()
		{
			return dynamic_cast<SceneObjectT*>(objects.emplace_back(std::make_unique<SceneObjectT>(sprite_engine)).get());
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
	public:
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
		}
	private:
		void onSceneLoad(Scene& scene)
		{
			current_scene = &scene;
			auto scene_event = std::make_unique<EventObject<EventSceneLoaded>>(CreateEventObject(EventSceneLoaded{&scene.GetObjectList()}));
			RaiseEvent(std::move(scene_event));
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
			if (ev.event_type_index == typeid(EventMouseInput))
			{
				if (current_scene && current_scene->OnMouseInput)
					current_scene->OnMouseInput(GetEventData<EventMouseInput>(ev));
			}
			else if (ev.event_type_index == typeid(EventKeyBoardInput))
			{
				if (current_scene && current_scene->OnKeyBoardInput)
					current_scene->OnKeyBoardInput(GetEventData<EventKeyBoardInput>(ev));
			}
		}
	};
}