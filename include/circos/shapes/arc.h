#pragma once

#include <vector>
#include <algorithm>
#include "../basics/point.h"
#include "../basics/color.h"
#include "circle.h"
#include "line.h"

namespace circos
{
	struct Arc
	{
		Point from_point;
		Point to_point;
		Point center;
		amplify_angle begin_angle;
		amplify_angle end_angle;
		std::uint16_t radius;
		bool large_flag;
		bool sweep_flag;//clock_wise or inverse_clock_wise
		bool fill_flag;
		Color color;
		float opacity;
		std::uint16_t stroke;
		Arc()
		{

		}
		Arc(std::uint16_t in_radius, amplify_angle begin_angle, amplify_angle end_angle, Point in_center, Color in_color, bool in_fill_flag=false, float in_opacity = 1.0, std::uint16_t in_stroke = 1)
			: fill_flag(in_fill_flag)
			, radius(in_radius)
			, begin_angle(begin_angle)
			, end_angle(end_angle)
			, center(in_center)
			, color(in_color)
			, opacity(in_opacity)
			, stroke(in_stroke)
		{
			sweep_flag = 1; //
			from_point = Point::radius_point(in_radius, begin_angle) + center;
			to_point = Point::radius_point(in_radius, end_angle) + center;
			// 根据
			if (abs((end_angle - begin_angle).value) < amplify_angle::factor)
			{
				large_flag = 0;
				sweep_flag = 0;
				return;
			}
			if (abs(abs((end_angle - begin_angle).value) - 360 * amplify_angle::factor) < amplify_angle::factor)
			{
				large_flag = 1;
				sweep_flag = 0;
				return;
			}
			// 根据cross_product来计算sweep_flag
			auto diff_point_1 = from_point - center;
			auto diff_point_2 = to_point - center;
			auto diff_angle = end_angle - begin_angle;
			large_flag = bool(diff_angle.cos() < 0);
			sweep_flag = bool(diff_angle.sin() >= 0);
			
		}
		std::vector<Point> path() const
		{
			std::vector<Point> result;
			if(abs((begin_angle-end_angle).value) < amplify_angle::factor)
			{
				return result;
			} 
			const auto& points = Circle::get_circle(radius);
			if(begin_angle >end_angle)
			{
				auto result_1 = Arc(radius,begin_angle,amplify_angle::from_angle(359.9),center,color).path();
				auto result_2 = Arc(radius, amplify_angle::from_angle(0.1),end_angle,center,color).path();
				std::vector<std::vector<Point>> temp_result;
				temp_result.emplace_back(std::move(result_1));
				Line::connect_paths(temp_result, result_2);
				std::vector<Point> final_result;
				std::copy(temp_result[0].begin(), temp_result[0].end(), std::back_inserter(temp_result[1]));
				return temp_result[1];
			}
			int begin_idx = begin_angle.value / (45 * amplify_angle::factor);
			int end_idx = end_angle.value / (45 * amplify_angle::factor);
			if(begin_idx == end_idx)
			{
				result = arc_path(begin_angle,end_angle, radius);
				for (auto& i : result)
				{
					i += center;
				}
				return result;
			}
			else
			{
				std::vector<std::vector<Point>> temp_result;
				temp_result.push_back(arc_path(begin_angle, amplify_angle::from_angle((begin_idx+1) * 45), radius));
				begin_idx++;
				while(begin_idx < end_idx)
				{
					Line::connect_paths(temp_result, arc_path(amplify_angle::from_angle(begin_idx*45), amplify_angle::from_angle((begin_idx + 1)*45), radius));
					begin_idx++;
				}
				Line::connect_paths(temp_result, arc_path(amplify_angle::from_angle(begin_idx * 45), end_angle, radius));
				result.swap(temp_result[0]);
				for (int i = 1; i < temp_result.size(); i++)
				{
					std::copy(temp_result[i].begin(), temp_result[i].end(), std::back_inserter(result));
				}
				for (auto& i : result)
				{
					i += center;
				}
				return result;
			}
		}

		static std::vector<Point> arc_path(amplify_angle angle_begin, amplify_angle angle_end, std::uint16_t radius)
		{
			//这个函数只负责不大于PI/4区域的路径
			if ((angle_end - angle_begin).value < 1)
			{
				return std::vector<Point>();
			}
			bool negative_y = false;
			bool negative_x = false;
			bool swap_x_y = false;

			int idx = angle_begin.value / (45 * amplify_angle::factor);
			if (idx >= 4)
			{
				//大于半圆的话 直接反转
				std::swap(angle_begin, angle_end);
				angle_begin = amplify_angle::from_angle(360) - angle_begin;
				angle_end = amplify_angle::from_angle(360) - angle_end;
				negative_y = true;
				idx = angle_begin.value / (45 * amplify_angle::factor);

			}
			if (idx >= 2)
			{
				//在左半球的话，直接向右折叠
				std::swap(angle_begin, angle_end);
				angle_begin = amplify_angle::from_angle(180) - angle_begin;
				angle_end = amplify_angle::from_angle(180) - angle_end;
				negative_x = true;
				idx = angle_begin.value / (45 * amplify_angle::factor);
			}
			if (idx == 0)
			{
				//如果在第一象限右半区的话 对折到第一象限的上半区
				angle_begin = amplify_angle::from_angle(90) - angle_begin;
				angle_end = amplify_angle::from_angle(90) - angle_end;
				swap_x_y = true;
				std::swap(angle_begin, angle_end);
			}
			//现在确信是在第一象限上半区
			const auto& cache = Circle::get_circle(radius);
			double begin_x = radius*angle_end.cos();
			double end_x = radius*angle_begin.cos();
			auto begin_iter = std::lower_bound(cache.begin(), cache.end(), begin_x, [](const Point& a, double x)
			{
				return a.x < x;
			});
			auto end_iter = std::lower_bound(cache.begin(), cache.end(),end_x, [](const Point& a, double x)
			{
				return a.x < x;
			});
			std::vector<Point> result;
			std::copy(begin_iter, end_iter, back_inserter(result));
			std::transform(result.begin(), result.end(), result.begin(), [=](const Point& origin_p)
			{
				Point new_p = origin_p;
				if (swap_x_y)
				{
					auto temp = new_p.x;
					new_p.x = new_p.y;
					new_p.y = temp;
				}
				if (negative_x)
				{
					new_p.x = -new_p.x;
				}
				if (negative_y)
				{
					new_p.y = -new_p.y;
				}
				return new_p;

			});
			return result;
		}
	};
}