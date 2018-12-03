#pragma once
#include <cmath>
namespace circos
{
	constexpr double pi()
	{
		return 3.14159265;
	}
	enum class text_align_type: std::uint8_t
	{
		center,
		left,
		right,
	};
	enum class value_on_band_draw_type: std::uint8_t
	{
		point,
		link,
		rectangle,
		link_shadow,
		
	};

}