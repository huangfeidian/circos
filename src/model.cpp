#include <circos/model.h>

namespace circos::model
{
	void model::to_shapes(shape_collection& pre_collection)
	{
		// 1. 首先输出所有的圆环
		for(const auto& i: circles)
		{
			const auto& cur_circle = i.second;
			if(cur_circle.opacity <= 0.001)
			{
				continue;
			}
			auto cur_ring = Ring(config.center, cur_circle.inner_radius, cur_circle.outer_radius, 0, 360, cur_circle.fill_color, 1, true, cur_circle.opacity);
			pre_collection.rings.push_back(cur_ring);
		}
		// 然后计算每个圆环上每条带子的位置
		
	}
}