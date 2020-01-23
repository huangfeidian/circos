#pragma once
#include <cmath>
#include <cstdint>
#include "constants.h"
namespace spiritsaway::circos
{
	class free_angle;
	struct fixed_angle
	{
		static const int factor = 100; // 每一个角度的精度
		static double angle_percent_to_rad(int in_angle_with_percent)
		{
			return pi() / (180 * factor) * in_angle_with_percent;
		}
		static int rad_to_angle_percent(double in_rad)
		{
			return int((in_rad / pi()) * 180 * factor) % (360 * factor);
		}
		static int middle(int angle_1, int angle_2)
		{
			return angle_1 / 2 + angle_2 / 2;
		}

	public:
		
		friend class free_angle;
	private:
		int _value;
		fixed_angle(int in_value)
			:_value(in_value)
		{

		}
	public:
		fixed_angle(const fixed_angle& other)
			:_value(other._value)
		{

		}
		fixed_angle()
			:_value(0)
		{

		}
		fixed_angle& operator=(const fixed_angle& other)
		{
			_value = other._value;
			return *this;
		}
		int value() const
		{
			return _value;
		}
		float angle()const
		{
			return static_cast<float>(_value * 1.0 / factor);
		}
		float rad() const
		{
			return static_cast<float>(_value * pi() / (factor * 180));
		}
		float sin() const
		{
			return std::sin(rad());
		}
		float cos() const
		{
			return std::cos(rad());
		}
		float tan() const
		{
			return std::tan(rad());
		}
		bool operator==(const fixed_angle& other) const
		{
			return _value == other._value;
		}
		bool operator<(const fixed_angle& other) const
		{
			return _value < other._value;
		}
		bool operator<=(const fixed_angle& other) const
		{
			return _value <= other._value;
		}
		bool operator>(const fixed_angle& other) const
		{
			return _value > other._value;
		}
		bool operator>=(const fixed_angle& other) const
		{
			return _value >= other._value;
		}
		static fixed_angle middle(const fixed_angle& from, const fixed_angle& to)
		{
			if (from < to)
			{
				return fixed_angle((from.value() + to.value()) / 2);
			}
			else
			{
				return fixed_angle((from.value() + to.value()) / 2 + 180 * factor);
			}
		}
	};
	class free_angle
	{
	private:
		int value;
	public:
		free_angle()
		{

		}
		free_angle(const fixed_angle& fix_data)
			:value(fix_data._value)
		{

		}
		fixed_angle normal() const
		{
			return fixed_angle(value % (360 * fixed_angle::factor));
		}
		operator fixed_angle() const
		{
			return normal();
		}

		static free_angle from_rad(float rad)
		{
			free_angle result;
			result.value = static_cast<int>(rad / pi() * fixed_angle::factor * 180);
			return result;
		}
		static free_angle from_angle(float angle)
		{
			free_angle result;
			result.value = static_cast<int>(angle * fixed_angle::factor);
			return result;
		}
		friend free_angle operator+(const free_angle& data_1, const free_angle& data_2)
		{
			free_angle result;
			result.value = data_1.value + data_2.value;

			return result;
		}
		friend free_angle operator-(const free_angle& data_1, const free_angle& data_2)
		{
			free_angle result;
			result.value = data_1.value - data_2.value;
			return result;
		}
		free_angle(const free_angle& other)
		{
			value = other.value;
		}
		free_angle& operator=(const free_angle& other)
		{
			value = other.value;
			return *this;
		}
		free_angle operator/(float arg) const
		{
			free_angle result;
			result.value = static_cast<int>(value / arg);
			return result;
		}
		free_angle operator*(float arg) const
		{
			free_angle result;
			result.value = static_cast<int>(value * arg);
			return result;
		}
	};
}