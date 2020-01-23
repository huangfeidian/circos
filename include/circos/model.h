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


namespace spiritsaway::circos::model
{

	struct circle
	{
		std::string_view circle_id;
		std::uint32_t inner_radius;
		std::uint32_t outer_radius;
		std::uint32_t gap;
		Color fill_color;
		bool filled;
		float opacity;
		float angle_per_unit;

	};
	struct tile
	{
		std::string_view circle_id;
		std::string_view tile_id;
		std::uint32_t width;
		Color fill_color;
		float opacity;
		bool is_fill;
		std::uint32_t sequence; // must not be duplicated
		//calculated
		float angle_begin;
	};
	struct circle_tick
	{
		// 处理圆环上的刻度
		std::string_view circle_tick_id;
		std::string_view circle_id;
		std::uint32_t gap;
		Color fill_color;
		std::uint32_t width;
		std::uint32_t height;
		float opacity;
	};

	struct tile_pos
	{
		// 贴片上的一个位置
		std::string_view tile_id;
		std::uint32_t pos_idx;
	};

	struct tile_region
	{
		// 贴片上的一段区域
		std::string_view tile_id;
		std::uint32_t begin_pos;
		std::uint32_t end_pos;
	};

	struct point_link
	{
		// 两点之间的连线 可能是贝塞尔曲线 也可能是直线
		std::string_view link_id;
		tile_pos from;
		tile_pos to;
		float control_radius_percent; //negative for direct link
		Color fill_color;
		float opacity;
		std::uint32_t width;

	};
	struct line_text
	{
		// 一条直线上的文本
		// 如果初始位置与结束位置相同 则代表此位置的顺时针切线方向
		std::string_view line_text_id;
		tile_pos from;
		tile_pos to;
		std::string_view utf8_text;
		std::string_view font_name;
		std::uint16_t font_size;
		Color fill_color;
		float opacity;
	};

	struct range_link
	{
		// 交叉或者不交叉的条带 
		std::string_view link_id;
		tile_region from;
		tile_region to;
		bool is_cross;
		Color fill_color;
		float opacity;
		float control_radius_percent;

	};
	struct tick_on_tile
	{
		std::string_view tick_tile_id;
		std::string_view tile_id;
		std::uint32_t gap;
		std::uint32_t height;
		std::uint32_t width;
		Color fill_color;
		float opacity;
	};
	struct fill_ontile
	{
		tile_region range;
		Color fill_color;
	};

	struct value_on_tile
	{
		std::string_view data_id;
		std::string_view track_id;
		std::string_view tile_id;
		std::uint32_t begin_pos;
		std::uint32_t end_pos;
		float data_value;
		float angle_begin;
		float angle_end;
		float data_percentage;
	};
	struct track_config
	{
		std::string_view track_id;
		std::string_view circle_id;
		std::pair<Color, Color> clamp_color;
		std::pair<float, float> clamp_data_value;
		std::pair<std::uint32_t, std::uint32_t> radius_offset;
		bool with_shadow;
		track_draw_type draw_type;
	};


	struct model_config
	{
		std::uint32_t radius;
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
		std::unordered_map<std::string_view, track_config> track_configs;
		void to_shapes(shape_collection& pre_collection);
		std::unordered_map<std::string_view, std::uint32_t> circle_ranges;
	private:

		void to_shapes(shape_collection& pre_collection, const track_config& config, std::vector<value_on_tile>& data);
		void to_shapes(shape_collection& pre_collection, const tick_on_tile& data);
		void to_shapes(shape_collection& pre_collection, const point_link& data);
		void to_shapes(shape_collection& pre_collection, const line_text& data);
		void to_shapes(shape_collection& pre_collection, const range_link& data);
		void to_shapes(shape_collection& pre_collection, const circle_tick& data);
		void to_shapes(shape_collection& pre_collection, const tile& data);
		void to_shapes(shape_collection& pre_collection, const circle& data);
		void draw_tiles(shape_collection& pre_collection);
		void draw_tiles_in_circle(shape_collection& pre_collection, circle& cur_circle, std::vector<tile>& tiles_in_circle);
		void prepare_value_on_track(const track_config& config, std::vector<value_on_tile>& one_track_data);
	};
}