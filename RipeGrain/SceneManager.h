#pragma once
#include <list>
#include "Scene.h"
#include "EngineComponent.h"
#include "EngineServices.h"
#include "RepulsiveEngine/ResourceEngine.h"

namespace RipeGrain
{
	struct EventSceneLoaded
	{
		std::unique_ptr<Scene, std::function<void(Scene*)>> scene;
	};

	class SceneLoader : public EngineEventRaiser , public SceneService
	{
	public:
		using SceneService::LoadSceneObject;
		void LoadSceneObject(std::unique_ptr<Scene, scene_deleter> scene) override
		{
			RaiseEvent(CreateEventObject(EventSceneLoaded{ .scene = std::move(scene)}));
		}
	};

	class SceneManager : public EngineEventRaiser , public EngineEventSubscriber
	{
	private:
		EngineProxyServiceLocator& engine_proxy;
		std::unique_ptr<Scene, SceneService::scene_deleter> current_scene = nullptr;
	public:
		SceneManager(EngineProxyServiceLocator& engine_proxy) : engine_proxy(engine_proxy){}
		~SceneManager() = default;	
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
			if (ev.event_type_index == typeid(EventSceneLoaded))
			{
				current_scene = std::move(GetEventData<EventSceneLoaded>(ev).scene);
				current_scene->SetSceneEngineProxyServiceLocator(&engine_proxy);
				current_scene->SetSceneEventRaiser(std::bind_front(&SceneManager::RaiseEvent , this));
				
				current_scene->Initialize();
			}
			else if (current_scene)
			{
				current_scene->OnEventReceive(ev);
			}
		}
	};
}