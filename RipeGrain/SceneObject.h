#pragma once
#include <list>
#include "RepulsiveEngine/ImageSprite.h"
namespace RipeGrain
{
	class SceneObject
	{
		friend class Scene;
		friend class ObjectAnimator;
	private:
		std::list<ImageSprite>::iterator object_ref;
		std::reference_wrapper<std::list<ImageSprite>> objects;
	public:
		SceneObject(std::list<ImageSprite>::iterator object_ref , std::list<ImageSprite>& objects) : object_ref(object_ref) , objects(objects)
		{}
	public:
		void SetX(int x)
		{
			SetPosition(x, GetY());
		}
		void SetY(int y)
		{
			SetPosition(GetX(), y);
		}
		void SetZ(int z)
		{
			SetPosition(GetX(), GetY(), z);
		}
		void SetPosition(int x, int y)
		{
			SetPosition(x, y, GetZ());
		}
		void SetPosition(int x, int y, int z)
		{
			object_ref->SetPosition(DirectX::XMVectorSet(x, y, z , 1));
		}
	public:
		int GetX() const
		{
			return DirectX::XMVectorGetX(object_ref->GetPosition());
		}
		int GetY() const
		{
			return DirectX::XMVectorGetY(object_ref->GetPosition());
		}
		int GetZ() const
		{
			return DirectX::XMVectorGetZ(object_ref->GetPosition());
		}
		DirectX::XMVECTOR GetPosition() const
		{	
			return object_ref->GetPosition();
		}
	public:
		void Remove()
		{
			objects.get().erase(object_ref);
		}
	};
}