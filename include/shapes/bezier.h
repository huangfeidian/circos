#pragma once
#include "point.h"
using std::endl;
using std::stringstream;
namespace circos
{
#define EPS 0.01
#define PI 3.14159f
	struct Bezier
	{
		Point begin_point;
		Point end_point;
		Point control_point;
		Color color;
		double opacity;
		int stroke_width;
		Bezier()
		{

		}
		Bezier(Point in_begin, Point in_end,Point in_control, Color in_color, int in_stroke = 1, double in_opacity = 1.0 )
		: begin_point(in_begin)
		, end_point(in_end)
		, control_point(in_control)
		, color(in_color)
		, stroke_width(in_stroke)
		, opacity(in_opacity)
		{

		}
		Bezier(Point center,int in_on_radius, double in_begin_angle, double in_end_angle, Color in_color,int in_control_radius=0,
		 int in_stroke_width = 1, double in_opacity = 1.0)
			: stroke_width(in_stroke_width)
			, color(in_color)
			, opacity(in_opacity)
			//这个构造函数的最重要参数是control radius
			//代表的是这个bezier的控制点与圆的中心点之间的距离
			//控制点的角度为这两个角度的中间值
		{
			begin_point = center+radius_point(in_on_radius, in_begin_angle);
			end_point = center+radius_point(in_on_radius, in_end_angle);
			double radius_diff = in_end_angle - in_begin_angle;
			if(abs(radius_diff - PI) < EPS)
			{
				if(in_begin_angle < PI)
				{
					control_point = center+radius_point(in_control_radius, (in_begin_angle + in_end_angle)/2);
				}
				else
				{
					control_point = center+radius_point(in_control_radius, (in_begin_angle + in_end_angle) / 2 - PI);
				}
			}
			else
			{
				if(radius_diff < PI)
				{
					control_point = center+radius_point(in_control_radius, (in_begin_angle + in_end_angle)/2);
				}
				else
				{
					control_point = center+radius_point(in_control_radius, (in_begin_angle + in_end_angle) / 2 - PI);
				}
			}
		}
	};
}