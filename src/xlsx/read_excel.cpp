#include <optional>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <tuple>
#include <iostream>

#include <magic_enum.hpp>

#include <circos/model.h>
#include <circos/drawer/png_drawer.h>
#include <circos/drawer/svg_drawer.h>
#include <circos/xlsx/read_excel.h>

#include <xlsx_reader/xlsx_typed_worksheet.h>
#include <xlsx_reader/xlsx_typed_cell.h>
#include <xlsx_reader/xlsx_workbook.h>
namespace 
{
	using namespace spiritsaway::xlsx_reader;
	using namespace std;
	using namespace spiritsaway::circos;
	using namespace spiritsaway::container;
	enum class sheet_type
	{
		config,
		font_info,
		colors,
		circle,
		tile,
		fill_ontile,
		path_text,
		tick_on_tile,
		value_on_tile,
		point_link,
		range_link,
		circle_tick,
		track_config,
	};
	std::optional<Color> read_ref_color(const std::string& color_name, const std::unordered_map<std::string, Color>& defined_colors)
	{
		auto cur_iter = defined_colors.find(color_name);
		if (cur_iter == defined_colors.end())
		{
			return {};
		}
		return cur_iter->second;
		
	}

	std::optional<Point> read_point_from_cell(spiritsaway::memory::arena& temp_arena, const typed_worksheet& cur_worksheet, const typed_cell& cell_value)
	{
		if (!cell_value.cur_arena_typed_value)
		{
			return std::nullopt;
		}

		auto opt_point = cell_value.expect_value<std::tuple<int, int>>();
		if(!opt_point)
		{
			return std::nullopt;
		}
		else
		{
			auto real_point = opt_point.value();
			return Point(std::get<0>(real_point), std::get<1>(real_point));
		}
	}

