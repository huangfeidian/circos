#pragma once
#include "../basics/point.h"
#include "../basics/color.h"
#include <sstream>

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
		int sweep_flag;//clock_wise or inverse_clock_wise
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
			sweep_flag = 1; //
			from_point = radius_point(in_radius, begin_angle) + center;
			to_point = radius_point(in_radius, end_angle) + center;
			if (end_angle > begin_angle)
			{
				sweep_flag = 1;
				if (end_angle - begin_angle >= PI)
				{
					large_flag = 1;
				}
				else
				{
					large_flag = 0;
				}
			}
			else
			{
				sweep_flag = 0;
				if (end_angle - begin_angle + 2 * PI > PI)
				{
					large_flag = 0;
				}
				else
				{
					large_flag = 1;
				}
			}
			
		}
		vector<Point> path() const
		{
			vector<Point> result;
			if(abs(begin_angle-end_angle) < EPS)
			{
				return result;
			} 
			const auto& points = Circle::get_circle(radius);
			if(begin_angle >end_angle)
			{
				auto result_1 = Arc(radius,begin_angle,PI*2,center,color).path();
				auto result_2 = Arc(radius,0,end_angle,center,color).path();
				copy(result_2.begin(),result_2.end(), back_inserter(result_1));
				return result_1;
			}
			int begin_idx = begin_angle*4/PI;
			int end_idx = end_angle*4/PI;
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
				result = arc_path(begin_angle+EPS, (begin_idx+1)*PI/4-EPS, radius);
				begin_idx++;
				while(begin_idx < end_idx)
				{
					auto temp = arc_path(begin_idx*PI/4+EPS, (begin_idx+1)*PI/4-EPS, radius);
					copy(temp.begin(),temp.end(), back_inserter(result));
					begin_idx++;
				}
				auto temp = arc_path(begin_idx*PI/4+EPS, end_angle, radius);
				copy(temp.begin(),temp.end(), back_inserter(result));
				for (auto& i : result)
				{
					i += center;
				}
				return result;
			}
		}

		static vector<Point> arc_path(double angle_begin, double angle_end, int radius)
		{
			//这个函数只负责不大于PI/4区域的路径
			if (abs(angle_end - angle_begin) < EPS)
			{
				return vector<Point>();
			}
			int idx = angle_begin * 4 / PI;
			if (idx >= 4)
			{
				//大于半圆的话 直接反转
				angle_begin = 2 * PI - angle_begin;
				angle_end = 2 * PI - angle_end;
				auto result = arc_path(angle_end,angle_begin, radius);
				//将y轴坐标反转
				for (auto & i : result)
				{
					i.y = -i.y;
				}
				return result;
			}
			if (idx >= 2)
			{
				//在左半球的话，直接向右折叠
				angle_begin = PI - angle_begin;
				angle_end = PI - angle_end;
				auto result = arc_path(angle_end,angle_begin,  radius);
				//将x坐标反转
				for (auto& i : result)
				{
					i.x = -i.x;
				}
				return result;
			}
			if (idx == 0)
			{
				//如果在第一象限右半区的话 对折到第一象限的上半区
				angle_begin = PI / 2 - angle_begin;
				angle_end = PI / 2 - angle_end;
				auto result = arc_path(angle_end,angle_begin, radius);
				for (auto& i : result)
				{
					auto temp = i.x;
					i.x = i.y;
					i.y = temp;
				}
				return result;
			}
			//现在确信是在第一象限上半区

			const auto& cache = Circle::get_circle(radius);
			double begin_x = radius*cos(angle_end);
			double end_x = radius*cos(angle_begin);
			auto begin_iter = lower_bound(cache.begin(), cache.end(), begin_x, [](const Point& a, double x)
			{
				return a.x < x;
			});
			auto end_iter = lower_bound(cache.begin(), cache.end(),end_x, [](const Point& a, double x)
			{
				return a.x < x;
			});
			vector<Point> result;
			copy(begin_iter, end_iter, back_inserter(result));
			return result;
		}
	};
}