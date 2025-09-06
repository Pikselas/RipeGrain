#pragma once
#include "Engine.h"
#include "ProxyComponent.h"
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
}

namespace RipeGrain
{
	template <>
	class ProxyComponent<RenderService> : public std::true_type
	{
	public:
		using proxy_type = RenderServiceProxy;
	};
}