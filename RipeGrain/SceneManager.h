#pragma once
#include <list>
#include "EngineComponent.h"
#include "RepulsiveEngine/CoreEngine.h"

namespace RipeGrain
{
	
	class Scene
	{
	private:
		CoreEngine& sprite_engine;
	private:
		std::list<ImageSprite> sprites;
	public:
		Scene(CoreEngine& engine) : sprite_engine(engine) {}
	public:
		ImageSprite& AddSprite(ImageSprite sprite)
		{
			return sprites.emplace_back(sprite);
		}
		ImageSprite& AddSprite(const Image& img)
		{
			return sprites.emplace_back(sprite_engine.CreateSprite(img));
		}
	public:
		std::list<ImageSprite>& GetSpriteList()
		{
			return sprites;
		}
	};

	class SceneLoader
	{
	public:
		std::function<void(Scene&)> OnSceneLoaded;
	public:
		void Load(Scene& scene)
		{
			if (OnSceneLoaded)
			{
				OnSceneLoaded(scene);
			}
		}
	};

	class SceneManager : public EngineEventRaiser
	{
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
			auto scene_event = std::make_unique<EventObject<EventSceneLoaded>>(CreateEventObject(EventSceneLoaded{&scene.GetSpriteList()}));
			RaiseEvent(std::move(scene_event));
		}
	};

}