#pragma once
#include "point.h"
#include <cmath>
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
		
	};
}
