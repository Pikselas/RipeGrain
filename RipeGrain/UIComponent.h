#pragma once
#include <vector>
#include <functional>
#include "Event.h"
#include "RepulsiveEngine/ImageSprite.h"
namespace RipeGrain
{
	class UIComponent
	{
	private:
		std::string id;
		ImageSprite ui_sprite;
	private:
		std::vector<UIComponent> children;
	public:
		void OnEvent(EventMouseInput evnt)
		{
			for (auto& child : children)
			{
				int ui_x = child.GetX();
				int ui_y = child.GetY();

				int width = child.ui_sprite.GetWidth();
				int height = child.ui_sprite.GetHeight();

				if (evnt.x_pos >= ui_x && evnt.x_pos <= ui_x + width && evnt.y_pos >= ui_y && evnt.y_pos <= ui_y + height)
				{
					evnt.x_pos = ui_x - evnt.x_pos;
					evnt.y_pos = ui_y - evnt.y_pos;
					child.OnEvent(evnt);
				}
			}
		}
		void OnEvent(CustomWindow::KeyBoard::EventT evnt)
		{

		}
	public:
		UIComponent() = default;
		UIComponent(ImageSprite ui_sprite) : ui_sprite(ui_sprite) {}
	public:
		unsigned int GetHeight() const
		{
			return ui_sprite.GetHeight();
		}
		unsigned int GetWidth() const
		{
			return ui_sprite.GetWidth();
		}
		int GetX() const
		{
			return DirectX::XMVectorGetX(ui_sprite.GetPosition());
		}
		int GetY() const
		{
			return DirectX::XMVectorGetY(ui_sprite.GetPosition());
		}
	public:
		void SetX(int x)
		{
			ui_sprite.SetPosition(DirectX::XMVectorSetX(ui_sprite.GetPosition(), x));
		}
		void SetY(int y)
		{
			ui_sprite.SetPosition(DirectX::XMVectorSetY(ui_sprite.GetPosition(), y));
		}
		void SetPosition(int x, int y)
		{
			ui_sprite.SetPosition(DirectX::XMVectorSet(x, y, 1, 1));
		}
	public:
		void SetTexture(Texture tex)
		{
			ui_sprite.SetTexture(tex);
		}
	public:
		void AddComponent(UIComponent component)
		{
			children.emplace_back(std::move(component));
		}
		void RemoveComponent(std::string component_id)
		{
			std::remove_if(children.begin(), children.end(), [component_id](auto& component) 
				{
					return component.id == component_id;
				});
		}
		void Render(CoreEngine& engine, int parent_x, int parent_y) const
		{
			auto sprite = ui_sprite;

			auto half_size = DirectX::XMVectorDivide(DirectX::XMVectorSet(sprite.GetWidth(), sprite.GetHeight(), 1, 1), DirectX::XMVectorSet(2, 2, 1, 1));
			auto pos = DirectX::XMVectorAdd(sprite.GetPosition(), half_size);

			pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(parent_x, parent_y, 1, 1));

			sprite.SetPosition(pos);

			//pos = DirectX::XMVectorDivide(DirectX::XMVectorSet(parent->get().GetWidth(), parent->get().GetHeight(), 1, 1), DirectX::XMVectorSet(2, 2, 1, 1));
			//pos = DirectX::XMVectorSubtract(ui_sprite.GetPosition(), pos);
			//sprite.SetTransformation(DirectX::XMMatrixTranslationFromVector(pos));

			sprite.Draw(engine);
			for (const auto& child_ui : children)
			{
				child_ui.Render(engine, DirectX::XMVectorGetX(ui_sprite.GetPosition()), DirectX::XMVectorGetY(ui_sprite.GetPosition()));
			}
		}
	};

	struct EventSetUIFrame
	{
		UIComponent component;
	};
}