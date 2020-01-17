#pragma once
#include <cmath>
#include <cstdint>
#include "constants.h"
namespace spiritsaway::circos
{
	struct amplify_angle
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
		static int normalise(int in_angle)
		{
			return in_angle % (360 * factor);
		}
		int value;

		amplify_angle()
		{

		}
		static amplify_angle from_rad(float rad)
		{
			amplify_angle result;
			result.value = static_cast<int>(rad / pi() * factor * 180);
			return result;
		}
		static amplify_angle from_angle(float angle)
		{
			amplify_angle result;
			result.value = static_cast<int>(angle * factor);
			return result;
		}
		friend amplify_angle operator+(const amplify_angle& data_1, const amplify_angle& data_2)
		{
			amplify_angle result;
			result.value = data_1.value + data_2.value;
			return result;
		}
		friend amplify_angle operator-(const amplify_angle& data_1, const amplify_angle& data_2)
		{
			amplify_angle result;
			result.value = data_1.value - data_2.value;
			return result;
		}
		amplify_angle(const amplify_angle& other)
		{
			value = other.value;
		}
		amplify_angle& operator=(const amplify_angle& other)
		{
			value = other.value;
			return *this;
		}
		amplify_angle operator/(float arg) const
		{
			amplify_angle result;
			result.value = static_cast<int>(value / arg);
			return result;
		}
		amplify_angle operator*(float arg) const
		{
			amplify_angle result;
			result.value = static_cast<int>(value * arg);
			return result;
		}
		float angle()const
		{
			return static_cast<float>(normalise(value) * 1.0 / factor);
		}
		float rad() const
		{
			return static_cast<float>(normalise(value) * pi() / (factor * 180));
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
		bool operator==(const amplify_angle& other) const
		{
			return (value - other.value) % (360 * factor) == 0;
		}
		bool operator<(const amplify_angle& other) const
		{
			return normalise(value) < normalise(other.value);
		}
		bool operator<=(const amplify_angle& other) const
		{
			return normalise(value) <= normalise(other.value);
		}
		bool operator>(const amplify_angle& other) const
		{
			return normalise(value) > normalise(other.value);
		}
		bool operator>=(const amplify_angle& other) const
		{
			return normalise(value) >= normalise(other.value);
		}
		amplify_angle normalise() const
		{
			amplify_angle result;
			result.value = normalise(value);
			return result;
		}
	};
}