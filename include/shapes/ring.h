#pragma once
#include "../color.h"
#include "point.h"
#include <vector>
using std::vector;
using std::string;
using std::stringstream;
#pragma once
namespace circos
{
	struct Ring
	{
		double inner_radius;
		double outer_radius;
		Point center;
		Color color;
		double opacity;
		int stroke;
		bool fill;
		double begin_angle;
		double end_angle;
		Ring(Point in_center, int in_radius , int out_radius ,double in_begin_angle,double in_end_angle, Color in_color = Color(), int in_stroke = 1, bool in_fill=false,float in_opacity = 1) 
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