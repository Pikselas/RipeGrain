#pragma once
#include <list>
#include "SceneLayer.h"
#include "SceneObject.h"
#include "ObjectAnimator.h"
#include "EngineServices.h"
#include "RepulsiveEngine/ResourceEngine.h"

namespace RipeGrain
{
	class Scene
	{
	private:
		EngineProxyServiceLocator* engine_proxy = nullptr;
	private:
		std::function<void(EngineEventObject)> event_raiser;
	private:
		std::vector<SceneLayer*> layers;
	public:
		virtual ~Scene() = default;
	public:
		void SetSceneEngineProxyServiceLocator(EngineProxyServiceLocator* proxy)
		{
			engine_proxy = proxy;
		}
		void SetSceneEventRaiser(std::function<void(EngineEventObject)> event_raiser)
		{
			this->event_raiser = event_raiser;
		}
	protected:
		unsigned int GetViewPortWidth() const
		{
			return getProxyServiceLocator().QueryProxyService<RenderService>().GetViewPortWidth();
		}
		unsigned int GetViewPortHeight() const
		{
			return getProxyServiceLocator().QueryProxyService<RenderService>().GetViewPortHeight();
		}
	public:
		void AddLayer(SceneLayer* layer)
		{
			layers.emplace_back(layer);
		}
		void RemoveLayer(SceneLayer* layer)
		{
			layers.erase(std::find(layers.begin(), layers.end(), layer));
		}
		inline void RegisterEvent(EngineEventObject ev)
		{
			event_raiser(std::move(ev));
		}
		inline void SetViewPortSize(unsigned int width, unsigned int height)
		{
			getProxyServiceLocator().QueryProxyService<RenderService>().SetViewPortSize(width, height);
		}
	protected:
		template<typename T, typename... ParamsT>
		void LoadScene(ParamsT&& ... params)
		{
			getProxyServiceLocator().QueryProxyService<SceneService>().LoadScene<T>(std::forward<ParamsT>(params)...);
		}
	protected:
		Texture CreateTexture(const Image& img)
		{
			return getCoreEngine().CreateTexture(img);
		}
		ImageSprite CreateSprite(const Image& img)
		{
			return getCoreEngine().CreateSprite(img);
		}
		ImageSprite CreateSprite(Texture texture)
		{
			return getCoreEngine().CreateSprite(texture, texture.GetWidth(), texture.GetHeight());
		}
		ImageSprite CreateSprite(Texture texture, unsigned int width, unsigned int height)
		{
			return getCoreEngine().CreateSprite(texture, width, height);
		}
		ImageSprite CreateSprite(const Image& img, unsigned int width, unsigned int height)
		{
			return getCoreEngine().CreateSprite(CreateTexture(img), width, height);
		}
	protected:
		EngineProxyServiceLocator& getProxyServiceLocator() const
		{
			return *engine_proxy;
		}
	protected:
		ResourceEngine& getCoreEngine()
		{
			return getProxyServiceLocator().QueryProxyService<RenderService>().GetResourceEngine();
		}
	public:
		virtual void Update()
		{
			for (auto& layer : layers)
				layer->Update();
		}
		virtual void Initialize()
		{
		}
		virtual void OnEventReceive(Event& ev)
		{
		}
	protected:
		DirectX::XMVECTOR GetWindowEdgeDistance(DirectX::XMVECTOR base_position) const
		{
			return DirectX::XMVectorSubtract(DirectX::XMVectorSet(GetViewPortWidth(), GetViewPortHeight(), 0, 1), base_position);
		}
	public:
		void Render(RenderCommandEngine& engine) const
		{
			for (const auto& layer : layers)
			{
				layer->Render(engine);
			}
		}
	};
}