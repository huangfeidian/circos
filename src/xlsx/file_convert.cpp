#include <optional>
#include <vector>
#include <unordered_map>
#include <map>
#include <string_view>
#include <tuple>
#include <iostream>

#include <circos/model.h>
#include <circos/drawer/png_drawer.h>
#include <circos/drawer/svg_drawer.h>
#include <circos/xlsx/file_convert.h>

#ifdef WITH_XLSX_SUPPORT
#include <xlsx_reader/xlsx_typed_worksheet.h>
#include <xlsx_reader/xlsx_typed_cell.h>
#include <xlsx_reader/xlsx_workbook.h>

namespace 
{
	using namespace xlsx_reader;
	using namespace std;
	using namespace circos;
	enum class sheet_type
	{
		circle,
		tile,
		fill_ontile,
		lable_on_tile,
		tick_on_tile,
		value_on_tile,
		colors,
		point_link,
		range_link,
		config,
		circle_tick,
	};
	std::optional<Color> read_ref_color(const typed_worksheet& cur_worksheet, uint32_t ref_header_idx, string_view color_name)
	{
		auto ref_header = cur_worksheet.get_typed_headers()[ref_header_idx];
		if (!ref_header)
		{
			return nullopt;
		}
		if (color_name.empty())
		{
			return nullopt;
		}
		auto cur_type_detail = ref_header->type_desc->get_ref_detail_t();
		if (!cur_type_detail)
		{
			return std::nullopt;
		}
		auto ref_detail = cur_type_detail.value();

		auto[cur_wb_name, cur_ws_name, cur_ref_type] = ref_detail;
		typed_value cur_ref_value(color_name);
		const auto& row_value = cur_worksheet.get_ref_row(cur_ws_name, &cur_ref_value);
		if (row_value.size() != 5)
		{
			return nullopt;
		}

		vector<uint32_t> rgb_values;
		for (int i = 2; i < 5; i++)
		{

			const auto& cur_cell_value = row_value[i];
			auto cur_int_opt = cur_cell_value.expect_value<std::uint32_t>();
			if (!cur_int_opt)
			{
				return std::nullopt;
			}
			rgb_values.push_back(cur_int_opt.value());
		}
		return Color(rgb_values[0], rgb_values[1], rgb_values[2]);
	}

