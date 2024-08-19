#pragma once
#include "SceneObject.h"
namespace RipeGrain
{
	class BoxCollider
	{
	private:
		int top;
		int left;
		int right;
		int bottom;
	private:
		SceneObject* object;
	public:
		BoxCollider(SceneObject* obj, int left, int right, int top, int bottom) : object(obj), left(left), right(right), top(top), bottom(bottom) {}
	public:
		inline int GetLeft() const
		{
			return object->GetX() - left;
		}
		inline int GetRight() const
		{
			return object->GetX() + right;
		}
		inline int GetTop() const
		{
			return object->GetY() - top;
		}
		inline int GetBottom() const
		{
			return object->GetY() + bottom;
		}
	public:
		inline DirectX::XMVECTOR GetPosition() const
		{
			return object->GetPosition();
		}
	public:
		bool TestCollision(BoxCollider other) const
		{
			return GetLeft() <= other.GetRight() && GetRight() >= other.GetLeft() && GetTop() <= other.GetBottom() && GetBottom() >= other.GetTop();
		}
	public:
		SceneObject* GetInlineObject() const
		{
			return object;
		}
	};
}