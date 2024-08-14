#pragma once
#include <vector>
#include "RepulsiveEngine/CoreEngine.h"
namespace RipeGrain
{
	class SceneObject
	{
	private:
		CoreEngine& sprite_engine;
	protected:
		DirectX::XMVECTOR position;
	protected:
		std::vector<ImageSprite> sprites;
	public:
		SceneObject(CoreEngine& sprite_engine) : sprite_engine(sprite_engine) , position(DirectX::XMVectorZero()){}
		virtual ~SceneObject() = default;
	public:
		inline ImageSprite CreateSprite(const Image& img)
		{
			return sprite_engine.CreateSprite(img);
		}
		inline ImageSprite CreateSprite(Texture texture)
		{
			return sprite_engine.CreateSprite(texture, texture.GetWidth(), texture.GetHeight());
		}
		inline ImageSprite CreateSprite(Texture texture, unsigned int width, unsigned int height)
		{
			return sprite_engine.CreateSprite(texture, width, height);
		}
	public:
		inline Texture CreateTexture(const Image& img)
		{
			return sprite_engine.CreateTexture(img);
		}
	public:
		inline void AddSprite(ImageSprite sprite)
		{
			sprites.emplace_back(sprite);
		}
	public:
		std::vector<ImageSprite>& GetSprites()
		{
			return sprites;
		}
	public:
		virtual void Update() {}
	public:
		inline void SetX(int x)
		{
			position = DirectX::XMVectorSetX(position, x);
		}
		inline void SetY(int y)
		{
			position = DirectX::XMVectorSetY(position, y);
		}
		inline void SetZ(int z)
		{
			position = DirectX::XMVectorSetZ(position, z);
		}
		inline void SetPosition(int x, int y)
		{
			SetPosition(x, y, GetZ());
		}
		inline void SetPosition(int x , int y , int z)
		{
			position = DirectX::XMVectorSet(x, y, z, 1);
		}
		inline void SetPosition(DirectX::XMVECTOR position)
		{
			this->position = position;
		}
	public:
		inline int GetX() const
		{
			return DirectX::XMVectorGetX(position);
		}
		inline int GetY() const
		{
			return DirectX::XMVectorGetY(position);
		}
		inline int GetZ() const
		{
			return DirectX::XMVectorGetZ(position);
		}
		inline std::pair<int , int> GetXY() const
		{
			return { GetX() , GetY() };
		}
		inline DirectX::XMVECTOR GetPosition() const
		{
			return position;
		}
	};
}