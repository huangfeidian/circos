#pragma once
#include <cmath>
namespace spiritsaway::circos
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
	enum class text_type : std::uint8_t
	{
		arc,//弧形文字紧凑排列
		line,//切线文字
		normal,// 法向文字 与切线文字正交
		arc_span,// 平摊在弧形上 分散排列
	};
	enum class track_draw_type: std::uint8_t
	{
		point = 1,
		link,
		histogram,
		area,
		fan,
		radius_point,
		color_fan,
	};
}