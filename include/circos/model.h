#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "basics/point.h"
#include "basics/color.h"
#include "basics/constants.h"

#include "shape_collection.h"
#include "shapes/circle.h"
#include "shapes/line.h"
#include "shapes/arc.h"
#include "shapes/ring.h"


namespace circos::model
{

	struct circle
	{
		std::string_view circle_id;
		int inner_radius;
		int outer_radius;
		int gap;
		Color fill_color;
		double opacity;
	};
	struct band
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

		struct point_link
	{
		std::string_view link_id;
		std::string_view from_band_id;
		int from_pos_idx;
		std::string_view to_band_id;
		int to_pos_idx;
		double control_radius_percent; //negative for direct link
		Color fill_color;
		double opacity;

	};
	struct range_link
	{
		std::string_view link_id;
		std::string_view from_band_id;
		int from_pos_begin_idx;
		int from_pos_end_idx;
		std::string_view to_band_id;
		int to_pos_begin_idx;
		int to_pos_end_idx;
		bool is_cross;
		Color fill_color;
		double opacity;

	};

		struct fill_onband
	{
		std::string_view band_id;
		int on_band_begin;
		int on_band_end;
		Color fill_color;
	};

	struct text_onband:public fill_onband
	{
		std::string_view text;
		int font_size;
		std::string_view font_name;
		text_align_type align_type;
	};
	struct tick_onband: public fill_onband
	{
		int tick_gap;
		int width;
		int height;
	};
	struct value_onband: public fill_onband
	{
		double value;
	};
	
	struct value_onband_config
	{
		circos::value_on_band_draw_type draw_type;
		double base_value;
		double max_value;
		double scale;
		Color base_color;
		Color max_color;
		std::string_view band_id;
	};

	struct model_config
	{
		int radius;
		Color background_color;
		Point center;
	};
	class model
	{
	public:
		// 这些是原始的输入数据
		model_config config;
		std::unordered_map<std::string_view, circle> circles;
		std::unordered_map<std::string_view, band> bands;
		std::unordered_map<std::string_view, circle_tick> circle_ticks;
		std::unordered_map<std::string_view, point_link> point_links;
		std::unordered_map<std::string_view, range_link> range_links;
		void to_shapes(shape_collection& pre_collection);
	};
}