#pragma once

#include "color.h"
#include "constants.h"
#include "amplify_angle.h"

namespace spiritsaway::circos
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
			new_pos.x = static_cast<T>(p.x*s);
			new_pos.y = static_cast<T>(p.y*s);
			return new_pos;
		}
		friend basic_point operator/(const basic_point& p, double s)
		{
			basic_point new_pos;
			new_pos.x = p.x/s;
			new_pos.y = p.y/s;
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
		static basic_point radius_point(T radius, fixed_angle in_angle, basic_point center = basic_point())
		{
			basic_point result;
			result.x = static_cast<T>(radius * in_angle.cos());
			result.y = static_cast<T>(radius * in_angle.sin());
			return result + center;
		}
		T operator*(const basic_point& other_point)
		{
			// 计算点积
			return x * other_point.x + y * other_point.y;
		}
		static T cross_product(const basic_point& p_a, const basic_point& p_b)
		{
			return p_a.x * p_b.y - p_a.y * p_b.x;
		}

	};
	using Point = basic_point<std::int16_t>;
	template<typename T1, typename T2>
	basic_point<T1> cast_point(const basic_point<T2>& in_point)
	{
		basic_point<T1> result;
		result.x = static_cast<T1>(in_point.x);
		result.y = static_cast<T1>(in_point.y);
		return result;
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
