#pragma once
#include "SceneLayer.h"
#include "UIComponent.h"

namespace RipeGrain
{
	class UILayer final : public SceneLayer
	{
	private:
		std::vector<UIComponent> components;
	public:
		void AddUIFrame(UIComponent component)
		{
			components.emplace_back(component);
		}
	public:
		void Render(CoreEngine& engine) const override
		{
			for (const auto& ui_component : components)
			{
				ui_component.Render(engine, DirectX::XMVectorGetX(base_position), DirectX::XMVectorGetY(base_position));
			}
		}
	};
}