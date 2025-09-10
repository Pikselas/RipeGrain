#pragma once
#include "Audio.h"
#include "Engine.h"
#include "ProxyComponent.h"
#include "../Crotine/Xecutor.hpp"
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
	private:
		Crotine::Xecutor executor;
		Crotine::TaskRunner runner;
	public:
		ExecutionService() : runner(executor) {}
	public:
		void Execute(auto&& f , auto&&... params)
		{
			runner.Run(f, params...).detach();
		}
	public:
		~ExecutionService() = default;
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
}