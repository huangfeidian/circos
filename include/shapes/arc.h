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
		int sweep_flag;//代表是否是顺时针还是逆时针
		bool fill_flag;
		Color color;
		double opacity;
		int stroke;
		Arc()
		{

		}
		Arc(double in_radius, double begin_angle, double end_angle, Point in_center, Color in_color, bool in_fill_flag=false, double in_opacity = 1.0,int in_stroke = 1)
			: fill_flag(in_fill_flag)
			, radius(in_radius)
			, begin_angle(begin_angle)
			, end_angle(end_angle)
			, center(in_center)
			, color(in_color)
			, opacity(in_opacity)
			, stroke(in_stroke)
		{
			if (begin_angle < end_angle)
			{
				sweep_flag = 1;
			}
			else
			{
				sweep_flag = 0;
			}
			from_point = radius_point(in_radius, begin_angle) + center;
			to_point = radius_point(in_radius, end_angle) + center;
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