#pragma once

#include <cmath>
#include <utility>
#include <vector>

#include "../basics/point.h"
#include "../basics/color.h"

namespace circos
{
	struct Line
	{
		Point from;
		Point to;
		Color color;
		float opacity;
		std::uint16_t width;
		Line() :from(), to(), color(), opacity(1), width(1)
		{
		
		}
		Line(const Point in_from, const Point in_to, Color in_color = Color(), std::uint16_t in_width = 1,float in_opacity= 1.0)
			:from(in_from), to(in_to), color(in_color), width(in_width),opacity(in_opacity)
		{
			
		}
		double len() const
		{
			return std::sqrt(len_square());
		}
		int len_square() const
		{
			int diff_x = from.x - to.x;
			int diff_y = from.y - to.y;
			return diff_x * diff_x + diff_y * diff_y;
		}
		std::vector<Point> path() const
		{
			std::vector<Point> result;
			int xfrom = from.x;
			int yfrom = from.y;
			int xto = to.x;
			int yto = to.y;
			//  Bresenham Algorithm.
			//
			int dy = yto - yfrom;
			int dx = xto - xfrom;
			int stepx, stepy;

			if (dy < 0)
			{
				dy = -dy;
				stepy = -1;
			}
			else
			{
				stepy = 1;
			}
			if (dx < 0)
			{
				dx = -dx;
				stepx = -1;
			}
			else
			{
				stepx = 1;
			}
			dy <<= 1;     // dy is now 2*dy
			dx <<= 1;     // dx is now 2*dx
			
			result.emplace_back(xfrom, yfrom);
			if (dx > dy)
			{
				int fraction = dy - (dx >> 1);
				while (xfrom != xto)
				{
					if (fraction >= 0)
					{
						yfrom += stepy;
						fraction -= dx;
					}
					xfrom += stepx;
					fraction += dy;
					result.emplace_back(xfrom , yfrom);
				}
			}
			else
			{
				int fraction = dx - (dy >> 1);
				while (yfrom != yto)
				{
					if (fraction >= 0)
					{
						xfrom += stepx;
						fraction -= dy;
					}
					yfrom += stepy;
					fraction += dx;
					result.emplace_back(xfrom, yfrom);
				}
			}
			return result;
		}
		Point symmetric_point(Point point) const
		// 获得对称点
		{
			std::pair<int, int> vec_1 = std::make_pair(to.x - from.x, to.y - from.y);
			std::pair<int, int> vec_2 = std::make_pair(point.x - from.x, point.y - from.y);
			float length = (vec_1.first*vec_2.first + vec_1.second*vec_2.second)/(1.0*(vec_1.first*vec_1.first+vec_1.second*vec_1.second));
			std::pair<int, int> remain = static_cast<std::pair<int,int>>(std::make_pair(vec_2.first - length*vec_1.first, vec_2.second - length*vec_1.second));
			std::pair<int, int> vec_3 = std::make_pair(vec_2.first - 2 * remain.first, vec_2.second - 2 * remain.second);
			return Point(vec_3.first + from.x, vec_3.second + from.y);
		}
		static std::vector<Point> connect_points(Point p_a, Point p_b)
		{
			// 补充两点之间的连接点
			std::vector<Point> result;
			auto diff_point = p_b - p_a;
			if(abs(diff_point.x) > 1 || abs(diff_point.y) > 1)
			{
				return Line(p_a, p_b).path();
			}
			else
			{
				return result;
			}
		}
		static void connect_paths(std::vector<std::vector<Point>>& pre_paths, std::vector<Point>& new_path)
		{
			if (pre_paths.size() == 0)
			{
				pre_paths.emplace_back(std::move(new_path));
				return;
			}
			if (new_path.size() == 0)
			{
				return;
			}
			if(pre_paths.size() == 1)
			{
				// 这里可能需要首尾相连
				std::vector<int> dis_vec;
				dis_vec.push_back(Line(pre_paths.back().back(), new_path.front()).len_square());
				dis_vec.push_back(Line(pre_paths.back().back(), new_path.back()).len_square());
				dis_vec.push_back(Line(pre_paths.back().front(), new_path.back()).len_square());
				dis_vec.push_back(Line(pre_paths.back().front(), new_path.front()).len_square());
				double min_dis = dis_vec[0];
				int min_mode = 0;
				for (int i = 0; i < dis_vec.size(); i++)
				{
					if (dis_vec[i] < min_dis)
					{
						min_mode = i;
						min_dis = dis_vec[i];
					}
				}
				switch(min_mode)
				{
				case 0:
					pre_paths.emplace_back(std::move(new_path));
					break;
				case 1:
					std::reverse(new_path.begin(), new_path.end());
					pre_paths.emplace_back(std::move(new_path));
					break;
				case 2:
					std::reverse(pre_paths.back().begin(), pre_paths.back().end());
					std::reverse(new_path.begin(), new_path.end());
					pre_paths.emplace_back(std::move(new_path));
					break;
				case 3:
					std::reverse(pre_paths.back().begin(), pre_paths.back().end());
					pre_paths.emplace_back(std::move(new_path));
					break;
				default:
					pre_paths.emplace_back(std::move(new_path));
				}
			}
			else
			{
				if (Line(pre_paths.back().back(), new_path.front()).len() > Line(pre_paths.back().back(), new_path.back()).len())
				{
					std::reverse(new_path.begin(), new_path.end());
				}
				pre_paths.emplace_back(std::move(new_path));
			}
		}

		static Line cacl_tangent_clock_wise(Point center, Point tangent_point)
		{
			// 计算圆上的一点的顺时针切线
			int diff_x = tangent_point.x - center.x;
			int diff_y = tangent_point.y - center.y;
			int new_diff_x = -1 * diff_y;
			int new_diff_y = diff_x;

			return Line(tangent_point, tangent_point + Point(new_diff_x, new_diff_y));
		}
	};
}
