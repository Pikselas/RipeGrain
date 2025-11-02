#pragma once
#include <concepts>
#include <memory>
#include <functional>
#include <type_traits>

#include "Audio.h"
#include "Engine.h"
#include "TypeHelpers.h"
#include "ProxyComponent.h"
#include "../Crotine/TaskRunner.hpp"
#include "RepulsiveEngine/ResourceEngine.h"

namespace RipeGrain
{
	class RenderService
	{
	private:
		unsigned int viewport_width;
		unsigned int viewport_height;
	public:
		virtual float GetFrameDelta() const
		{
			return 0.06f;
		}
		virtual ResourceEngine& GetResourceEngine() const
		{
			throw std::runtime_error("not implemented by default service ");
		}
		virtual void SetViewPortSize(unsigned int w , unsigned int h)
		{
			viewport_width = w;
			viewport_height = h;
		}
	public:
		unsigned int GetViewPortWidth() const
		{
			return viewport_width;
		}
		unsigned int GetViewPortHeight() const
		{
			return viewport_height;
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
		 virtual void LoadSceneObject(std::unique_ptr<Scene, scene_deleter> scene) {}
	public:
		void LoadSceneObject(Scene* scene , scene_deleter deleter)
		{
			LoadSceneObject(std::unique_ptr<Scene, scene_deleter>(scene, deleter));
		}
		template<CScene T, typename... ParamsT>
		void LoadConstructedScene(ParamsT&& ... params, scene_deleter deleter = [](Scene* scene) { delete scene; })
		{
			LoadSceneObject(new T(std::forward<ParamsT>(params)...), deleter);
		}
		template<CScene T, typename... ParamT>
		void LoadScene(ParamT&& ... p)
		{
			if constexpr (sizeof...(ParamT) != 0)
			{
				if constexpr (std::is_same_v<Helpers::LastParamT<ParamT...>, scene_deleter>)
				{
					auto&& lp = Helpers::GetLastParam(std::forward<ParamT>(p)...);
					auto t2 = Helpers::strip_tuple<ParamT&&...>(std::forward_as_tuple(std::forward<ParamT>(p)...));

					std::apply([this,last_param = std::forward<decltype(lp)>(lp)](auto&&... p) mutable
						{
							LoadConstructedScene<T, decltype(p)...>(std::forward<decltype(p)>(p)..., std::forward<decltype(last_param)>(last_param));
						}, t2);
				}
			}
			else if constexpr (sizeof...(ParamT) == 1)
			{
				auto&& lp = Helpers::GetLastParam(std::forward<ParamT>(p)...);
				using last_p_t = decltype(lp);
				if constexpr (std::is_same_v<last_p_t, scene_deleter>)
				{
					LoadConstructedScene<T>(std::forward<scene_deleter>(p));
				}
				else
				{
					LoadConstructedScene<T, last_p_t>(std::forward<last_p_t>(lp));
				}
			}
			else
			{
				LoadConstructedScene<T, ParamT...>(std::forward<decltype(p)>(p)...);
			}
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
		ResourceEngine& GetResourceEngine() const
		{
			return service->GetResourceEngine();
		}
		void SetViewPortSize(unsigned int w, unsigned int h)
		{
			service->SetViewPortSize(w, h);
		}
		unsigned int GetViewPortWidth() const
		{
			return service->GetViewPortWidth();
		}
		unsigned int GetViewPortHeight() const
		{
			return service->GetViewPortHeight();
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
			service->LoadScene<T,ParamsT...>(std::forward<ParamsT>(params)...);
		}
	};
}

namespace RipeGrain
{
	template<typename U>
	struct BindProxyToService : public std::true_type
	{
		using proxy_type = U;
	};

	template <>
	struct ProxyComponent<AudioService> : public BindProxyToService<AudioServiceProxy> {};
	template <>
	struct ProxyComponent<SceneService> : public BindProxyToService<SceneServiceProxy> {};
	template <>
	struct ProxyComponent<RenderService> : public BindProxyToService<RenderServiceProxy> {};
	template <>
	struct ProxyComponent<ExecutionService> : public BindProxyToService<ExecutionServiceProxy> {};
}