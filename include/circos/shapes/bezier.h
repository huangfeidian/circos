#pragma once
#include <vector>

#include "../basics/point.h"
#include "../basics/color.h"
#include "line.h"

namespace circos
{
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
				
			auto final_angle = middle;
			if(abs(angle_diff - 180 * amplify_angle::factor) < 3 * amplify_angle::factor)
			{
				
				// 这里我们要随机的让他翻转
				if (angle_diff % 2)
				{
					final_angle = (middle + 180 * amplify_angle::factor) % (360 * amplify_angle::factor);
				}
				
			}
			else
			{
				if(angle_diff >= 180 * amplify_angle::factor)
				{
					final_angle = (middle + 180 * amplify_angle::factor) % (360 * amplify_angle::factor);
				}
			}
			/*std::cout << "begin " << in_begin_angle << " end " << in_end_angle << " middle " << middle <<"diff"<<angle_diff<< " final " << final_angle << std::endl;*/
			control_point = center + Point::radius_point(in_control_radius, final_angle);
		}
		std::vector<Point> path() const
		{
			std::vector<Point> result;
			const auto& p1 = cast_point<std::int16_t, float>(begin_point);
			const auto& p2 = cast_point<std::int16_t, float>(end_point);
			const auto& cp = cast_point<std::int16_t, float>(control_point);
			basic_point<float> c1;
			basic_point<float> c2;
			basic_point<float> double_px;
			Point px;
			int total_len = (Line(begin_point, control_point).len() + Line(control_point, end_point).len()) * 1.5;
			if (total_len == 0)
			{
				return result;
			}
			result.reserve(total_len);
			float step = 1.0 / total_len;
			float inc = 0;
			c1 = p1 + (cp - p1)*inc;
			c2 = cp + (p2 - cp)*inc;
			double_px = c1 + (c2 - c1)*inc;
			px = cast_point<float, std::int16_t>(double_px);
			result.push_back(px);
			inc += step;
			while (inc <= 1)
			{
				c1 = p1 + (cp - p1)*inc;
				c2 = cp + (p2 - cp)*inc;
				double_px = c1 + (c2 - c1)*inc;
				px = cast_point<float, std::int16_t>(double_px);
				if (!(px == result.back()))
				{
					result.push_back(px);
				}
				inc += step;
			}
			return result;

		}
	};
}