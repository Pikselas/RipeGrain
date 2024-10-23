#pragma once
#include <list>
#include <functional>
#include "Event.h"
#include "RepulsiveEngine/ImageSprite.h"
#include "RepulsiveEngine/StencilBuffer.h"

namespace RipeGrain
{
	struct UIComponentDescription
	{
		int position_x;
		int position_y;
		ImageSprite ui_sprite;
	};

	class UIComponent
	{
		friend class UILayer;
	private:
		ImageSprite ui_sprite;
	public:
		using UIPtr = std::list<UIComponent>::iterator;
	private:
		UIPtr self;
		std::optional<UIPtr> parent;
		std::function<void(UIPtr)> remove_self;
	public:
		std::function<void(EventMouseInput)> on_mouse = nullptr;
	private:
		std::list<UIComponent> children;
	public:
		bool IsInRange(int x_pos, int y_pos)
		{
			int ui_x = GetX();
			int ui_y = GetY();

			int width = ui_sprite.GetWidth();
			int height = ui_sprite.GetHeight();

			return (x_pos >= ui_x && x_pos <= ui_x + width && y_pos >= ui_y && y_pos <= ui_y + height);
		}
		void OnEvent(EventMouseInput evnt)
		{
			if(on_mouse)
				on_mouse(evnt);
			for (auto& child : children)
			{
				if (child.IsInRange(evnt.x_pos , evnt.y_pos))
				{
					evnt.x_pos = evnt.x_pos - child.GetX();
					evnt.y_pos = evnt.y_pos - child.GetY();
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
	private:
		static UIComponent create_component(UIComponentDescription desc)
		{
			UIComponent comp(desc.ui_sprite);
			comp.SetPosition(desc.position_x, desc.position_y);
			return comp;
		}
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
			SetX(x);
			SetY(y);
		}
	public:
		void SetTexture(Texture tex)
		{
			ui_sprite.SetTexture(tex);
		}
	public:
		UIPtr AddComponent(UIComponent component)
		{
			auto& child = children.emplace_back(std::move(component));
			child.parent = self;
			child.remove_self = remove_self;
			return child.self = std::prev(children.end());
		}
		UIPtr AddComponent(UIComponentDescription desc)
		{
			return AddComponent(create_component(desc));
		}
	public:
		void Remove()
		{
			remove_self(self);
		}
		void RemoveComponent(UIPtr child)
		{
			child->Remove();
		}
		void Render(CoreEngine& engine, int parent_x, int parent_y , unsigned int stencil_ref) const
		{
			auto sprite = ui_sprite;

			auto half_size = DirectX::XMVectorDivide(DirectX::XMVectorSet(sprite.GetWidth(), sprite.GetHeight(), 1, 1), DirectX::XMVectorSet(2, 2, 1, 1));
			auto pos = DirectX::XMVectorAdd(sprite.GetPosition(), half_size);

			pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(parent_x, parent_y, 1, 1));
			sprite.SetPosition(pos);
			sprite.Draw(engine);
			for (const auto& child_ui : children)
			{
				engine.BeginStencilClipping(stencil_ref + 1);
				child_ui.Render(engine, DirectX::XMVectorGetX(ui_sprite.GetPosition()), DirectX::XMVectorGetY(ui_sprite.GetPosition()),stencil_ref + 1);
				engine.EndStencilClipping(stencil_ref + 1);
			}
		}
	};
}