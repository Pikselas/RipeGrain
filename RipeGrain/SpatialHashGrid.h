#pragma once
#include "BoxCollider.h"
#include <unordered_set>
#include <unordered_map>


namespace RipeGrain
{
	struct BoxColliderHasher
	{
		std::size_t operator()(const BoxCollider& b) const
		{
			return std::hash<int>()(b.GetLeft()) ^ std::hash<int>()(b.GetRight()) ^ std::hash<int>()(b.GetTop()) ^ std::hash<int>()(b.GetBottom());
		}
	};

	class SpatialHashGrid
	{
	private:
		unsigned int cell_side;
		std::unordered_map<std::string, std::unordered_set<BoxCollider, BoxColliderHasher>> cells;
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
		void Insert(BoxCollider collider)
		{
			forEachCellKey(collider, [this , collider](std::string key) 
				{
					cells[key].insert(collider);
				});
		}
		void Remove(BoxCollider collider)
		{
			forEachCellKey(collider, [this, collider](std::string key)
				{
					cells[key].erase(collider);
				});
		}
		void Clear()
		{
			cells.clear();
		}
		std::vector<BoxCollider> FindNear(BoxCollider collider) const
		{
			std::vector <BoxCollider> colliders;
			forEachCellKey(collider, [&,this](std::string key) 
				{
					if (cells.contains(key))
					{
						for (auto& b : cells.at(key))
						{
							if(b != collider)
							 colliders.push_back(b);
						}
					}
				});
			return colliders;
		}
	};
}