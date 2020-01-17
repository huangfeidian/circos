#pragma once
#include <cmath>
#include "../basics/point.h"
#include "../basics/color.h"
namespace spiritsaway::circos
{
	struct Rectangle
	{
		Point left;
		Point right;
		std::uint16_t height;
		Color color;
		float opacity;
		bool fill;
		Rectangle()
			:left(), right(), color(), opacity(1)
		{
		
		}
		Rectangle(Point in_left, Point in_right, Color in_color, std::uint16_t in_height, bool in_fill = false, float in_opacity =1.0)
			: left(in_left)
			, right(in_right)
			, color(in_color)
			, height(in_height)
			, fill(in_fill)
			, opacity(in_opacity)
		{
			
		}
	};
}
