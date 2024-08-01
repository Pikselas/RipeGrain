#pragma once
#include <list>
#include "EngineComponent.h"
#include "ObjectAnimator.h"
#include "RepulsiveEngine/CoreEngine.h"

namespace RipeGrain
{
	
	class Scene
	{
	private:
		CoreEngine& sprite_engine;
	private:
		std::list<ImageSprite> sprites;
	private:
		std::list<std::unique_ptr<ObjectAnimator>> animators;
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
		void AddObjectAnimator(std::unique_ptr<ObjectAnimator> animator)
		{
			animators.emplace_back(std::move(animator));
		}
	public:
		std::list<ImageSprite>& GetSpriteList()
		{
			return sprites;
		}
		std::list<std::unique_ptr<ObjectAnimator>>& GetAnimators()
		{
			return animators;
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
	private:
		std::list<std::unique_ptr<ObjectAnimator>>* animators = nullptr;
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
			animators = &scene.GetAnimators();
			auto scene_event = std::make_unique<EventObject<EventSceneLoaded>>(CreateEventObject(EventSceneLoaded{&scene.GetSpriteList()}));
			RaiseEvent(std::move(scene_event));
		}
	public:
		void OnUpdate() override
		{
			if (animators)
				for (auto& animator : *animators)
					animator->Animate();
		}
	};

}