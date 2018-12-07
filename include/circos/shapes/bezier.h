#pragma once
#include <vector>

#include "../basics/point.h"
#include "../basics/color.h"
#include "line.h"
#include <iostream>

namespace circos
{
#define EPS 100
	struct Bezier
	{
		Point begin_point;
		Point end_point;
		Point control_point;
		Color color;
		float opacity;
		std::uint16_t stroke_width;
		Bezier()
		{

		}
		Bezier(Point in_begin, Point in_end,Point in_control, Color in_color, std::uint16_t in_stroke = 1, float in_opacity = 1.0 )
		: begin_point(in_begin)
		, end_point(in_end)
		, control_point(in_control)
		, color(in_color)
		, stroke_width(in_stroke)
		, opacity(in_opacity)
		{

		}
		Bezier(Point center,std::uint16_t in_on_radius, std::uint16_t in_begin_angle, std::uint16_t in_end_angle, Color in_color,std::uint16_t in_control_radius=0,
		 std::uint16_t in_stroke_width = 1, float in_opacity = 1.0)
			: stroke_width(in_stroke_width)
			, color(in_color)
			, opacity(in_opacity)
			//这个构造函数的最重要参数是control radius
			//代表的是这个bezier的控制点与圆的中心点之间的距离
			//控制点的角度为这两个角度的中间值
		{
			begin_point = center+Point::radius_point(in_on_radius, in_begin_angle);
			end_point = center+Point::radius_point(in_on_radius, in_end_angle);
			if (in_begin_angle > in_end_angle)
			{
				std::swap(in_begin_angle, in_end_angle);
			}
			auto angle_diff = in_end_angle - in_begin_angle;
			auto middle = amplify_angle::middle(in_begin_angle, in_end_angle);
			if(abs(angle_diff - 180 * amplify_angle::factor) < EPS)
			{
				
				// 这里我们要随机的让他翻转
				auto final_angle = middle;
				if (angle_diff % 2)
				{
					final_angle = (middle + 180 * amplify_angle::factor) % (360 * amplify_angle::factor);
				}
				control_point = center + Point::radius_point(in_control_radius, final_angle);
				std::cout << "final angle " << final_angle <<"middle" <<middle<< "angle diff"<< angle_diff<<std::endl;
			}
			else
			{
				if(angle_diff < 180 * amplify_angle::factor)
				{
					control_point = center+Point::radius_point(in_control_radius, middle);
				}
				else
				{
					control_point = center+Point::radius_point(in_control_radius, 360 * amplify_angle::factor - middle);
				}
			}
		}
		std::vector<Point> path() const
		{
			std::vector<Point> result;
			const auto& p1 = cast_point<std::int16_t,double>(begin_point);
			const auto& p2 = cast_point<std::int16_t, double>(end_point);
			const auto& cp = cast_point<std::int16_t, double>(control_point);
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
			px = cast_point<double, std::int16_t>(double_px);
			result.push_back(px);
			inc += step;
			while (inc <= 1)
			{
				c1 = p1 + (cp - p1)*inc;
				c2 = cp + (p2 - cp)*inc;
				double_px = c1 + (c2 - c1)*inc;
				px = cast_point<double,std::int16_t>(double_px);
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