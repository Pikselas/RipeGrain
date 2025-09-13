#pragma once
#include <concepts>
#include <memory>
#include <functional>
#include <type_traits>

#include "Audio.h"
#include "Engine.h"
#include "ProxyComponent.h"
#include "../Crotine/TaskRunner.hpp"

namespace RipeGrain
{
	class RenderService
	{
	public:
		virtual float GetFrameDelta() const
		{
			return 0.06f;
		}
	public:
		virtual ~RenderService() = default;
	public:
		static RenderService& DefaultInstance()
		{
			static RenderService instance;
			return instance;
		}
	};

	class ExecutionService
	{
		Crotine::TaskRunner runner;
	public:
		ExecutionService(Crotine::Executor& executor = Crotine::Executor::getDefaultExecutor()) : runner(executor) {}
	public:
		void Execute(auto&& f , auto&&... params)
		{
			runner.Run(f, params...).detach();
		}
	public:
		virtual ~ExecutionService() = default;
	public:
		static ExecutionService& DefaultInstance()
		{
			static ExecutionService instance;
			return instance;
		}
	};

	class AudioService
	{
		public:
			virtual ~AudioService() = default;
		public:
			virtual void GetPlayBackHandle(Audio& audio , PlayBackHandle& handle){}
		public:
			static AudioService& DefaultInstance()
			{
				static AudioService instance;
				return instance;
			}
	};

	class Scene;

	template<typename T>
	concept CScene = std::is_base_of_v<Scene, T>;

	class SceneService
	{
	public:
		using scene_deleter = std::function<void(Scene*)>;
	public:
		virtual ~SceneService() = default;
	public:
		static void default_scene_deleter(Scene* scene)
		{
			if(scene)
			delete scene;
		}
	public:
		 virtual void LoadSceneObject(std::unique_ptr<Scene, scene_deleter> scene) {}
	public:
		void LoadSceneObject(Scene* scene , scene_deleter deleter = default_scene_deleter)
		{
			LoadSceneObject(std::unique_ptr<Scene, scene_deleter>(scene, deleter));
		}
		template<CScene T, typename... ParamsT>
		void LoadScene(ParamsT&& ... params, scene_deleter deleter = default_scene_deleter)
		{
			LoadSceneObject(new T(std::forward<ParamsT>(params)...), deleter);
		}
	public:
		static SceneService& DefaultInstance()
		{
			static SceneService instance;
			return instance;
		}
	};

	class EngineServiceLocator
	{
	private:
		Engine& engine;
	public:
		EngineServiceLocator(Engine& eng) : engine(eng) {}
	public:
		template<typename T>
		T* QueryService()
		{
			return engine.QueryComponent<T>();
		}
	};

	class EngineProxyServiceLocator
	{
	private:
		Engine& engine;
	public:
		EngineProxyServiceLocator(Engine& eng) : engine(eng) {}
	public:
		template<ProxyAbleComponent T>
		auto QueryProxyService()
		{
			using proxy_t = typename ProxyComponent<T>::proxy_type;
			auto component = engine.QueryComponent<T>();
			return component ? proxy_t{ *component } : proxy_t{};
		}
	};
}

namespace RipeGrain
{

	class RenderServiceProxy
	{
	private:
		RenderService* service;
	public:
		RenderServiceProxy() : service(&RenderService::DefaultInstance()) {}
		RenderServiceProxy(RenderService& service) : service(&service) {}
	public:
		float GetFrameDelta() const
		{
			return service->GetFrameDelta();
		}
	};

	class ExecutionServiceProxy
	{
	private:
			ExecutionService* service;
	public:
		ExecutionServiceProxy() : service(&ExecutionService::DefaultInstance()) {}
		ExecutionServiceProxy(ExecutionService& service) : service(&service) {}
	public:
		void Execute(auto&& f , auto&&... params)
		{
			service->Execute(f , params...);
		}
	};

	class AudioServiceProxy
	{
	private:
		AudioService* service;
	public:
		AudioServiceProxy() : service(&AudioService::DefaultInstance()) {}
		AudioServiceProxy(AudioService& service) : service(&service) {}
	public:
		void GetPlayBackHandle(Audio& audio , PlayBackHandle& handle)
		{
			return service->GetPlayBackHandle(audio , handle);
		}
	};

	class SceneServiceProxy
	{
	private:
		SceneService* service;
	public:
		SceneServiceProxy() : service(&SceneService::DefaultInstance()) {}
		SceneServiceProxy(SceneService& service) : service(&service) {}
	public:
		template<typename T, typename... ParamsT>
		void LoadScene(ParamsT&& ... params)
		{
			service->LoadScene<T>(std::forward<ParamsT>(params)...);
		}
	};
}

namespace RipeGrain
{
	template <>
	class ProxyComponent<RenderService> : public std::true_type
	{
	public:
		using proxy_type = RenderServiceProxy;
	};

	template <>
	class ProxyComponent<ExecutionService> : public std::true_type
	{
		public:
		using proxy_type = ExecutionServiceProxy;
	};

	template <>
	class ProxyComponent<AudioService> : public std::true_type
	{
		public:
		using proxy_type = AudioServiceProxy;
	};

	template <>
	class ProxyComponent<SceneService> : public std::true_type
	{
	public:
		using proxy_type = SceneServiceProxy;
	};

}