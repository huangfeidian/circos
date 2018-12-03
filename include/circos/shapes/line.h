#pragma once

#include <cmath>
#include <utility>

#include "../basics/point.h"
#include "../basics/color.h"

namespace circos
{
	struct Line
	{
		Point from;
		Point to;
		int width;
		Color color;
		double opacity;
		Line() :from(), to(), color(), opacity(1), width(1)
		{
		
		}
		Line(const Point in_from, const Point in_to, Color in_color = Color(), int in_width = 1,double in_opacity= 1.0)
			:from(in_from), to(in_to), color(in_color), width(in_width),opacity(in_opacity)
		{
			
		}
		double len()
		{
			double diff_x = from.x - to.x;
			double diff_y = from.y - to.y;
			return std::sqrt(diff_x*diff_x + diff_y*diff_y);
		}
		Point symmetric_point(Point point) const
		// 获得对称点
		{
			std::pair<int, int> vec_1 = std::make_pair(to.x - from.x, to.y - from.y);
			std::pair<int, int> vec_2 = std::make_pair(point.x - from.x, point.y - from.y);
			float length = (vec_1.first*vec_2.first + vec_1.second*vec_2.second)/(1.0*(vec_1.first*vec_1.first+vec_1.second*vec_1.second));
			std::pair<int, int> remain = static_cast<std::pair<int,int>>(std::make_pair(vec_2.first - length*vec_1.first, vec_2.second - length*vec_1.second));
			std::pair<int, int> vec_3 = std::make_pair(vec_2.first - 2 * remain.first, vec_2.second - 2 * remain.second);
			return Point(vec_3.first + from.x, vec_3.second + from.y);
		}
	};
}
