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
				arc_2 = Arc(radius, span2_end, span2_begin, center, in_color);
			}
			else
			{
				arc_2=Arc(radius, span2_begin, span2_end, center, in_color);
			}
			bezier_1 = Bezier(center, radius, arc_1.end_angle, arc_2.begin_angle, in_color, control_radius);
			bezier_2 = Bezier(center, radius, arc_2.end_angle, arc_1.begin_angle, in_color, control_radius);
		}
	};
}
