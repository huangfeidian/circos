#include "../color.h"
#include "point.h"
#include <vector>
using std::vector;
using std::string;
using std::stringstream;
#pragma once
namespace circos
{
	struct ring
	{
		double inner_radius;
		double outer_radius;
		Point center;
		Color color;
		double opacity;
		ring(Point in_center, int in_radius = 0, int out_radius = 0, Color in_color = Color(), float in_opacity = 1) 
		: center(in_center)
		, inner_radius(in_radius)
		, outer_radius(out_radius)
		, color(in_color)
		, opacity(in_opacity)
		{
			
		}
	};
};