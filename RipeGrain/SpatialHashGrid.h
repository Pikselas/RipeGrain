#pragma once
#include "BoxCollider.h"
#include <unordered_set>
#include <unordered_map>

namespace RipeGrain
{
	struct BoxColliderPair
	{
		BoxCollider static_collider;
		BoxCollider ref_collider;
		std::size_t operator()(const BoxColliderPair& cp) const
		{
			auto& b = cp.static_collider;
			return std::hash<int>()(b.GetLeft()) ^ std::hash<int>()(b.GetRight()) ^ std::hash<int>()(b.GetTop()) ^ std::hash<int>()(b.GetBottom());
		}
		bool operator==(const BoxColliderPair& other) const
		{
			return ref_collider == other.ref_collider /* ? true : collider.GetLeft() == other.collider.GetLeft() &&
				collider.GetRight() == other.collider.GetRight() && 
				collider.GetTop() == other.collider.GetTop() && 
				collider.GetBottom() == other.collider.GetBottom()*/;
		}
	};

	class SpatialHashGrid
	{
	private:
		unsigned int cell_side;
		std::unordered_map<std::string, std::unordered_set<BoxColliderPair , BoxColliderPair>> cells;
	public:
		SpatialHashGrid(unsigned int cell_side) : cell_side(cell_side)
		{}
	private:
		std::pair<int, int> getCellIndex(int x, int y) const
		{
			return { x / cell_side , y / cell_side };
		}
		std::string getKey(int x, int y) const
		{
			return std::to_string(x) + "." + std::to_string(y);
		}
	private:
		void forEachCellKey(BoxCollider collider , std::function<void(std::string key)> f) const
		{
			auto [x1, y1] = getCellIndex(collider.GetLeft(), collider.GetTop());
			auto [x2, y2] = getCellIndex(collider.GetRight(), collider.GetBottom());

			for (int x = x1; x <= x2; ++x)
			{
				for (int y = y1; y <= y2; ++y)
				{
					f(getKey(x, y));
				}
			}
		}
	public:
		void Insert(BoxCollider static_collider , BoxCollider dynamic_collider)
		{
			BoxColliderPair c{ .static_collider = static_collider , .ref_collider = dynamic_collider };
			forEachCellKey(static_collider, [this , c](std::string key) 
				{
					cells[key].insert(c);
				});
		}
		void Remove(BoxCollider static_collider, BoxCollider dynamic_collider)
		{
			BoxColliderPair c{ .static_collider = static_collider , .ref_collider = dynamic_collider }; 
			forEachCellKey(static_collider, [this, c](std::string key)
				{
					cells[key].erase(c);
				});
		}
		void Clear()
		{
			cells.clear();
		}
		std::vector<BoxCollider> FindNear(BoxCollider static_collider) const
		{
			std::vector <BoxCollider> colliders;
			forEachCellKey(static_collider, [&,this](std::string key) 
				{
					if (cells.contains(key))
					{
						for (auto& b : cells.at(key))
						{
							if(b.static_collider != static_collider)
							 colliders.push_back(b.ref_collider);
						}
					}
				});
			return colliders;
		}
	};
}