#pragma once

#include "../color.h"
#include "point.h"
using std::vector;
using std::string;
using std::stringstream;
#pragma once
namespace circos
{
	struct Circle
	{
		double radius;
		Point center;
		Color color;
		double opacity;
		bool filled;
		string_view id;
		Circle(string_view in_id, double in_radius, Point in_center, Color in_color , double in_opacity = 1, bool in_filled = false) 
		: center(in_center)
		, radius(in_radius)
		, color(in_color)
		, opacity(in_opacity)
		, filled(in_filled)
		id(in_id),
		{
			
		}
	};
};