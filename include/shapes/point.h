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
	template<typename T>
	struct basic_point
	{
		T x;
		T y;
		basic_point() :x(0), y(0)
		{

		}
		basic_point(T in_x, T in_y) :x(in_x), y(in_y)
		{

		}
		friend basic_point operator*(const basic_point& p, double s)
		{
			basic_point new_pos;
			new_pos.x = p.x*s;
			new_pos.y = p.y*s;
			return new_pos;
		}
		friend basic_point operator*(double s,const basic_point& p)
		{
			basic_point new_pos;
			new_pos.x = p.x*s;
			new_pos.y = p.y*s;
			return new_pos;
		}
		friend basic_point operator+(const basic_point& first, const basic_point& second)
		{
			basic_point new_pos;
			new_pos.x = first.x + second.x;
			new_pos.y = first.y + second.y;
			return new_pos;
		}
		friend basic_point operator-(const basic_point& first, const basic_point& second)
		{
			basic_point new_pos;
			new_pos.x = first.x - second.x;
			new_pos.y = first.y - second.y;
			return new_pos;
		}
		bool operator==(const basic_point& second)
		{
			return x == second.x&&y == second.y;
		}
		basic_point& operator+=(const basic_point& second)
		{
			
			this->x = this->x + second.x;
			this->y = this->y + second.y;
			return *this;
		}
		basic_point& operator-=(const basic_point& second)
		{
			
			this->x = this->x - second.x;
			this->y = this->y - second.y;
			return *this;
		}
		friend ostream& operator<<(ostream& in_stream, const basic_point& in_point)
		{
			in_stream << in_point.x << "," << in_point.y << " ";
			return in_stream;
		}
	};
	using Point = basic_point<int>;
	template<typename T1, typename T2>
	basic_point<T2> cast_point(const basic_point<T1>& in_point)
	{
		basic_point<T2> result;
		result.x = static_cast<T2>(in_point.x);
		result.y = static_cast<T2>(in_point.y);
		return result;
	}
	Point radius_point(int radius, double angle, Point center = Point())
	{
		Point result;
		result.x = radius*cos(angle);
		result.y = radius*sin(angle);
		return result+center;
	}
	struct Colorbasic_point
	{
		Point pos;
		Color color;
		Colorbasic_point(Point in_pos, Color in_color)
		: pos(in_pos)
		, color(in_color)
		{
			
		}
	};
}
