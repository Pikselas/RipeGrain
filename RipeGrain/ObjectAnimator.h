#pragma once
#include <chrono>
#include <optional>
#include "SceneObject.h"
namespace RipeGrain
{
	class ObjectAnimator
	{
	private:
		float per_frame_duration;
	private:
		unsigned int total_frame_count;
	private:
		unsigned int current_frame_index = 0;
	protected:
		ImageSprite& sprite;
	private:
		std::optional<std::chrono::steady_clock::time_point> last_frame_time;
	public:
		ObjectAnimator(SceneObject object , unsigned int frame_count , float duration) :  sprite(*object.object_ref) , per_frame_duration(duration /(float)frame_count) , total_frame_count(frame_count) {}
	protected:
		virtual void SetObjectFrame(unsigned int frame_index) = 0;
	public:
		virtual ~ObjectAnimator() = default;
	public:
		void Animate()
		{
			if (!last_frame_time.has_value())
			{
				last_frame_time = std::chrono::steady_clock::now();
				return;
			}

			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - *last_frame_time).count();
			if (elapsed >= per_frame_duration)
			{
				auto diff = elapsed - per_frame_duration;
				current_frame_index += (1 + diff);
				if (current_frame_index >= total_frame_count)
				{
					SetObjectFrame(current_frame_index = 0);
					last_frame_time = std::nullopt;
					return;
				}
				SetObjectFrame(current_frame_index);
				last_frame_time = std::chrono::steady_clock::now();
			}
		}
	public:
		bool IsStopped()
		{
			return !(last_frame_time.has_value());
		}
	};

	class SpriteSheetAnimator : public ObjectAnimator
	{
	private:
		std::vector<std::pair<unsigned int, unsigned int>> sheet_offsets;
	public:
		SpriteSheetAnimator(SceneObject object , std::vector<std::pair<unsigned int, unsigned int>> sheet_offsets , float duration_ms) 
			:
		 ObjectAnimator(object , sheet_offsets.size() , duration_ms) , sheet_offsets(sheet_offsets)
		{}
	protected:
		void SetObjectFrame(unsigned int frame_index) override
		{
			auto [x, y] = sheet_offsets[frame_index];
			sprite.SetTextureCoord(x, y);
		}
	};

	class TextureBatchAnimator : public ObjectAnimator
	{
	private:
		std::vector<Texture> textures;
	public:
		TextureBatchAnimator(SceneObject object, std::vector<Texture> textures, float duration_ms)
			:
			ObjectAnimator(object, textures.size() , duration_ms) , textures(textures)
		{}
	protected:
		void SetObjectFrame(unsigned int frame_index) override
		{
			sprite.SetTexture(textures[frame_index]);
		}
	};

	class PositionAnimator : public ObjectAnimator
	{
	private:
		int velocity;
		int start_x;
		int end_x;
	public:
		PositionAnimator(SceneObject object, int start_x, int end_x, float duration_ms) 
			:
		 ObjectAnimator(object ,  std::abs(end_x - start_x) , duration_ms) , start_x(start_x) , end_x(end_x)
		{
			velocity = start_x < end_x ? 1 : -1;
		}
	protected:
		void SetObjectFrame(unsigned int index) override
		{
			int new_x = start_x + index * velocity;
			int old_y = DirectX::XMVectorGetY(sprite.GetPosition());
			int old_z = DirectX::XMVectorGetZ(sprite.GetPosition());
			sprite.SetPosition(DirectX::XMVectorSet(new_x, old_y, old_z , 1));
		}
	};
}