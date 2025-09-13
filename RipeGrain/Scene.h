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
		unsigned int view_port_width;
		unsigned int view_port_height;
	private:
		ResourceEngine* sprite_engine = nullptr;
		EngineProxyServiceLocator* engine_proxy = nullptr;
	private:
		std::function<void(std::unique_ptr<Event>)> event_raiser;
	private:
		std::vector<SceneLayer*> layers;
	public:
		virtual ~Scene() = default;
	public:
		void SetSceneSpriteEngine(ResourceEngine* engine)
		{
			sprite_engine = engine;
		}
		void SetSceneEngineProxyServiceLocator(EngineProxyServiceLocator* proxy)
		{
			engine_proxy = proxy;
		}
		void SetSceneEventRaiser(std::function<void(std::unique_ptr<Event>)> event_raiser)
		{
			this->event_raiser = event_raiser;
		}
	protected:
		unsigned int GetViewPortWidth() const
		{
			return view_port_width;
		}
		unsigned int GetViewPortHeight() const
		{
			return view_port_height;
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
		inline void RegisterEvent(std::unique_ptr<Event> ev)
		{
			event_raiser(std::move(ev));
		}
		inline void SetViewPortSize(unsigned int width, unsigned int height)
		{
			view_port_width = width;
			view_port_height = height;
			RegisterEvent(CreateEventObject(EventResizeScreen{ .width = width , .height = height }));
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
			return sprite_engine->CreateTexture(img);
		}
		ImageSprite CreateSprite(const Image& img)
		{
			return sprite_engine->CreateSprite(img);
		}
		ImageSprite CreateSprite(Texture texture)
		{
			return sprite_engine->CreateSprite(texture, texture.GetWidth(), texture.GetHeight());
		}
		ImageSprite CreateSprite(Texture texture, unsigned int width, unsigned int height)
		{
			return sprite_engine->CreateSprite(texture, width, height);
		}
		ImageSprite CreateSprite(const Image& img, unsigned int width, unsigned int height)
		{
			return sprite_engine->CreateSprite(CreateTexture(img), width, height);
		}
	protected:
		EngineProxyServiceLocator& getProxyServiceLocator()
		{
			return *engine_proxy;
		}
	protected:
		ResourceEngine& getCoreEngine()
		{
			return *sprite_engine;
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
			return DirectX::XMVectorSubtract(DirectX::XMVectorSet(view_port_width, view_port_height, 0, 1), base_position);
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