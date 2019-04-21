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
#include "shapes/line_text.h"


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
	struct line_text
	{
		// 一条直线上的文本
		// 如果初始位置与结束位置相同 则代表此位置的顺时针切线方向
		std::string_view line_text_id;
		std::string_view from_tile_id;
		int from_pos_idx;
		std::string_view to_tile_id;
		int to_pos_idx;
		std::string_view utf8_text;
		std::string_view font_name;
		std::uint16_t font_size;
		Color fill_color;
		float opacity;
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
	struct tick_on_tile
	{
		std::string_view tick_tile_id;
		std::string_view tile_id;
		int gap;
		int height;
		int width;
		Color fill_color;
		float opacity;
	};
	struct fill_ontile
	{
		std::string_view tile_id;
		int on_tile_begin;
		int on_tile_end;
		Color fill_color;
	};

	struct value_on_tile
	{
		std::string_view data_id;
		std::string_view track_id;
		std::string_view tile_id;
		int begin_pos;
		int end_pos;
		float data_value;
		float angle = 0;
	};
	struct point_track_config
	{
		std::string_view track_id;
		std::pair<Color, Color> clamp_color;
		std::pair<float, float> clamp_data_value;
		std::pair<int, int> clamp_point_size;
		std::pair<int, int> radius_offset;
		bool with_shadow;
		std::uint8_t link_width;
		Color link_color;
	};


	struct value_ontile_config
	{
		circos::value_on_tile_draw_type draw_type;
		float base_value;
		float max_value;
		float scale;
		Color base_color;
		Color max_color;
		std::uint32_t point_size;
		std::uint32_t stroke_size;

	};

	struct model_config
	{
		int radius;
		Color background_color;
		Point center;
		model_config()
		{
			radius = 0;
		}
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
		std::unordered_map<std::string_view, tick_on_tile> tile_ticks;
		std::unordered_map<std::string_view, line_text> line_texts;
		std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>> font_info;

		std::unordered_map<std::string_view, std::vector<value_on_tile>> all_value_on_tile_by_track;
		std::unordered_map<std::string_view, point_track_config> point_track_configs;
		void to_shapes(shape_collection& pre_collection);
		
	};
}