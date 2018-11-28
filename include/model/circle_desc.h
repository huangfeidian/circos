#include <vector>
#include <string>
#include <string_view>
#include "../basics/point.h"
#include "../basics/color.h"

namespace circos::model
{
	struct circle_desc
	{
		std::string_view circle_id;
		int inner_radius;
		int outer_radius;
		int gap;
		Color fill_color;
		double opacity;
	};
	struct band_desc
	{
		std::string_view circle_id;
		std::string_view band_id;
		int range_begin;
		int range_end;
		Color fill_color;
		double opacity;
	};
	struct circle_tick
	{
		// 处理圆环上的刻度
		std::string_view circle_id;
		int gap;
		Color fill_color;
		int width;
		int height;
		double opacity;
	};
	struct model_config
	{
		int radius;
		Color background_color;
	};
}