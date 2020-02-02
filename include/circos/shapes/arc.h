#pragma once

#include <vector>
#include <algorithm>
#include "../basics/point.h"
#include "../basics/color.h"
#include "circle.h"
#include "line.h"

namespace spiritsaway::circos
{
	struct Arc
	{
		Point center;
		fixed_angle _begin_angle;
		fixed_angle width;
		std::uint16_t radius;
		bool fill_flag;
		bool reverse_end;
		Color color;
		float opacity;
		std::uint16_t stroke;
		Arc()
		{

		}
		Arc(std::uint16_t in_radius, fixed_angle _begin_angle, fixed_angle in_width, Point in_center, bool in_reverse_end, Color in_color, bool in_fill_flag=false, float in_opacity = 1.0, std::uint16_t in_stroke = 1)
			: fill_flag(in_fill_flag)
			, radius(in_radius)
			, _begin_angle(_begin_angle)
			, width(in_width)
			, center(in_center)
			, color(in_color)
			, opacity(in_opacity)
			, stroke(in_stroke)
			, reverse_end(in_reverse_end)
		{
			
		}
		bool large_flag() const
		{
			return width > free_angle::from_angle(180);
		}
		template<typename T = std::int32_t>
		basic_point<T> from_point() const
		{
			return basic_point<T>::radius_point(radius, from_angle()) + cast_point<T>(center);;
			
		}
		template<typename T = std::int32_t>
		basic_point<T> to_point() const
		{
			return basic_point<T>::radius_point(radius, to_angle()) + cast_point<T>(center);
		}

		template<typename T = std::int32_t>
		basic_point<T> middle_point() const
		{
			return basic_point<T>::radius_point(radius, (free_angle(_begin_angle) + free_angle(width) / 2)) + cast_point<T>(center);;
		}
		fixed_angle from_angle() const
		{
			if (!reverse_end)
			{
				return _begin_angle;
			}
			else
			{
				return (free_angle(_begin_angle) + free_angle(width));
			}
		}
		fixed_angle middle_angle() const
		{
			return (free_angle(_begin_angle) + free_angle(width) / 2);
		}
		fixed_angle to_angle() const
		{
			if (reverse_end)
			{
				return _begin_angle;
			}
			else
			{
				return (free_angle(_begin_angle) + free_angle(width));
			}
		}
		std::vector<Point> path() const
		{
			auto temp_result = path_without_orient();
			if (reverse_end)
			{
				std::reverse(temp_result.begin(), temp_result.end());
				return temp_result;
			}
			else
			{
				return temp_result;
			}
		}
		std::vector<Point> path_without_orient() const
		{
			// 这个函数并没有考虑方向的问题 业务层自己搞一个吧
			std::vector<Point> result;
			if(width.value() < fixed_angle::factor)
			{
				return result;
			} 
			auto end_angle = (free_angle(_begin_angle) + width);
			const auto& points = Circle::get_circle(radius);
			if(_begin_angle > end_angle)
			{
				auto result_1 = Arc(radius,_begin_angle, (free_angle::from_angle(359.9f) - _begin_angle), center, reverse_end, color).path_without_orient();
				auto result_2 = Arc(radius, free_angle::from_angle(0.1f), (free_angle(end_angle) - free_angle::from_angle(0.1f)), center, reverse_end, color).path_without_orient();
				std::vector<std::vector<Point>> temp_result;
				temp_result.emplace_back(std::move(result_1));
				Line::connect_paths(temp_result, result_2);
				std::vector<Point> final_result;
				std::copy(temp_result[0].begin(), temp_result[0].end(), std::back_inserter(temp_result[1]));
				return temp_result[1];
			}
			int begin_idx = _begin_angle.value() / (45 * fixed_angle::factor);
			int end_idx = end_angle.normal().value() / (45 * fixed_angle::factor);
			if(begin_idx == end_idx)
			{
				result = arc_path(_begin_angle,end_angle, radius);
				for (auto& i : result)
				{
					i += center;
				}
				return result;
			}
			else
			{
				std::vector<std::vector<Point>> temp_result;
				temp_result.push_back(arc_path(_begin_angle, free_angle::from_angle((begin_idx+1) * 45.0f), radius));
				begin_idx++;
				while(begin_idx < end_idx)
				{
					Line::connect_paths(temp_result, arc_path(free_angle::from_angle(begin_idx*45.0f), free_angle::from_angle((begin_idx + 1)*45.0f), radius));
					begin_idx++;
				}
				Line::connect_paths(temp_result, arc_path(free_angle::from_angle(begin_idx * 45.0f), end_angle, radius));
				result.swap(temp_result[0]);
				for (std::uint32_t i = 1; i < temp_result.size(); i++)
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

		static std::vector<Point> arc_path(free_angle angle_begin, free_angle angle_end, std::uint16_t radius)
		{
			
			//这个函数只负责不大于PI/4区域的路径
			if ((angle_end - angle_begin).normal().value() < 1)
			{
				return std::vector<Point>();
			}
			bool negative_y = false;
			bool negative_x = false;
			bool swap_x_y = false;

			int idx = angle_begin.normal().value() / (45 * fixed_angle::factor);
			if (idx >= 4)
			{
				//大于半圆的话 直接反转
				std::swap(angle_begin, angle_end);
				angle_begin = free_angle::from_angle(360.0f) - angle_begin;
				angle_end = free_angle::from_angle(360.0f) - angle_end;
				negative_y = true;
				idx = angle_begin.normal().value() / (45 * fixed_angle::factor);

			}
			if (idx >= 2)
			{
				//在左半球的话，直接向右折叠
				std::swap(angle_begin, angle_end);
				angle_begin = free_angle::from_angle(180.0f) - angle_begin;
				angle_end = free_angle::from_angle(180.0f) - angle_end;
				negative_x = true;
				idx = angle_begin.normal().value() / (45 * fixed_angle::factor);
			}
			if (idx == 0)
			{
				//如果在第一象限右半区的话 对折到第一象限的上半区
				angle_begin = free_angle::from_angle(90.0f) - angle_begin;
				angle_end = free_angle::from_angle(90.0f) - angle_end;
				swap_x_y = true;
				std::swap(angle_begin, angle_end);
			}
			//现在确信是在第一象限上半区
			const auto& cache = Circle::get_circle(radius);
			double begin_x = radius*angle_end.normal().cos();
			double end_x = radius*angle_begin.normal().cos();
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