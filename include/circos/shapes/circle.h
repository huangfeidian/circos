#pragma once
#include <unordered_map>
#include <vector>
#include <string_view>
#include "../basics/point.h"
#include "../basics/color.h"


namespace circos
{
	struct Circle
	{
		double radius;
		Point center;
		Color color;
		double opacity;
		bool filled;
		Circle(double in_radius, Point in_center, Color in_color , double in_opacity = 1, bool in_filled = false) 
		: center(in_center)
		, radius(in_radius)
		, color(in_color)
		, opacity(in_opacity)
		, filled(in_filled)
		{
			
		}
		static const std::vector<Point>& get_circle(int radius)
		// 这里返回的是1/8个圆
		{
			static std::unordered_map<int, std::vector<Point>> circle_cache;
			if (circle_cache.find(radius) != circle_cache.end())
			{
				return circle_cache[radius];
			}
			std::vector<Point> path_points;
			int x = 0;
			int y = radius;
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
			circle_cache[radius] = std::move(path_points);
			return circle_cache[radius];
		}
		std::vector<Point> path() const
		{
			const auto& one_eight = Circle::get_circle(radius);
			std::vector<Point> one_four;
			copy(one_eight.begin(), one_eight.end(), back_inserter(one_four));
			for (const auto& i : one_eight)
			{
				one_four.emplace_back(i.y, i.x);
			}
			std::vector<Point> one_half;
			copy(one_four.begin(), one_four.end(), back_inserter(one_half));
			for (const auto& i : one_four)
			{
				one_half.emplace_back(-i.x, i.y);
			}
			std::vector<Point> one_total;
			copy(one_half.begin(), one_half.end(), back_inserter(one_total));
			for (const auto& i : one_half)
			{
				one_total.push_back(Point(i.x, -i.y));
			}
			for (auto& i : one_total)
			{
				i += center;
			}
			return one_total;
		}
	};
};