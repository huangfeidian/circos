#pragma once
#include <cmath>
#include "../basics/point.h"
#include "../basics/color.h"
#include "arc.h"
#include "bezier.h"

namespace circos
{
	struct Track
	{
		Arc arc_1;
		Arc arc_2;
		Bezier bezier_1;
		Bezier bezier_2;
		Color color;
		bool fill; 
		double opacity;
		Track(Arc in_arc_1, Arc in_arc_2, Bezier in_bezier_1, Bezier in_bezier_2, 
		Color in_color, bool in_fill = false, double in_opacity = 1.0 )
		: arc_1(in_arc_1)
		, arc_2(in_arc_2)
		, bezier_1(in_bezier_1)
		, bezier_2(in_bezier_2)
		, fill(in_fill)
		, color(in_color)
		, opacity(in_opacity)
		{
			
		}
		Track(Point center, int radius, double span1_begin, double span1_end, double span2_begin, double span2_end, Color in_color, int control_radius =0, bool twist = false,bool in_fill = false, double in_opacity = 1.0)
			: color(in_color)
			, fill(in_fill)
			, opacity(in_opacity)
		{
			arc_1 = Arc(radius, span1_begin, span1_end, center, in_color);
			if (twist)
			{
				std::swap(span2_begin, span2_end);
			}
			
			arc_2=Arc(radius, span2_begin, span2_end, center, in_color);
			bezier_1 = Bezier(center, radius, arc_1.end_angle, arc_2.begin_angle, in_color, control_radius);
			bezier_2 = Bezier(center, radius, arc_2.end_angle, arc_1.begin_angle, in_color, control_radius);
		}
		Track(Point center, int radius1, int radius2, double span1_begin, double span1_end, double span2_begin, double span2_end, Color in_color, int control_radius_percent = 0, bool twist = false,bool in_fill = false, double in_opacity = 1.0)
			: color(in_color)
			, fill(in_fill)
			, opacity(in_opacity)

		{
			arc_1 = Arc(radius1, span1_begin, span1_end, center, in_color);
			if (twist)
			{
				std::swap(span2_begin, span2_end);
			}
			arc_2 = Arc(radius2, span2_begin, span2_end, center, in_color);
			auto control_point_1 = radius_point((radius1 + radius2) / 2 * control_radius_percent, (span1_end + span2_begin) / 2, center);
			auto control_point_2 = radius_point((radius1 + radius2) / 2 * control_radius_percent, (span2_end + span1_begin) / 2, center);
			bezier_1 = Bezier(radius_point(radius1, span1_end, center), radius_point(radius2, span2_begin, center), control_point_1, in_color, in_opacity);
			bezier_2 = Bezier(radius_point(radius2, span2_end, center), radius_point(radius1, span1_begin, center), control_point_2, in_color, in_opacity);

		}
	};
}
