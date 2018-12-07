#pragma once
#include "circle.h"

namespace circos
{
	struct Annulus
	{
		Point center;
		std::uint16_t inner_radius;
		std::uint16_t outer_radius;
		float opacity;
		bool filled;
		Color color;
		Annulus(Point in_center, Color in_color, std::uint16_t in_radius_inner, std::uint16_t in_radius_outer, float in_opacity = 1, bool in_filled = false)
		: center(in_center),
		color(in_color),
		inner_radius(in_radius_inner),
		outer_radius(in_radius_outer),
		opacity(in_opacity),
		filled(in_filled)
		{

		}
	};
}