#pragma once

#include <cmath>
#include <ostream>

#include "../color.h"
using std::ostream;
using std::sin;
using std::cos;
#pragma once
namespace circos
{
	struct Point
	{
		int x;
		int y;
		Point() :x(0), y(0)
		{

		}
		Point(int in_x, int in_y) :x(in_x), y(in_y)
		{

		}
		Point(int radius, double angle)
		{
			x = radius*cos(angle);
			y = radius*sin(angle);
		}
		friend Point operator*(const Point& p, float s)
		{
			Point new_pos;
			new_pos.x = p.x*s;
			new_pos.y = p.y*s;
			return new_pos;
		}
		friend Point operator*(float s,const Point& p)
		{
			Point new_pos;
			new_pos.x = p.x*s;
			new_pos.y = p.y*s;
			return new_pos;
		}
		friend Point operator+(const Point& first, const Point& second)
		{
			Point new_pos;
			new_pos.x = first.x + second.x;
			new_pos.y = first.y + second.y;
			return new_pos;
		}
		friend Point operator-(const Point& first, const Point& second)
		{
			Point new_pos;
			new_pos.x = first.x - second.x;
			new_pos.y = first.y - second.y;
			return new_pos;
		}
		bool operator==(const Point& second)
		{
			return x == second.x&&y == second.y;
		}
		Point& operator+=(const Point& second)
		{
			
			this->x = this->x + second.x;
			this->y = this->y + second.y;
			return *this;
		}
		Point& operator-(const Point& second)
		{
			Point new_pos;
			new_pos.x = x - second.x;
			new_pos.y = y - second.y;
			return new_pos;
		}
		Point& operator-=(const Point& second)
		{
			
			this->x = this->x - second.x;
			this->y = this->y - second.y;
			return *this;
		}
		friend ostream& operator<<(ostream& in_stream, const Point& in_point)
		{
			in_stream << in_point.x << "," << in_point.y << " ";
			return in_stream;
		}
	};
	struct ColorPoint
	{
		Point pos;
		Color color;
		ColorPoint(Point in_pos, Color in_color)
		: pos(in_pos)
		, color(in_color)
		{
			
		}
	};
}
