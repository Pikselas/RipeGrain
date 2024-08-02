#pragma once
#include <list>
#include "SceneObject.h"
#include "ObjectAnimator.h"
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
	private:
		std::list<std::unique_ptr<ObjectAnimator>> animators;
	public:
		Scene(CoreEngine& engine) : sprite_engine(engine) {}
	public:
		SceneObject AddSprite(SceneObject object)
		{
			sprites.emplace_back(*object.object_ref);
			return  SceneObject{ std::prev(sprites.end()) , sprites};
		}
		SceneObject AddSprite(const Image& img)
		{
			sprites.emplace_back(sprite_engine.CreateSprite(img));
			return  SceneObject{ std::prev(sprites.end()) , sprites };
		}
		SceneObject AddSprite(const Image& img , unsigned int width , unsigned int height)
		{

			sprites.emplace_back(sprite_engine.CreateSprite(sprite_engine.CreateTexture(img), width, height));
			return  SceneObject{ std::prev(sprites.end()) , sprites };
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

	class SceneManager : public EngineEventRaiser , public EngineEventSubscriber
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
		void OnEventReceive(Event& ev) override
		{
			if (ev.event_type_index == typeid(EventMouseInput))
			{
				
			}
		}
	};
}