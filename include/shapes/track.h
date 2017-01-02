#pragma once
#include <cmath>
#include "point.h"
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
		, opacity(in_opacity)
		{
			
		}
	};
}
