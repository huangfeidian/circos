#pragma once
#include "../basics/point.h"
#include "../basics/color.h"
#include <vector>
using std::vector;
using std::string;
using std::stringstream;
#pragma once
namespace spiritsaway::circos
{
	struct Tile
	{
		std::uint16_t inner_radius;
		std::uint16_t outer_radius;
		Point center;
		Color color;
		float opacity;
		int stroke;
		bool fill;
		amplify_angle begin_angle;
		amplify_angle end_angle;
		Tile(Point in_center, std::uint16_t in_radius , std::uint16_t out_radius , amplify_angle in_begin_angle, amplify_angle in_end_angle, Color in_color = Color(), int in_stroke = 1, bool in_fill=false,float in_opacity = 1)
		: center(in_center)
		, inner_radius(in_radius)
		, outer_radius(out_radius)
		, begin_angle(in_begin_angle)
		, end_angle(in_end_angle)
		, color(in_color)
		, stroke(in_stroke)
		, fill(in_fill)
		, opacity(in_opacity)
		{
			 
		}
	};
};