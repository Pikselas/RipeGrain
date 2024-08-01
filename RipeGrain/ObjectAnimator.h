#pragma once
#include <chrono>
#include <optional>
#include "RepulsiveEngine/ImageSprite.h"
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
		ObjectAnimator(ImageSprite& sprite , unsigned int frame_count , float duration) :  sprite(sprite) , per_frame_duration(duration /(float)frame_count) , total_frame_count(frame_count) {}
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
				SetObjectFrame((current_frame_index += 1 + diff) % total_frame_count);
				last_frame_time = std::chrono::steady_clock::now();
			}
		}
	};

	class SpriteSheetAnimator : public ObjectAnimator
	{
	private:
		std::vector<std::pair<unsigned int, unsigned int>> sheet_offsets;
	public:
		SpriteSheetAnimator(ImageSprite& sprite , std::vector<std::pair<unsigned int, unsigned int>> sheet_offsets , float duration_ms) 
			:
		 ObjectAnimator(sprite , sheet_offsets.size() , duration_ms) , sheet_offsets(sheet_offsets)
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
		TextureBatchAnimator(ImageSprite& sprite, std::vector<Texture> textures, float duration_ms)
			:
			ObjectAnimator(sprite  , textures.size() , duration_ms) , textures(textures)
		{}
	protected:
		void SetObjectFrame(unsigned int frame_index) override
		{
			sprite.SetTexture(textures[frame_index]);
		}
	};
}