#pragma once
#include <cmath>
#include "../basics/point.h"
#include "../basics/color.h"
#include "arc.h"
#include "bezier.h"

namespace spiritsaway::circos
{
	struct Ribbon
	{
		Arc arc_1;
		Arc arc_2;
		Bezier bezier_1;
		Bezier bezier_2;
		Color color;
		bool fill; 
		float opacity;
		bool twisted;

		Ribbon(Point center, std::uint16_t radius, fixed_angle span1_begin, fixed_angle span1_width, fixed_angle span2_begin, fixed_angle span2_width, Color in_color, bool twist, std::uint16_t control_radius = 0 , bool in_fill = false, float in_opacity = 1.0)
			: color(in_color)
			, fill(in_fill)
			, opacity(in_opacity)
			, twisted(twist)
		{
			arc_1 = Arc(radius, span1_begin, span1_width, center, false, in_color);
			
			arc_2=Arc(radius, span2_begin, span2_width, center, !twisted, in_color);
			bezier_1 = Bezier(center, radius, arc_1.to_angle(), arc_2.from_angle(), in_color, control_radius);
			bezier_2 = Bezier(center, radius, arc_2.to_angle(), arc_1.from_angle(), in_color, control_radius);
			
		}
		Ribbon(Point center, std::uint16_t radius1, std::uint16_t radius2, fixed_angle span1_begin, fixed_angle span1_width, fixed_angle span2_begin, fixed_angle span2_width, Color in_color, bool twist, float control_radius_percent = 0, bool in_fill = false, float in_opacity = 1.0f)
			: color(in_color)
			, fill(in_fill)
			, opacity(in_opacity)
			, twisted(twist)
		{
			arc_1 = Arc(radius1, span1_begin, span1_width, center,false,  in_color);

			arc_2 = Arc(radius2, span2_begin, span2_width, center, !twisted, in_color);
			auto control_point_1 = Point::radius_point(static_cast<std::uint16_t>((radius1 + radius2) / 2 * control_radius_percent), fixed_angle::middle(arc_1.to_angle(), arc_2.from_angle()), center);
			auto control_point_2 = Point::radius_point(static_cast<std::uint16_t>((radius1 + radius2) / 2 * control_radius_percent), fixed_angle::middle(arc_2.to_angle(), arc_1.from_angle()), center);
			bezier_1 = Bezier(arc_1.to_point(), arc_2.from_point(), control_point_1, in_color, 1, in_opacity);
			bezier_2 = Bezier(arc_2.to_point(), arc_1.from_point(), control_point_2, in_color, 1, in_opacity);

		}
	};
}
