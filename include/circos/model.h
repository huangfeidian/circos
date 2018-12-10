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
#include "shapes/tile.h"


namespace circos::model
{

	struct circle
	{
		std::string_view circle_id;
		int inner_radius;
		int outer_radius;
		int gap;
		Color fill_color;
		bool filled;
		float opacity;
		double angle_per_unit;

	};
	struct tile
	{
		std::string_view circle_id;
		std::string_view tile_id;
		int width;
		Color fill_color;
		float opacity;
		bool is_fill;
		int sequence; // must not be duplicated
		//calculated
		float angle_begin;
	};
	struct circle_tick
	{
		// 处理圆环上的刻度
		std::string_view circle_tick_id;
		std::string_view circle_id;
		int gap;
		Color fill_color;
		int width;
		int height;
		float opacity;
	};

	struct point_link
	{
		// 两点之间的连线
		std::string_view link_id;
		std::string_view from_tile_id;
		int from_pos_idx;
		std::string_view to_tile_id;
		int to_pos_idx;
		float control_radius_percent; //negative for direct link
		Color fill_color;
		float opacity;
		int width;

	};
	struct range_link
	{
		std::string_view link_id;
		std::string_view from_tile_id;
		int from_pos_begin_idx;
		int from_pos_end_idx;
		std::string_view to_tile_id;
		int to_pos_begin_idx;
		int to_pos_end_idx;
		bool is_cross;
		Color fill_color;
		float opacity;
		float control_radius_percent;

	};

	struct fill_ontile
	{
		std::string_view tile_id;
		int on_tile_begin;
		int on_tile_end;
		Color fill_color;
	};

	struct text_ontile:public fill_ontile
	{
		std::string_view text;
		int font_size;
		std::string_view font_name;
		text_align_type align_type;
	};
	struct tick_ontile: public fill_ontile
	{
		int tick_gap;
		int width;
		int height;
	};
	struct value_ontile: public fill_ontile
	{
		float value;
	};
	
	struct value_ontile_config
	{
		circos::value_on_tile_draw_type draw_type;
		float base_value;
		float max_value;
		float scale;
		Color base_color;
		Color max_color;
		std::string_view tile_id;
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
		std::unordered_map<std::string_view, tile> tiles;
		std::unordered_map<std::string_view, circle_tick> circle_ticks;
		std::unordered_map<std::string_view, point_link> point_links;
		std::unordered_map<std::string_view, range_link> range_links;
		void to_shapes(shape_collection& pre_collection);
	};
}