	void read_circle_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, model::circle>& all_circles, const std::unordered_map<std::string, Color>& defined_colors)
	{
		// circle headers circle_id(string) inner_radius(int) outer_radius(int) gap(int) color(RGB) ref_color(ref) opacity(float) filled(bool)
		std::unordered_map<string, const typed_header*> sheet_headers;
		auto circle_id_header = 
			sheet_headers["circle_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "circle_id", "");
		sheet_headers["inner_radius"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "inner_radius", "");
		sheet_headers["outer_radius"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "outer_radius", "");
		
		auto color_type_detail = std::make_shared<typed_string_desc>(basic_value_type::number_uint);
		sheet_headers["color"] = new typed_header(std::make_shared<typed_string_desc>(color_type_detail, 3), "color", "");
		sheet_headers["ref_color"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "ref_color", "");

		sheet_headers["opacity"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "opacity", "");

		sheet_headers["gap"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "gap", "");
		sheet_headers["filled"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_bool), "filled", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "circle_id");
		
		if(!header_match)
		{
			std::cerr<<"header for circle description mismatch for sheet "<<current_sheet.m_name<<std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "circle_id", "inner_radius", "outer_radius", "color", "ref_color", "opacity", "gap", "filled" };
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			model::circle cur_circle;
			auto [opt_cirlce_id, opt_inner_radius, opt_outer_radius, opt_color, opt_ref_color, opt_opacity, opt_gap, opt_filled] =
				current_sheet.try_convert_row<std::string, std::uint32_t, std::uint32_t, tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string, float, std::uint32_t, bool>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!(opt_cirlce_id && opt_inner_radius && opt_outer_radius && opt_opacity && opt_gap && opt_filled))
			{
				continue;
			}
			cur_circle.circle_id = opt_cirlce_id.value();
			cur_circle.inner_radius = opt_inner_radius.value();
			cur_circle.outer_radius = opt_outer_radius.value();
			cur_circle.opacity = opt_opacity.value();
			cur_circle.gap = opt_gap.value();
			cur_circle.filled = opt_filled.value();

			if (opt_color)
			{
				auto color_value = opt_color.value();
				cur_circle.fill_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_ref_color)
			{
				auto temp_color = read_ref_color(opt_ref_color.value(), defined_colors);
				if (temp_color)
				{
					cur_circle.fill_color = temp_color.value();
				}
			}
			if (cur_circle.circle_id.empty())
			{
				std::cerr << "cant find circle for row " << std::endl;
				continue;
			}
			if (cur_circle.inner_radius >= cur_circle.outer_radius)
			{
				swap(cur_circle.inner_radius, cur_circle.outer_radius);
			}
			if (all_circles.find(cur_circle.circle_id) != all_circles.end())
			{
				std::cerr << "duplicated circle_id " << cur_circle.circle_id << std::endl;
			}
			else
			{
				all_circles[cur_circle.circle_id] = cur_circle;
			}
		}
	} 
	void read_tile_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, model::tile>& all_tiles, const std::unordered_map<std::string, Color>& defined_colors)
	{
		// tile_desc headers tile_id(string) circle_id(string)  width(int) color(RGB) ref_color(ref) opacity(float) sequence(int) filled(bool)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["circle_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "circle_id", "");

		sheet_headers["tile_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "tile_id", "");

		sheet_headers["width"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "width", "");

		sheet_headers["sequence"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "sequence", "");
		
		auto color_type_detail = std::make_shared<typed_string_desc>(basic_value_type::number_uint);
		sheet_headers["color"] = new typed_header(std::make_shared<typed_string_desc>(color_type_detail, 3), "color", "");
		sheet_headers["ref_color"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "ref_color", "");

		sheet_headers["opacity"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "opacity", "");

		sheet_headers["filled"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_bool), "filled", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "tile_id");
		if(!header_match)
		{
			std::cerr<<"header for tile description mismatch for sheet "<<current_sheet.m_name<<std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "tile_id", "circle_id", "width", "sequence", "color", "ref_color", "opacity", "filled" };
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			
			model::tile cur_tile;

			auto[opt_tile_id, opt_circle_id, opt_width, opt_seq, opt_color, opt_ref_color, opt_opacity, opt_filled] = 
				current_sheet.try_convert_row<std::string, std::string, std::uint32_t, std::uint32_t, tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string, float, bool>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!(opt_tile_id && opt_circle_id && opt_width && opt_seq && opt_opacity && opt_filled))
			{
				continue;
			}
			cur_tile.tile_id = opt_tile_id.value();
			cur_tile.circle_id = opt_circle_id.value();
			cur_tile.width = opt_width.value();
			cur_tile.sequence = opt_seq.value();
			cur_tile.opacity = opt_opacity.value();
			cur_tile.is_fill = opt_filled.value();

			if(opt_color)
			{
				auto color_value = opt_color.value();
				cur_tile.fill_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_ref_color)
			{
				auto temp_color = read_ref_color(opt_ref_color.value(), defined_colors);
				if (temp_color)
				{
					cur_tile.fill_color = temp_color.value();
				}
			}

			if(cur_tile.tile_id.empty())
			{
				std::cerr<<"cant find tile for row "<<std::endl;
				continue;
			}
			if(all_tiles.find(cur_tile.tile_id) != all_tiles.end())
			{
				std::cerr<<"duplicated tile_id "<<cur_tile.tile_id<<std::endl;
				continue;
			}
			if(cur_tile.circle_id.empty())
			{
				std::cerr<<"missing circle_id for tile "<<cur_tile.tile_id<<std::endl;
				continue;
			}

			all_tiles[cur_tile.tile_id] = cur_tile;
		}
	} 

	void read_circle_tick_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, model::circle_tick>& all_circle_ticks, const std::unordered_map<std::string, Color>& defined_colors)
	{
		// circle_tick headers tick_id(string) circle_id(string)  width(int) height(int) color(RGB) ref_color(ref) opacity(float)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["tick_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "tick_id", "");

		sheet_headers["circle_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "circle_id", "");

		sheet_headers["width"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "width", "");
		sheet_headers["height"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "height", "");
		sheet_headers["gap"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "gap", "");
		auto color_type_detail = std::make_shared<typed_string_desc>(basic_value_type::number_uint);
		sheet_headers["color"] = new typed_header(std::make_shared<typed_string_desc>(color_type_detail, 3), "color", "");
		sheet_headers["ref_color"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "ref_color", "");
		sheet_headers["opacity"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "opacity", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "tick_id");
		if(!header_match)
		{
			std::cerr<<"header for circle_tick description mismatch for sheet "<<current_sheet.m_name<<std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "tick_id", "circle_id", "width", "height", "color", "ref_color", "opacity", "gap"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			model::circle_tick cur_circle_tick;
			auto[opt_tick_id, opt_circle_id, opt_width, opt_height, opt_color, opt_ref_color, opt_opacity, opt_gap] =
				current_sheet.try_convert_row<std::string, std::string, std::uint32_t, std::uint32_t, tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string, float, std::uint32_t>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!(opt_tick_id && opt_circle_id && opt_width && opt_height && opt_opacity&& opt_gap))
			{
				continue;
			}

			cur_circle_tick.circle_tick_id = opt_tick_id.value();
			cur_circle_tick.circle_id = opt_circle_id.value();
			cur_circle_tick.height = opt_height.value();
			cur_circle_tick.opacity = opt_opacity.value();
			cur_circle_tick.width = opt_width.value();
			cur_circle_tick.gap = opt_gap.value();
			if (opt_color)
			{
				auto color_value = opt_color.value();
				cur_circle_tick.fill_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_ref_color)
			{
				auto temp_color = read_ref_color(opt_ref_color.value(), defined_colors);
				if (temp_color)
				{
					cur_circle_tick.fill_color = temp_color.value();
				}
			}
			if(cur_circle_tick.circle_id.empty())
			{
				std::cerr<<"cant find circle_tick for row "<<std::endl;
				continue;
			}
			if(all_circle_ticks.find(cur_circle_tick.circle_tick_id) != all_circle_ticks.end())
			{
				std::cerr<<"duplicated tile_id "<<cur_circle_tick.circle_tick_id<<std::endl;
				continue;
			}

			all_circle_ticks[cur_circle_tick.circle_tick_id] = cur_circle_tick;
		}
	} 

	void read_point_link_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, model::point_link>& all_point_links, const std::unordered_map<std::string, Color>& defined_colors)
	{
		// point_link headers link_id(string) from_tile_id(string) from_pos_idx(int) to_tile_id(string) to_pos_idx(int)  color(RGB) ref_color(ref) opacity(float)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["link_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "link_id", "");

		sheet_headers["from_tile_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "from_tile_id", "");

		sheet_headers["to_tile_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "to_tile_id", "");

		sheet_headers["from_pos_idx"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "from_pos_idx", "");
		sheet_headers["to_pos_idx"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "to_pos_idx", "");
		
		auto color_type_detail = std::make_shared<typed_string_desc>(basic_value_type::number_uint);
		sheet_headers["color"] = new typed_header(std::make_shared<typed_string_desc>(color_type_detail, 3), "color", "");
		sheet_headers["ref_color"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "ref_color", "");
		sheet_headers["opacity"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "opacity", "");

		sheet_headers["control_radius_percent"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "control_radius_percent", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "link_id");
		if(!header_match)
		{
			std::cerr<<"header for point_link description mismatch for sheet "<<current_sheet.m_name<<std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "link_id", "from_tile_id", "to_tile_id", "from_pos_idx", "to_pos_idx", "color", "ref_color", "opacity" , "control_radius_percent"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			
			model::point_link cur_point_link;
			cur_point_link.width = 1;
			auto[opt_link_id, opt_from_tile, opt_to_tile, opt_from_pos, opt_to_pos, opt_color, opt_ref_color, opt_opacity, opt_control] =
				current_sheet.try_convert_row<std::string, std::string, std::string, std::uint32_t, std::uint32_t, tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string, float, float>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!(opt_link_id && opt_from_tile && opt_from_pos && opt_to_tile && opt_to_pos && opt_opacity && opt_control))
			{
				continue;
			}
			cur_point_link.link_id = opt_link_id.value();
			cur_point_link.from.tile_id = opt_from_tile.value();
			cur_point_link.to.tile_id = opt_to_tile.value();
			cur_point_link.from.pos_idx = opt_from_pos.value();
			cur_point_link.to.pos_idx = opt_to_pos.value();
			cur_point_link.opacity = opt_opacity.value();
			cur_point_link.control_radius_percent = opt_control.value();
			if (opt_color)
			{
				auto color_value = opt_color.value();
				cur_point_link.fill_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_ref_color)
			{
				auto temp_color = read_ref_color(opt_ref_color.value(), defined_colors);
				if (temp_color)
				{
					cur_point_link.fill_color = temp_color.value();
				}
			}
			if(cur_point_link.link_id.empty())
			{
				std::cerr<<"cant find point link for row "<<std::endl;
				continue;
			}
			if(all_point_links.find(cur_point_link.link_id) != all_point_links.end())
			{
				std::cerr<<"duplicated link_id "<<cur_point_link.link_id<<std::endl;
				continue;
			}

			all_point_links[cur_point_link.link_id] = cur_point_link;
		}
	} 
	void read_path_text_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, model::path_text>& all_path_texts, const std::unordered_map<std::string, Color>& defined_colors)
	{
		// path_text headers path_text_id(string) tile_id(string) from_pos_idx(int) to_pos_idx(int)  text(string) font_name(string) font_size(u32) color(RGB) ref_color(str) opacity(float) text_path_typep(choice_str) text_align_type(choice_str) offset(u32)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["path_text_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "path_text_id", "");

		sheet_headers["tile_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "tile_id", "");

		sheet_headers["text_path_type"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "text_path_type", "");

		sheet_headers["text_align_type"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "text_align_type", "");

		sheet_headers["begin_pos"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "begin_pos", "");
		sheet_headers["end_pos"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "end_pos", "");

		sheet_headers["text"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "text", "");
		sheet_headers["font_name"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "font_name", "");
		sheet_headers["font_size"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "font_size", "");

		sheet_headers["offset"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_int), "offset", "");

		auto color_type_detail = std::make_shared<typed_string_desc>(basic_value_type::number_uint);
		sheet_headers["color"] = new typed_header(std::make_shared<typed_string_desc>(color_type_detail, 3), "color", "");
		sheet_headers["ref_color"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "ref_color", "");
		sheet_headers["opacity"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "opacity", "");



		auto header_match = current_sheet.check_header_match(sheet_headers, "path_text_id");
		if (!header_match)
		{
			std::cerr << "header for point_link description mismatch for sheet " << current_sheet.m_name << std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "path_text_id", "tile_id", "text_path_type", "text_align_type", "begin_pos", "end_pos",  "text", "font_name", "font_size", "color", "ref_color", "opacity", "offset"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			
			model::path_text cur_path_text;

			auto[opt_path_id, opt_tile_id, opt_text_path_type, opt_text_align_type, opt_from_pos, opt_to_pos, opt_text, opt_font_name, opt_font_size, opt_color, opt_ref_color, opt_opacity, opt_offset] =
				current_sheet.try_convert_row<std::string, std::string, std::string, std::string, std::uint32_t, std::uint32_t, std::string, std::string, std::uint32_t, tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string, float, int>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!(opt_path_id && opt_tile_id && opt_from_pos && opt_to_pos && opt_text&& opt_font_name && opt_font_size&& opt_opacity && opt_text_path_type && opt_text_align_type))
			{
				continue;
			}
			cur_path_text.path_text_id = opt_path_id.value();
			cur_path_text._on_path = magic_enum::enum_cast<text_type>(opt_text_path_type.value()).value_or(text_type::normal);

			cur_path_text._align = magic_enum::enum_cast<text_align_type>(opt_text_align_type.value()).value_or(text_align_type::left);

			cur_path_text._region.tile_id = opt_tile_id.value();
			cur_path_text._region.begin_pos = opt_from_pos.value();
			cur_path_text._region.end_pos = opt_to_pos.value();
			cur_path_text.utf8_text = opt_text.value();
			cur_path_text.font_name = opt_font_name.value();
			cur_path_text.font_size = static_cast<std::uint16_t>(opt_font_size.value());
			cur_path_text.offset = opt_offset.value_or(0);
			cur_path_text.opacity = opt_opacity.value();
			if (opt_color)
			{
				auto color_value = opt_color.value();
				cur_path_text.fill_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_ref_color)
			{
				auto temp_color = read_ref_color(opt_ref_color.value(), defined_colors);
				if (temp_color)
				{
					cur_path_text.fill_color = temp_color.value();
				}
			}
			if (cur_path_text.path_text_id.empty())
			{
				std::cerr << "cant find path_text for row " << std::endl;
				continue;
			}
			if (all_path_texts.find(cur_path_text.path_text_id) != all_path_texts.end())
			{
				std::cerr << "duplicated path_text_id " << cur_path_text.path_text_id << std::endl;
				continue;
			}
			

			all_path_texts[cur_path_text.path_text_id] = cur_path_text;
		}
	}
	void read_range_link_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, model::range_link>& all_range_links, const std::unordered_map<std::string, Color>& defined_colors)
	{
		// point_link headers link_id(string) from_tile_id(string) from_pos_idx_begin(int) from_pos_idx_end(int) to_tile_id(string) to_pos_idx_begin(int)  to_pos_idx_end(int) color(RGB) ref_color(ref) opacity(float)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["link_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "link_id", "");

		sheet_headers["from_tile_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "from_tile_id", "");

		sheet_headers["to_tile_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "to_tile_id", "");

		sheet_headers["from_pos_idx_begin"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "from_pos_idx_begin", "");
		sheet_headers["to_pos_idx_begin"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "to_pos_idx_begin", "");

		sheet_headers["from_pos_idx_end"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "from_pos_idx_end", "");
		sheet_headers["to_pos_idx_end"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "to_pos_idx_end", "");
		
		auto color_type_detail = std::make_shared<typed_string_desc>(basic_value_type::number_uint);
		sheet_headers["color"] = new typed_header(std::make_shared<typed_string_desc>(color_type_detail, 3), "color", "");
		sheet_headers["ref_color"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "ref_color", "");

		sheet_headers["opacity"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "opacity", "");

		sheet_headers["is_cross"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_bool), "is_cross", "");
		
		sheet_headers["control_radius_percent"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "control_radius_percent", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "link_id");
		if(!header_match)
		{
			std::cerr<<"header for range_link description mismatch for sheet "<<current_sheet.m_name<<std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "link_id", "from_tile_id", "to_tile_id", "from_pos_idx_begin", "to_pos_idx_begin", "from_pos_idx_end", "to_pos_idx_end", "color", "ref_color", "opacity" , "control_radius_percent" , "is_cross"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			model::range_link cur_range_link;
			auto[opt_link_id, opt_from_tile, opt_to_tile, opt_from_pos_begin, opt_to_pos_begin, opt_from_pos_end, opt_to_pos_end, opt_color, opt_ref_color, opt_opacity, opt_control, opt_cross] =
				current_sheet.try_convert_row<std::string, std::string, std::string, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t,  tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string, float, float, bool>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!(opt_link_id && opt_from_tile && opt_to_tile && opt_from_pos_begin && opt_from_pos_end && opt_to_pos_begin && opt_to_pos_end && opt_opacity && opt_opacity && opt_cross))
			{
				continue;
			}
			cur_range_link.link_id = opt_link_id.value();
			cur_range_link.from.tile_id = opt_from_tile.value();
			cur_range_link.from.begin_pos = opt_from_pos_begin.value();
			cur_range_link.to.begin_pos = opt_to_pos_begin.value();
			cur_range_link.from.end_pos = opt_from_pos_end.value();
			cur_range_link.to.end_pos = opt_to_pos_end.value();
			cur_range_link.opacity = opt_opacity.value();
			cur_range_link.control_radius_percent = opt_control.value();
			cur_range_link.is_cross = opt_cross.value();
			if (opt_color)
			{
				auto color_value = opt_color.value();
				cur_range_link.fill_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_ref_color)
			{
				auto temp_color = read_ref_color(opt_ref_color.value(), defined_colors);
				if (temp_color)
				{
					cur_range_link.fill_color = temp_color.value();
				}
			}
			if(cur_range_link.link_id.empty())
			{
				std::cerr<<"cant find point link for row "<<std::endl;
				continue;
			}
			if(all_range_links.find(cur_range_link.link_id) != all_range_links.end())
			{
				std::cerr<<"duplicated link_id "<<cur_range_link.link_id<<std::endl;
				continue;
			}

			all_range_links[cur_range_link.link_id] = cur_range_link;
		}
	} 

	void read_value_on_tile_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, std::vector<model::value_on_tile>>& all_value_on_tile_by_track, const std::unordered_map<std::string, Color>& defined_colors)
	{
		// value_on_tile headers value_id(string) track_id(string)  tile_id(string) begin_pos(int) end_pos(int)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["value_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "value_id", "");
		sheet_headers["track_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "track_id", "");
		sheet_headers["tile_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "tile_id", "");

		sheet_headers["begin_pos"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "begin_pos", "");

		sheet_headers["end_pos"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "end_pos", "");
		sheet_headers["data_value"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "data_value", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "value_id");
		if (!header_match)
		{
			std::cerr << "header for value_on_tile description mismatch for sheet " << current_sheet.m_name << std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "value_id", "track_id", "tile_id", "begin_pos", "end_pos", "data_value"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			auto[opt_data_id, opt_track_id, opt_tile_id, opt_begin_pos, opt_end_pos, opt_data_value] =
				current_sheet.try_convert_row<std::string, std::string, std::string, std::uint32_t, std::uint32_t, float>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!(opt_data_id && opt_track_id && opt_tile_id && opt_begin_pos && opt_end_pos && opt_data_value))
			{
				continue;
			}
			model::value_on_tile cur_value_on_tile;
			cur_value_on_tile.data_id = opt_data_id.value();
			cur_value_on_tile.track_id = opt_track_id.value();
			cur_value_on_tile.tile_id = opt_tile_id.value();
			cur_value_on_tile.begin_pos = opt_begin_pos.value();
			cur_value_on_tile.end_pos = opt_end_pos.value();
			cur_value_on_tile.data_value = opt_data_value.value();
			auto& pre = all_value_on_tile_by_track[cur_value_on_tile.track_id];
			pre.push_back(cur_value_on_tile);

		}
	}

	void read_track_config_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, model::track_config>& track_config, const std::unordered_map<std::string, Color>& defined_colors)
	{
		// point_track_config headers track_id(string) circle_id(string) draw_type(int) min_data_value(float) max_data_value(float)  radius_offset(int) min_color(RGB) max_color(RGB) min_color_ref(ref) max_color_ref(ref)  fixed_size(uint32_t)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["track_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "track_id", "");

		sheet_headers["circle_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "circle_id", "");

		sheet_headers["min_data_value"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "min_data_value", "");
		sheet_headers["max_data_value"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_float), "max_data_value", "");
		

		sheet_headers["radius_offset_min"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_int), "radius_offset_min", "");
		sheet_headers["radius_offset_max"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_int), "radius_offset_max", "");

		sheet_headers["fixed_size"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "fixed_size", "");

		auto color_type_detail = std::make_shared<typed_string_desc>(basic_value_type::number_uint);

		sheet_headers["min_color"] = new typed_header(std::make_shared<typed_string_desc>(color_type_detail, 3), "min_color", "");
		sheet_headers["min_color_ref"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "min_color_ref", "");

		sheet_headers["max_color"] = new typed_header(std::make_shared<typed_string_desc>(color_type_detail, 3), "max_color", "");
		sheet_headers["max_color_ref"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "max_color_ref", "");
		sheet_headers["draw_type"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "draw_type", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "track_id");
		if (!header_match)
		{
			std::cerr << "header for point_track_config description mismatch for sheet " << current_sheet.m_name << std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "track_id", "circle_id", "draw_type", "min_data_value", "max_data_value", "radius_offset_min", "radius_offset_max", "min_color", "max_color", "min_color_ref", "max_color_ref", "fixed_size"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		uint32_t min_color_ref_idx = header_indexes[9];
		uint32_t max_color_ref_idx = header_indexes[10];
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			
			auto [opt_track_id, opt_circle_id, opt_draw_type, opt_min_value, opt_max_value,opt_radius_offset_min, opt_radius_offset_max, opt_min_color, opt_max_color, opt_min_color_ref, opt_max_color_ref, opt_fixed_size] =
				current_sheet.try_convert_row<std::string, std::string, std::string, float, float, int, int, tuple<int, int,int>, tuple<std::uint32_t, std::uint32_t, std::uint32_t>, std::string, std::string, std::uint32_t>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!(opt_track_id && opt_circle_id && opt_draw_type && opt_min_value && opt_max_value && opt_radius_offset_min && opt_radius_offset_max))
			{
				continue;
			}
			model::track_config cur_track_config;
			auto opt_draw_type_enum = magic_enum::enum_cast<track_draw_type>(opt_draw_type.value());
			if (!opt_draw_type_enum)
			{
				return;
			}
			cur_track_config.draw_type = opt_draw_type_enum.value();
			cur_track_config.track_id = opt_track_id.value();
			cur_track_config.circle_id = opt_circle_id.value();
			cur_track_config.fixed_size = opt_fixed_size.value_or(0);
			cur_track_config.clamp_data_value = std::make_pair(opt_min_value.value(), opt_max_value.value());
			cur_track_config.radius_offset = std::make_pair(opt_radius_offset_min.value(), opt_radius_offset_max.value());
			Color min_color, max_color;
			if (opt_min_color)
			{
				auto color_value = opt_min_color.value();
				min_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_min_color_ref)
			{
				auto temp_color = read_ref_color(opt_min_color_ref.value(), defined_colors);
				if (temp_color)
				{
					min_color = temp_color.value();
				}
			}
			if (opt_max_color)
			{
				auto color_value = opt_max_color.value();
				max_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_max_color_ref)
			{
				auto temp_color = read_ref_color(opt_max_color_ref.value(), defined_colors);
				if (temp_color)
				{
					max_color = temp_color.value();
				}
			}
			cur_track_config.clamp_color = std::make_pair(min_color, max_color);
			

			if (track_config.find(cur_track_config.track_id) != track_config.end())
			{
				std::cerr << "duplicate point track config id " << cur_track_config.track_id << std::endl;
				continue;
			}
			track_config[cur_track_config.track_id] = cur_track_config;
		}
	}

	unordered_map<std::string, json> get_config_values_from_sheet(spiritsaway::memory::arena& temp_arena, const typed_worksheet& config_sheet)
	{
		// config_key(str), config_value(str), config_value_type(str)
		unordered_map<std::string, json> config_values;
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["config_key"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "config_key", "");
		sheet_headers["config_value"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::any), "config_value", "");
		sheet_headers["config_value_type"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::any), "config_value_type", "");
		auto header_match = config_sheet.check_header_match(sheet_headers, "config_key");
		if(!header_match)
		{
			std::cerr<<"header for model_config description mismatch for sheet "<<config_sheet.m_name<<std::endl;
			return config_values;
		}
		
		const auto& all_headers = config_sheet.get_typed_headers();
		auto cur_row_index = config_sheet.value_begin_row();
		while (cur_row_index <= config_sheet.max_rows)
		{
			std::string cur_config_key;
			json cur_config_value, cur_config_value_type;
			auto& one_row = config_sheet.get_row(cur_row_index);
			
			for(std::uint32_t j = 1; j< one_row.size(); j++)
			{
				uint32_t column_idx = j;
				auto temp_value = config_sheet.get_typed_cell_value(cur_row_index, column_idx);
				string current_header_name = all_headers[column_idx].header_name;
				if(current_header_name == "config_key")
				{
					if(!temp_value.is_string())
					{
						cout<<"no config key found"<<endl;
						break;
					}
					cur_config_key = temp_value.get<std::string>();
				}
				else if(current_header_name == "config_value")
				{
					if (temp_value.is_null())
					{
						cout<<"no config value found"<<endl;
						break;
					}
					cur_config_value = temp_value;
				}
				else if(current_header_name == "config_value_type")
				{
					if (temp_value.is_null())
					{
						cout<<"no opt_type value found"<<endl;
						break;
					}
					cur_config_value_type = temp_value;
				}

			}
			string empty_key;
			if(cur_config_key.empty())
			{
				empty_key = "config_key";
			}
			if(!empty_key.empty())
			{
				cout<<"value for "<< empty_key <<" is empty"<<endl;
				continue;
			}
			if (cur_config_value.is_null() || cur_config_value_type.is_null())
			{
				cout << "config value is " << cur_config_value << " type is " << cur_config_value_type << std::endl;
				continue;
			}
			auto current_parsed_type = typed_string_desc::get_type_from_json(cur_config_value_type);
			if (!current_parsed_type)
			{
				cout << "fail co convert config type " << cur_config_value_type << std::endl;
				continue;
			}
			if(!current_parsed_type->validate(cur_config_value))
			{
				cout<<"cant parse "<< cur_config_value << " for type "<< cur_config_value_type<<endl;
				continue;
			}
			config_values[cur_config_key] = cur_config_value;
			cur_row_index++;
		}
		return config_values;
	}
	void read_font_info(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<std::string, std::pair<std::string, std::string>>& fonts_info)
	{
		// font info headers font_id(str) font_file_path(str) font_web_name(str)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["font_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "font_id", "");

		sheet_headers["font_file_path"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "font_file_path", "");

		sheet_headers["font_web_name"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "font_web_name", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "font_id");
		if (!header_match)
		{
			std::cerr << "header for fonts_info description mismatch for sheet " << current_sheet.m_name << std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "font_id", "font_file_path", "font_web_name"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			model::range_link cur_range_link;
			auto[opt_font_id, opt_font_path, opt_font_name] =
				current_sheet.try_convert_row<std::string, std::string, std::string>(cur_row_index, header_indexes);
			cur_row_index++;
			if (!opt_font_id || !opt_font_name || !opt_font_path)
			{
				continue;
			}
			fonts_info[opt_font_id.value()] = std::make_pair(opt_font_path.value(), opt_font_name.value());

		}


	}
	bool convert_config_to_model(unordered_map<std::string, json> config_values, model::model_config& cur_config)
	{
		model::model_config new_model_config;
		auto radius_value = config_values["radius"];
		if(radius_value.is_number_integer())
		{

			new_model_config.radius = radius_value.get<int>();
			new_model_config.center = Point(new_model_config.radius, new_model_config.radius);
		}
		else
		{
			return false;
		}
		auto background_color_value = config_values["background_color"];
		if(background_color_value.is_array())
		{
			std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> cur_background_color;
			if(!spiritsaway::serialize::decode(background_color_value, cur_background_color))
			{
				return false;
			}
			
			new_model_config.background_color = Color(get<0>(cur_background_color), get<1>(cur_background_color), get<2>(cur_background_color)) ;
		}
		else
		{
			return false;
		}
		swap(new_model_config, cur_config);
		return true;
	}
	void read_model_config(spiritsaway::memory::arena& temp_arena, const typed_worksheet& model_worksheet, model::model_config& cur_config)
	{
		auto config_map = get_config_values_from_sheet(temp_arena, model_worksheet);
		if(!convert_config_to_model(config_map, cur_config))
		{
			cout<<"cant convert config"<<endl;
		}
		
		config_map.clear();

	}
	void read_color_info(spiritsaway::memory::arena& temp_arena, const typed_worksheet& current_sheet, std::unordered_map<string, Color>& defined_colors)
	{
		// color info headers color_id(str) red(int) green(int) blue(int)

		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["color_id"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "color_id", "");

		sheet_headers["red"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "red", "");

		sheet_headers["green"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "green", "");

		sheet_headers["blue"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::number_uint), "blue", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "color_id");
		if (!header_match)
		{
			std::cerr << "header for color info description mismatch for sheet " << current_sheet.m_name << std::endl;
			return;
		}
		const auto& all_headers = current_sheet.get_typed_headers();
		vector<string> header_names = { "color_id", "red", "green" ,"blue"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		auto cur_row_index = current_sheet.value_begin_row();
		while (cur_row_index <= current_sheet.max_rows)
		{
			
			auto[opt_color_id, opt_red, opt_green, opt_blue] =
				current_sheet.try_convert_row<std::string, std::uint32_t, std::uint32_t, std::uint32_t>(cur_row_index ,header_indexes);
			cur_row_index++;
			if (!opt_color_id || !opt_red || !opt_green || !opt_blue)
			{
				continue;
			}
			defined_colors[opt_color_id.value()] = Color(opt_red.value(), opt_green.value(), opt_blue.value());
		}
	}
	void read_sheet_content_by_role(spiritsaway::memory::arena& temp_arena, sheet_type sheet_role, const typed_worksheet& sheet_content, model::model& in_model, std::unordered_map<std::string, Color>& defined_colors)
	{

		switch(sheet_role)
		{
		case sheet_type::config:
			read_model_config(temp_arena, sheet_content, in_model.config);
			break;
		case sheet_type::colors:
			read_color_info(temp_arena, sheet_content, defined_colors);
			break;
		case sheet_type::font_info:
			read_font_info(temp_arena, sheet_content, in_model.font_info);
			break;
		case sheet_type::circle:
			read_circle_sheet(temp_arena, sheet_content, in_model.circles, defined_colors);
			break;
		case sheet_type::tile:
			read_tile_sheet(temp_arena, sheet_content, in_model.tiles, defined_colors);
			break;
		case sheet_type::point_link:
			read_point_link_sheet(temp_arena, sheet_content, in_model.point_links, defined_colors);
			break;
		case sheet_type::range_link:
			read_range_link_sheet(temp_arena, sheet_content, in_model.range_links, defined_colors);
			break;
		case sheet_type::circle_tick:
			read_circle_tick_sheet(temp_arena, sheet_content, in_model.circle_ticks, defined_colors);
			break;
		case sheet_type::path_text:
			read_path_text_sheet(temp_arena, sheet_content, in_model.path_texts, defined_colors);
			break;
		case sheet_type::value_on_tile:
			read_value_on_tile_sheet(temp_arena, sheet_content, in_model.all_value_on_tile_by_track, defined_colors);
			break;
		case sheet_type::track_config:
			read_track_config_sheet(temp_arena, sheet_content, in_model.track_configs, defined_colors);
			break;
		default:
			break;
		}
	}
	void read_role_sheet(const typed_worksheet& role_sheet, unordered_map<sheet_type, std::vector<string>>& sheet_role_map)
	{
		// headers sheet_name(string) role(string)
		std::unordered_map<string, const typed_header*> sheet_headers;
		sheet_headers["sheet_name"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "sheet_name", "");

		sheet_headers["role"] = new typed_header(std::make_shared<typed_string_desc>(basic_value_type::str), "role", "");
		auto header_match = role_sheet.check_header_match(sheet_headers, "sheet_name");
		if(!header_match)
		{
			cout<<"header not match for role_sheet"<<endl;
			return;
		}
		const auto& all_headers = role_sheet.get_typed_headers();
		auto cur_row_index = role_sheet.value_begin_row();
		while (cur_row_index <= role_sheet.max_rows)
		{
			auto& one_row = role_sheet.get_row(cur_row_index);
			string cur_sheet_name, cur_sheet_role;
			for(std::uint32_t j = 1; j< one_row.size(); j++)
			{
				auto cur_cell_value = role_sheet.get_typed_cell_value(cur_row_index, j);
				
				uint32_t column_idx = j;
				string current_header_name = all_headers[column_idx].header_name;
				if(current_header_name == "sheet_name")
				{
					if(!cur_cell_value.is_string())
					{
						break;
					}
					cur_sheet_name = cur_cell_value.get<std::string>();
				}
				else if(current_header_name == "role")
				{
					if (!cur_cell_value.is_string())
					{
						break;
					}
					cur_sheet_role = cur_cell_value.get<std::string>();
					
				}
			}
			cur_row_index++;
			if(cur_sheet_name.empty() || cur_sheet_role.empty())
			{
				continue;
			}
			auto cur_sheet_type_opt = magic_enum::enum_cast<sheet_type>(cur_sheet_role);
			if (!cur_sheet_type_opt)
			{
				continue;
			}
			auto& pre = sheet_role_map[cur_sheet_type_opt.value()];
			pre.push_back(cur_sheet_name);
		}
	}
	
}
namespace spiritsaway::circos
{
	using namespace xlsx_reader;
	using namespace std;
	model::model read_model_from_workbook(const workbook<typed_worksheet>& in_workbook)
	{
		spiritsaway::memory::arena temp_arena(4 * 1024);
		model::model result;
		auto role_sheet_idx = in_workbook.get_sheet_index_by_name("sheet_role");
		if(!role_sheet_idx)
		{
			cout<<"cant find role sheet"<<endl;
			return result;
		}
		unordered_map<std::string, Color> defined_colors;
		const auto& role_sheet = in_workbook.get_worksheet(role_sheet_idx.value());
		unordered_map<sheet_type, std::vector<string>> sheet_roles;
		read_role_sheet(role_sheet, sheet_roles);
		int config_count = 0;
		for (auto one_sheet_role : magic_enum::enum_values<sheet_type>())
		{
			auto sheets_iter = sheet_roles.find(one_sheet_role);
			if (sheets_iter == sheet_roles.end())
			{
				continue;
			}
			if (one_sheet_role == sheet_type::config && sheets_iter->second.size() != 1)
			{
				return model::model();
			}
			for (auto sheet_name : sheets_iter->second)
			{
				if (sheet_name == "sheet_role")
				{
					continue;
				}
				auto temp_sheet_idx = in_workbook.get_sheet_index_by_name(sheet_name);
				if (!temp_sheet_idx)
				{
					cout << "cant find sheet for " << sheet_name << endl;
					continue;
				}
				const auto& temp_sheet_content = in_workbook.get_worksheet(temp_sheet_idx.value());
				read_sheet_content_by_role(temp_arena, one_sheet_role, temp_sheet_content, result, defined_colors);
			}
		}

		return result;
		
	}
	void read_xlsx_and_draw(const string& xlsx_path, const string& png_output_file, const string& svg_output_file)
	{
		auto the_archive = make_shared<archive>(xlsx_path);
		if(!the_archive || !the_archive->is_valid())
		{
			return;
		}
		workbook<typed_worksheet> cur_workbook(the_archive);
		auto the_model = read_model_from_workbook(cur_workbook);
		if(the_model.config.radius == 0)
		{
			return;
		}
		std::unordered_map<string, pair<string, string>> font_info{ { "yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei") } };
		if(png_output_file.empty() && svg_output_file.empty())
		{
			return;
		}
		shape_collection cur_collection;
		the_model.to_shapes(cur_collection);
		if(!png_output_file.empty())
		{
			PngImage png_image(the_model.font_info, png_output_file, the_model.config.radius, the_model.config.background_color);
			draw_collections(png_image, cur_collection);
		}
		if(!svg_output_file.empty())
		{
			SvgGraph svg_graph(the_model.font_info, svg_output_file, the_model.config.radius, the_model.config.background_color);
			draw_collections(svg_graph, cur_collection);
		}

	}
}


