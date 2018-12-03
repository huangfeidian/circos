#pragma once
#include <vector>

#include "../basics/point.h"
#include "../basics/color.h"
#include "line.h"

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
			begin_point = center+Point::radius_point(in_on_radius, in_begin_angle);
			end_point = center+Point::radius_point(in_on_radius, in_end_angle);
			double radius_diff = abs(in_end_angle - in_begin_angle);
			if(abs(radius_diff - PI) < EPS)
			{
				// 这里我们要随机的让他翻转
				int second_digit = (int(in_begin_angle+in_end_angle * 100) / 10)%10;
				if(in_begin_angle < PI)
				{
					control_point = center+Point::radius_point(in_control_radius, (in_begin_angle + in_end_angle)/2);
				}
				else
				{
					control_point = center+Point::radius_point(in_control_radius, (in_begin_angle + in_end_angle) / 2 - PI);
				}
			}
			else
			{
				if(radius_diff < PI)
				{
					control_point = center+Point::radius_point(in_control_radius, (in_begin_angle + in_end_angle)/2);
				}
				else
				{
					control_point = center+Point::radius_point(in_control_radius, (in_begin_angle + in_end_angle) / 2 - PI);
				}
			}
		}
		std::vector<Point> path() const
		{
			std::vector<Point> result;
			const auto& p1 = cast_point<int,double>(begin_point);
			const auto& p2 = cast_point<int, double>(end_point);
			const auto& cp = cast_point<int, double>(control_point);
			basic_point<double> c1;
			basic_point<double> c2;
			basic_point<double> double_px;
			Point px;
			int total_len = (Line(begin_point, control_point).len() + Line(control_point, end_point).len());
			if (total_len == 0)
			{
				return result;
			}
			double step = 1.0 / total_len;
			double inc = 0;
			c1 = p1 + (cp - p1)*inc;
			c2 = cp + (p2 - cp)*inc;
			double_px = c1 + (c2 - c1)*inc;
			px = cast_point<double,int>(double_px);
			result.push_back(px);
			inc += step;
			while (inc <= 1)
			{
				c1 = p1 + (cp - p1)*inc;
				c2 = cp + (p2 - cp)*inc;
				double_px = c1 + (c2 - c1)*inc;
				px = cast_point<double,int>(double_px);
				if (!(px==result.back()))
				{
					result.push_back(px);
				}
				inc += step;
			}
			return result;
		}
	};
}