	std::optional<Point> read_point_from_cell(const typed_worksheet& cur_worksheet, const typed_cell& cell_value)
	{
		if (!cell_value.cur_typed_value)
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

	void read_circle_sheet(const typed_worksheet& current_sheet, std::unordered_map<std::string_view, model::circle>& all_circles)
	{
		// circle headers circle_id(string) inner_radius(int) outer_radius(int) gap(int) color(RGB) ref_color(ref) opacity(float) filled(bool)
		std::unordered_map<string_view, const typed_header*> sheet_headers;
		auto circle_id_header = 
		sheet_headers["circle_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "circle_id", "");
		sheet_headers["inner_radius"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "inner_radius", "");
		sheet_headers["outer_radius"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "outer_radius", "");
		
		auto color_type_detail = make_tuple(new typed_node_type_descriptor(basic_value_type::number_32), 3, ',');
		sheet_headers["color"] = new typed_header(new typed_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_float), "opacity", "");

		sheet_headers["gap"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "gap", "");
		sheet_headers["filled"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_bool), "filled", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "circle_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		
		if(!header_match)
		{
			std::cerr<<"header for circle description mismatch for sheet "<<current_sheet._name<<std::endl;
			return;
		}
		const vector<const typed_header*>& all_headers = current_sheet.get_typed_headers();
		vector<string_view> header_names = { "circle_id", "inner_radius", "outer_radius", "color", "ref_color", "opacity", "gap", "filled" };
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		uint32_t ref_color_idx = header_indexes[4];
		const auto& all_row_info = current_sheet.get_all_typed_row_info();
		for(int i=1; i< all_row_info.size(); i++)
		{
			model::circle cur_circle;
			auto[opt_cirlce_id, opt_inner_radius, opt_outer_radius, opt_color, opt_ref_color, opt_opacity, opt_gap, opt_filled] = 
				current_sheet.try_convert_row<string_view, int, int, tuple<int, int, int>, string_view, float, int, bool>(i, header_indexes);
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
				auto temp_color = read_ref_color(current_sheet
					, ref_color_idx, opt_ref_color.value());
				if (temp_color)
				{
					cur_circle.fill_color = temp_color.value();
				}
			}
			if(cur_circle.circle_id.empty())
			{
				std::cerr<<"cant find circle for row "<< i <<std::endl;
				continue;
			}
			if(cur_circle.inner_radius >= cur_circle.outer_radius)
			{
				swap(cur_circle.inner_radius, cur_circle.outer_radius);
			}
			if(all_circles.find(cur_circle.circle_id) != all_circles.end())
			{
				std::cerr<<"duplicated circle_id "<<cur_circle.circle_id<<std::endl;
			}
			else
			{
				all_circles[cur_circle.circle_id] = cur_circle;
			}
		}
	} 
	void read_tile_sheet(const typed_worksheet& current_sheet, std::unordered_map<std::string_view, model::tile>& all_tiles)
	{
		// tile_desc headers tile_id(string) circle_id(string)  width(int) color(RGB) ref_color(ref) opacity(float) sequence(int) filled(bool)
		std::unordered_map<string_view, const typed_header*> sheet_headers;
		sheet_headers["circle_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "circle_id", "");

		sheet_headers["tile_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "tile_id", "");

		sheet_headers["width"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "width", "");

		sheet_headers["sequence"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "sequence", "");
		
		auto color_type_detail = make_tuple(new typed_node_type_descriptor(basic_value_type::number_32), 3, ',');
		sheet_headers["color"] = new typed_header(new typed_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_float), "opacity", "");

		sheet_headers["filled"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_bool), "filled", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "tile_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for tile description mismatch for sheet "<<current_sheet._name<<std::endl;
			return;
		}
		const vector<const typed_header*>& all_headers = current_sheet.get_typed_headers();
		vector<string_view> header_names = { "tile_id", "circle_id", "width", "sequence", "color", "ref_color", "opacity", "filled" };
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		uint32_t ref_color_idx = header_indexes[5];
		const auto& all_row_info = current_sheet.get_all_typed_row_info();
		for(int i=1; i< all_row_info.size(); i++)
		{
			model::tile cur_tile;

			auto[opt_tile_id, opt_circle_id, opt_width, opt_seq, opt_color, opt_ref_color, opt_opacity, opt_filled] = 
				current_sheet.try_convert_row<string_view, string_view, int, int, tuple<int, int, int>, string_view, float, bool>(i, header_indexes);
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
				auto temp_color = read_ref_color(current_sheet
					, ref_color_idx, opt_ref_color.value());
				if (temp_color)
				{
					cur_tile.fill_color = temp_color.value();
				}
			}

			if(cur_tile.tile_id.empty())
			{
				std::cerr<<"cant find tile for row "<< i <<std::endl;
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

	void read_circle_tick_sheet(const typed_worksheet& current_sheet, std::unordered_map<std::string_view, model::circle_tick>& all_circle_ticks)
	{
		// circle_tick headers tick_id(string) circle_id(string)  width(int) height(int) color(RGB) ref_color(ref) opacity(float)
		std::unordered_map<string_view, const typed_header*> sheet_headers;
		sheet_headers["tick_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "tick_id", "");

		sheet_headers["circle_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "circle_id", "");

		sheet_headers["width"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "width", "");
		sheet_headers["height"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "height", "");
		sheet_headers["gap"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "gap", "");
		auto color_type_detail = make_tuple(new typed_node_type_descriptor(basic_value_type::number_32), 3, ',');
		sheet_headers["color"] = new typed_header(new typed_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_float), "opacity", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "circle_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for circle_tick description mismatch for sheet "<<current_sheet._name<<std::endl;
			return;
		}
		const vector<const typed_header*>& all_headers = current_sheet.get_typed_headers();
		vector<string_view> header_names = { "tick_id", "circle_id", "width", "height", "color", "ref_color", "opacity", "gap"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		uint32_t ref_color_idx = header_indexes[5];
		const auto& all_row_info = current_sheet.get_all_typed_row_info();
		for(int i = 1; i< all_row_info.size(); i++)
		{
			model::circle_tick cur_circle_tick;
			auto[opt_tick_id, opt_circle_id, opt_width, opt_height, opt_color, opt_ref_color, opt_opacity, opt_gap] =
				current_sheet.try_convert_row<string_view, string_view, int, int, tuple<int, int, int>, string_view, float, int>(i, header_indexes);
			if (!(opt_tick_id && opt_circle_id && opt_width && opt_height && opt_opacity&& opt_gap))
			{
				continue;
			}

			cur_circle_tick.circle_tick_id = opt_tick_id.value();
			cur_circle_tick.circle_id = opt_circle_id.value();
			cur_circle_tick.height = opt_height.value();
			cur_circle_tick.opacity = opt_opacity.value();
			cur_circle_tick.width = opt_width.value();
			cur_circle_tick.gap = opt_gap.value()
			if (opt_color)
			{
				auto color_value = opt_color.value();
				cur_circle_tick.fill_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_ref_color)
			{
				auto temp_color = read_ref_color(current_sheet
					, ref_color_idx, opt_ref_color.value());
				if (temp_color)
				{
					cur_circle_tick.fill_color = temp_color.value();
				}
			}
			if(cur_circle_tick.circle_id.empty())
			{
				std::cerr<<"cant find circle_tick for row "<< i <<std::endl;
				continue;
			}
			if(all_circle_ticks.find(cur_circle_tick.circle_tick_id) != all_circle_ticks.end())
			{
				std::cerr<<"duplicated tile_id "<<cur_circle_tick.circle_tick_id<<std::endl;
				continue;
			}

			all_circle_ticks[cur_circle_tick.circle_id] = cur_circle_tick;
		}
	} 

	void read_point_link_sheet(const typed_worksheet& current_sheet, std::unordered_map<std::string_view, model::point_link>& all_point_links)
	{
		// point_link headers link_id(string) from_tile_id(string) from_pos_idx(int) to_tile_id(string) to_pos_idx(int)  color(RGB) ref_color(ref) opacity(float)
		std::unordered_map<string_view, const typed_header*> sheet_headers;
		sheet_headers["link_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "link_id", "");

		sheet_headers["from_tile_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "from_tile_id", "");

		sheet_headers["to_tile_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "to_tile_id", "");

		sheet_headers["from_pos_idx"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "from_pos_idx", "");
		sheet_headers["to_pos_idx"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "to_pos_idx", "");
		
		auto color_type_detail = make_tuple(new typed_node_type_descriptor(basic_value_type::number_32), 3, ',');
		sheet_headers["color"] = new typed_header(new typed_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_float), "opacity", "");

		sheet_headers["control_radius_percent"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_float), "control_radius_percent", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "link_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for point_link description mismatch for sheet "<<current_sheet._name<<std::endl;
			return;
		}
		const vector<const typed_header*>& all_headers = current_sheet.get_typed_headers();
		vector<string_view> header_names = { "link_id", "from_tile_id", "to_tile_id", "from_pos_idx", "to_pos_idx", "color", "ref_color", "opacity" , "control_radius_percent"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		uint32_t ref_color_idx = header_indexes[6];
		const auto& all_row_info = current_sheet.get_all_typed_row_info();
		for (int i = 1; i < all_row_info.size(); i++)
		{
			model::point_link cur_point_link;
			cur_point_link.width = 1;
			auto[opt_link_id, opt_from_tile, opt_to_tile, opt_from_pos, opt_to_pos, opt_color, opt_ref_color, opt_opacity, opt_control] =
				current_sheet.try_convert_row<string_view, string_view, string_view, int, int, tuple<int, int, int>, string_view, float, float>(i, header_indexes);
			if (!(opt_link_id && opt_from_tile && opt_from_pos && opt_to_tile && opt_to_pos && opt_opacity && opt_control))
			{
				continue;
			}
			cur_point_link.link_id = opt_link_id.value();
			cur_point_link.from_tile_id = opt_from_tile.value();
			cur_point_link.to_tile_id = opt_to_tile.value();
			cur_point_link.from_pos_idx = opt_from_pos.value();
			cur_point_link.to_pos_idx = opt_to_pos.value();
			cur_point_link.opacity = opt_opacity.value();
			cur_point_link.control_radius_percent = opt_control.value();
			if (opt_color)
			{
				auto color_value = opt_color.value();
				cur_point_link.fill_color = Color(get<0>(color_value), get<1>(color_value), get<2>(color_value));
			}
			if (opt_ref_color)
			{
				auto temp_color = read_ref_color(current_sheet
					, ref_color_idx, opt_ref_color.value());
				if (temp_color)
				{
					cur_point_link.fill_color = temp_color.value();
				}
			}
			if(cur_point_link.link_id.empty())
			{
				std::cerr<<"cant find point link for row "<< i <<std::endl;
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

	void read_range_link_sheet(const typed_worksheet& current_sheet, std::unordered_map<std::string_view, model::range_link>& all_range_links)
	{
		// point_link headers link_id(string) from_tile_id(string) from_pos_idx_begin(int) from_pos_idx_end(int) to_tile_id(string) to_pos_idx_begin(int)  to_pos_idx_end(int) color(RGB) ref_color(ref) opacity(float)
		std::unordered_map<string_view, const typed_header*> sheet_headers;
		sheet_headers["link_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "link_id", "");

		sheet_headers["from_tile_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "from_tile_id", "");

		sheet_headers["to_tile_id"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "to_tile_id", "");

		sheet_headers["from_pos_idx_begin"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "from_pos_idx_begin", "");
		sheet_headers["to_pos_idx_begin"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "to_pos_idx_begin", "");

		sheet_headers["from_pos_idx_end"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "from_pos_idx_end", "");
		sheet_headers["to_pos_idx_end"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_32), "to_pos_idx_end", "");
		
		auto color_type_detail = make_tuple(new typed_node_type_descriptor(basic_value_type::number_32), 3, ',');
		sheet_headers["color"] = new typed_header(new typed_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_float), "opacity", "");

		sheet_headers["is_cross"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_bool), "is_cross", "");
		
		sheet_headers["control_radius_percent"] = new typed_header(new typed_node_type_descriptor(basic_value_type::number_float), "control_radius_percent", "");

		auto header_match = current_sheet.check_header_match(sheet_headers, "link_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for range_link description mismatch for sheet "<<current_sheet._name<<std::endl;
			return;
		}
		const vector<const typed_header*>& all_headers = current_sheet.get_typed_headers();
		vector<string_view> header_names = { "link_id", "from_tile_id", "to_tile_id", "from_pos_idx_begin", "to_pos_idx_begin", "from_pos_idx_end", "to_pos_idx_end", "color", "ref_color", "opacity" , "control_radius_percent" , "is_cross"};
		const vector<uint32_t>& header_indexes = current_sheet.get_header_index_vector(header_names);
		if (header_indexes.empty())
		{
			return;
		}
		uint32_t ref_color_idx = header_indexes[7];
		const auto& all_row_info = current_sheet.get_all_typed_row_info();
		for(int i = 1; i< all_row_info.size(); i++)
		{
			model::range_link cur_range_link;
			auto[opt_link_id, opt_from_tile, opt_to_tile, opt_from_pos_begin, opt_to_pos_begin, opt_from_pos_end, opt_to_pos_end, opt_color, opt_ref_color, opt_opacity, opt_control, opt_cross] =
				current_sheet.try_convert_row<string_view, string_view, string_view, int, int,int, int,  tuple<int, int, int>, string_view, float, float, bool>(i, header_indexes);
			if (!(opt_link_id && opt_from_tile && opt_to_tile && opt_from_pos_begin && opt_from_pos_end && opt_to_pos_begin && opt_to_pos_end && opt_opacity && opt_opacity && opt_cross))
			{
				continue;
			}
			cur_range_link.link_id = opt_link_id.value();
			cur_range_link.from_tile_id = opt_from_tile.value();
			cur_range_link.from_pos_begin_idx = opt_from_pos_begin.value();
			cur_range_link.to_pos_begin_idx = opt_to_pos_begin.value();
			cur_range_link.from_pos_end_idx = opt_from_pos_end.value();
			cur_range_link.to_pos_end_idx = opt_to_pos_end.value();
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
				auto temp_color = read_ref_color(current_sheet
					, ref_color_idx, opt_ref_color.value());
				if (temp_color)
				{
					cur_range_link.fill_color = temp_color.value();
				}
			}
			if(cur_range_link.link_id.empty())
			{
				std::cerr<<"cant find point link for row "<< i <<std::endl;
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

	unordered_map<string_view, typed_value*> get_config_values_from_sheet(const typed_worksheet& config_sheet)
	{

		// config_key(str), config_value(str), config_value_type(str)
		unordered_map<string_view, typed_value*> config_values;
		std::unordered_map<string_view, const typed_header*> sheet_headers;
		sheet_headers["config_key"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "config_key", "");
		sheet_headers["config_value"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "config_value", "");
		sheet_headers["config_value_type"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "config_value_type", "");
		auto header_match = config_sheet.check_header_match(sheet_headers, "config_key", std::vector<std::string_view>({}), std::vector<std::string_view>({}));
		if(!header_match)
		{
			std::cerr<<"header for model_config description mismatch for sheet "<<config_sheet._name<<std::endl;
			return config_values;
		}
		
		const vector<const typed_header*>& all_headers = config_sheet.get_typed_headers();
		const auto& all_row_info = config_sheet.get_all_typed_row_info();
		for(int i = 1; i< all_row_info.size(); i++)
		{
			string_view cur_config_key, cur_config_value, cur_config_value_type;
			for(int j = 1; j< all_row_info[i].size(); j++)
			{
				const auto& cur_cell_value = all_row_info[i][j];
				uint32_t column_idx = j;
				string_view current_header_name = all_headers[column_idx]->header_name;
				if(current_header_name == "config_key")
				{
					auto opt_config_key = cur_cell_value.expect_value<string_view>();
					if(!opt_config_key)
					{
						cout<<"no config key found"<<endl;
						break;
					}
					cur_config_key = opt_config_key.value();
				}
				else if(current_header_name == "config_value")
				{
					auto opt_config_value = cur_cell_value.expect_value<string_view>();
					if(!opt_config_value)
					{
						cout<<"no config value found"<<endl;
						break;
					}
					cur_config_value = opt_config_value.value();
				}
				else if(current_header_name == "config_value_type")
				{
					auto opt_type_value = cur_cell_value.expect_value<string_view>();
					if(!opt_type_value)
					{
						cout<<"no opt_type value found"<<endl;
						break;
					}
					cur_config_value_type = opt_type_value.value();
				}

			}
			string empty_key;
			if(cur_config_key.empty())
			{
				empty_key = "config_key";
			}
			if(cur_config_value.empty())
			{
				empty_key = "config_value";
			}
			if(cur_config_value_type.empty())
			{
				empty_key = "config_value_type";
			}
			if(!empty_key.empty())
			{
				cout<<"value for "<< empty_key <<" is empty"<<endl;
				continue;
			}
			auto current_parsed_type = typed_value_parser::parse_type(cur_config_value_type);
			auto current_parsed_value = typed_value_parser::parse_value_with_type(current_parsed_type, cur_config_value);
			if(!current_parsed_value)
			{
				cout<<"cant parse "<< cur_config_value << " for type "<< cur_config_value_type<<endl;
				if(current_parsed_type)
				{
					current_parsed_type->~typed_node_type_descriptor();
				}
				continue;
			}
			config_values[cur_config_key] = current_parsed_value;
		}
		return config_values;
	}
	bool convert_config_to_model(unordered_map<string_view, typed_value*> config_values, model::model_config& cur_config)
	{
		model::model_config new_model_config;
		auto radius_value_ptr = config_values["radius"];
		if(radius_value_ptr)
		{
			auto cur_radius_opt = radius_value_ptr->expect_value<int>();
			if(!cur_radius_opt)
			{
				return false;
			}
			new_model_config.radius = cur_radius_opt.value();
			new_model_config.center = Point(new_model_config.radius, new_model_config.radius);
		}
		else
		{
			return false;
		}
		auto background_color_ptr = config_values["background_color"];
		if(background_color_ptr)
		{
			auto cur_background_color_opt = background_color_ptr->expect_value<std::tuple<int, int, int>>();
			if(!cur_background_color_opt)
			{
				return false;
			}
			auto real_color = cur_background_color_opt.value();
			new_model_config.background_color = Color(get<0>(real_color), get<1>(real_color), get<2>(real_color)) ;
		}
		else
		{
			return false;
		}
		swap(new_model_config, cur_config);
		return true;
	}
	void read_model_config(const typed_worksheet& model_worksheet, model::model_config& cur_config)
	{
		auto config_map = get_config_values_from_sheet(model_worksheet);
		if(!convert_config_to_model(config_map, cur_config))
		{
			cout<<"cant convert config"<<endl;
		}
		for(const auto& i: config_map)
		{
			i.second->type_desc->~typed_node_type_descriptor();
			i.second->~typed_value();
		}
		config_map.clear();

	}
	void read_sheet_content_by_role(string_view sheet_role, const typed_worksheet& sheet_content, model::model& in_model)
	{
		const static unordered_map<string_view, sheet_type> avail_types = {{string_view("config"), sheet_type::config}, {string_view("circle"), sheet_type::circle}, {string_view("tile"), sheet_type::tile}, {string_view("point_link"), sheet_type::point_link}, {string_view("range_link"), sheet_type::range_link}, {string_view("color"), sheet_type::colors}};
		auto sheet_type_iter = avail_types.find(sheet_role);
		if(sheet_type_iter == avail_types.cend())
		{
			return;
		}
		auto current_role = sheet_type_iter->second;
		switch(current_role)
		{
		case sheet_type::config:
			read_model_config(sheet_content, in_model.config);
			break;
		case sheet_type::circle:
			read_circle_sheet(sheet_content, in_model.circles);
			break;
		case sheet_type::tile:
			read_tile_sheet(sheet_content, in_model.tiles);
			break;
		case sheet_type::point_link:
			read_point_link_sheet(sheet_content, in_model.point_links);
			break;
		case sheet_type::range_link:
			read_range_link_sheet(sheet_content, in_model.range_links);
			break;
		case sheet_type::circle_tick:
			read_circle_tick_sheet(sheet_content, in_model.circle_ticks);
			break;
		default:
			break;
		}
	}
	void read_role_sheet(const typed_worksheet& role_sheet, unordered_map<string_view, string_view>& sheet_role_map)
	{
		// headers sheet_name(string) role(string)
		std::unordered_map<string_view, const typed_header*> sheet_headers;
		sheet_headers["sheet_name"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "sheet_name", "");

		sheet_headers["role"] = new typed_header(new typed_node_type_descriptor(basic_value_type::string), "role", "");
		auto header_match = role_sheet.check_header_match(sheet_headers, "sheet_name", std::vector<std::string_view>({}), std::vector<std::string_view>({}));
		if(!header_match)
		{
			cout<<"header not match for role_sheet"<<endl;
			return;
		}
		const vector<const typed_header*>& all_headers = role_sheet.get_typed_headers();
		const auto& all_row_info = role_sheet.get_all_typed_row_info();
		for(int i = 1; i< all_row_info.size(); i++)
		{
			string_view cur_sheet_name, cur_sheet_role;
			for(int j = 1; j< all_row_info[i].size(); j++)
			{
				const auto&  cur_cell_value = all_row_info[i][j];
				uint32_t column_idx = j;
				string_view current_header_name = all_headers[column_idx]->header_name;
				if(current_header_name == "sheet_name")
				{
					auto opt_sheet_name = cur_cell_value.expect_value<string_view>();
					if(!opt_sheet_name)
					{
						break;
					}
					cur_sheet_name = opt_sheet_name.value();
				}
				else if(current_header_name == "role")
				{
					auto opt_role = cur_cell_value.expect_value<string_view>();
					if(!opt_role)
					{
						break;
					}
					cur_sheet_role = opt_role.value();
				}
			}
			if(cur_sheet_name.empty() || cur_sheet_role.empty())
			{
				continue;
			}
			if(sheet_role_map.find(cur_sheet_name) != sheet_role_map.end())
			{
				cout<<"duplicate sheet name "<<cur_sheet_name<<endl;
				continue;
			}
			sheet_role_map[cur_sheet_name] = cur_sheet_role;
		}
	}
	
}
namespace circos
{
	using namespace xlsx_reader;
	using namespace std;
	model::model read_model_from_workbook(const workbook<typed_worksheet>& in_workbook)
	{
		model::model result;
		auto role_sheet_idx = in_workbook.get_sheet_index_by_name("sheet_role");
		if(!role_sheet_idx)
		{
			cout<<"cant find role sheet"<<endl;
			return result;
		}
		const auto& role_sheet = in_workbook.get_worksheet(role_sheet_idx.value());
		unordered_map<string_view, string_view> sheet_roles;
		read_role_sheet(role_sheet, sheet_roles);
		int config_count = 0;
		for(const auto& i: sheet_roles)
		{
			auto sheet_name = i.first;
			auto sheet_role = i.second;
			if(sheet_name == "sheet_role")
			{
				cout<< "wrong sheet name sheet_role "<< endl;
				return model::model();
			}
			if(sheet_role == "config")
			{
				if(config_count != 0)
				{
					cout<<"more than one config sheet "<<sheet_name<<endl;
					return model::model();
				}
				config_count += 1;

			}
			auto temp_sheet_idx = in_workbook.get_sheet_index_by_name(sheet_name);
			if(!temp_sheet_idx)
			{
				cout<<"cant find sheet for "<<sheet_name<<endl;
				continue;
			}
			const auto& temp_sheet_content = in_workbook.get_worksheet(temp_sheet_idx.value());
			read_sheet_content_by_role(sheet_role, temp_sheet_content, result);
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
			PngImage png_image(font_info, png_output_file, the_model.config.radius, the_model.config.background_color);
			draw_collections(png_image, cur_collection);
		}
		if(!svg_output_file.empty())
		{
			SvgGraph svg_graph(font_info, svg_output_file, the_model.config.radius, the_model.config.background_color);
			draw_collections(svg_graph, cur_collection);
		}

	}
}
#endif

