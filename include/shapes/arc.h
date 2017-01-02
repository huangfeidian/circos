#include "point.h"
#include <sstream>
#pragma once
using std::endl;
using std::stringstream;
namespace circos
{
#define EPS 0.01
#define PI 3.14159f
	struct Arc
	{
		Point from_point;
		Point to_point;
		Point center;
		double begin_angle;
		double end_angle;
		double radius;
		int large_flag;
		int sweep_flag;
		Color color;
		double opacity;
		int stroke;
		Arc(double in_radius, double begin_angle, double end_angle, Point in_center, Color in_color, int in_sweep_flag=1, double in_opacity = 1.0,int in_stroke = 1)
			: sweep_flag(in_sweep_flag)
			, radius(in_radius)
			, begin_angle(begin_angle)
			, end_angle(end_angle)
			, center(in_center)
			, color(in_color)
			, opacity(in_opacity)
			, stroke(in_stroke)
		{
			from_point = Point(in_radius, begin_angle) + center;
			to_point = Point(in_radius, end_angle) + center;
			if (abs(end_angle - begin_angle )> PI )
			{
				large_flag = 1;
			}
			else
			{
				large_flag = 0;
			}
		}
	};
}