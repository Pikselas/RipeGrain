#pragma once
#include <list>
#include <unordered_map>
#include "SceneObject.h"
#include "ObjectAnimator.h"
#include "EngineComponent.h"
#include "RepulsiveEngine/CoreEngine.h"

namespace RipeGrain
{
	class SceneLoader;

	class Scene
	{
	public:
		std::function<void(EventMouseInput)> OnMouseInput;
		std::function<void(EventKeyBoardInput)> OnKeyBoardInput;
	private:
		CoreEngine& sprite_engine;
	protected:
		SceneLoader& scene_loader;
	private:
		std::list<ImageSprite> sprites;
	private:
		std::unordered_map<std::string, SceneObject> object_id_list;
	private:
		std::list<std::unique_ptr<ObjectAnimator>> animators;
	public:
		Scene(CoreEngine& engine , SceneLoader& scene_loader) : sprite_engine(engine) , scene_loader(scene_loader) {}
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
		SceneObject AddSprite(std::string Id , auto&& ... sprite_data)
		{
			return (*(object_id_list.insert({ Id, AddSprite(sprite_data...) }).first)).second;
		}
	public:
		std::optional<SceneObject> GetObjectById(std::string Id) const
		{
			auto object = object_id_list.find(Id);
			if(object != object_id_list.end())
				return object->second;
			return std::nullopt;
		}
	public:
		void AddObjectAnimator(std::unique_ptr<ObjectAnimator> animator)
		{
			animators.emplace_back(std::move(animator));
		}
	public:
		virtual void Update()
		{
			for (auto& animator : animators)
				animator->Animate();
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
	private:
		CoreEngine& sprite_engine;
		std::unique_ptr<Scene> current_scene;
	public:
		SceneLoader(CoreEngine& sprite_engine) : sprite_engine(sprite_engine) {}
	public:
		template<typename T>
		void LoadScene()
		{
			current_scene = std::make_unique<T>(sprite_engine , *this);
			if (OnSceneLoaded)
				OnSceneLoaded(*current_scene);
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
			auto scene_event = std::make_unique<EventObject<EventSceneLoaded>>(CreateEventObject(EventSceneLoaded{&scene.GetSpriteList()}));
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