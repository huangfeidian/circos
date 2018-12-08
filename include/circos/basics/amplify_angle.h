#pragma once
#include <cmath>
#include <cstdint>
#include "constants.h"
namespace circos
{
	struct amplify_angle
	{
		static const int factor = 80; //再往上就可能导致两个角度相加溢出
		static double angle_percent_to_rad(std::uint16_t in_angle_with_percent)
		{
			return pi() / (180 * factor) * in_angle_with_percent;
		}
		static std::uint16_t rad_to_angle_percent(double in_rad)
		{
			return static_cast<std::uint16_t>(int((in_rad / pi()) * 180 * factor) % (360 * factor));
		}
		static std::uint16_t middle(std::uint16_t angle_1, std::uint16_t angle_2)
		{
			return angle_1 / 2 + angle_2 / 2;
		}
	};
}