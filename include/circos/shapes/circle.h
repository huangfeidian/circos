#pragma once
#include <unordered_map>
#include <vector>
#include <string_view>
#include "../basics/point.h"
#include "../basics/color.h"


namespace spiritsaway::circos
{
	struct Circle
	{
		std::uint16_t radius;
		Point center;
		Color color;
		float opacity;
		bool filled;
		std::uint16_t stroke_width;
		Circle(std::uint16_t in_radius, Point in_center, Color in_color , float in_opacity = 1, bool in_filled = false, std::uint16_t in_stroke_width = 1)
		: center(in_center)
		, radius(in_radius)
		, color(in_color)
		, opacity(in_opacity)
		, filled(in_filled)
		, stroke_width(in_stroke_width)
		{
			
		}
		static const std::vector<Point>& get_circle(std::uint16_t radius)
		// 这里返回的是1/8个圆
		{
			static std::unordered_map<std::uint16_t, std::vector<Point>> circle_cache;
			if (circle_cache.find(radius) != circle_cache.end())
			{
				return circle_cache[radius];
			}
			std::vector<Point> path_points;
			int x = 0;
			int y = radius;
			path_points.emplace_back(x, y);
			int p = (5-radius*4) / 4;
			while(x<y)
			{
				x++;
				if(p<0)
				{
					p+=2*x+1;
				}
				else
				{
					y--;
					p+=2*(x-y) + 1;
				}
				path_points.emplace_back(x,y);
			}
			path_points.emplace_back(x, y);
			circle_cache[radius] = std::move(path_points);
			return circle_cache[radius];
		}
		std::vector<Point> path() const
		{
			const auto& one_eight = Circle::get_circle(radius);
			std::uint32_t point_size = one_eight.size();
			std::vector<Point> one_four;
			one_four.insert(one_four.end(), one_eight.begin(), one_eight.end());
			for (const auto& i : one_eight)
			{
				one_four.emplace_back(i.y, i.x);
			}
			for (std::uint32_t i = 0; i <= point_size / 2; i++)
			{
				std::swap(one_four[i + point_size], one_four[2 * point_size - i - 1]);
			}
			point_size *= 2;
			std::vector<Point> one_half;
			one_half.insert(one_half.end(), one_four.begin(), one_four.end());
			for (const auto& i : one_four)
			{
				one_half.emplace_back(-i.x, i.y);
			}
			for (std::uint32_t i = 0; i <= point_size / 2; i++)
			{
				std::swap(one_half[i], one_half[point_size - i - 1]);
			}
			point_size *= 2;
			std::vector<Point> one_total;
			one_total.insert(one_total.end(), one_half.begin(), one_half.end());
			for (const auto& i : one_half)
			{
				one_total.push_back(Point(i.x, -i.y));
			}
			for (std::uint32_t i = 0; i <= point_size / 2; i++)
			{
				std::swap(one_total[i + point_size], one_total[2 * point_size - i - 1]);
			}
			for (auto& i : one_total)
			{
				i += center;
			}
			return one_total;
		}
	};
};