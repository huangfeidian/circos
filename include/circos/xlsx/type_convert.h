﻿#pragma once
#include <optional>
#include <xlsx_cell_extend.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <string_view>
#include <tuple>
#include <xlsx_typed.h>

#include "../shapes/line.h"
#include "../shapes/circle.h"
#include "../shapes/arc.h"
#include "../shapes/bezier.h"
#include "../shapes/ring.h"
#include "../shapes/ribbon.h"
#include "../shapes/rectangle.h"
#include "../shapes/line_text.h"

namespace circos
{
	using namespace xlsx_reader;
	using namespace std;
	enum class sheet_type
	{
		circle,
		tile,
		fill_ontile,
		lable_on_tile,
		tick_on_tile,
		value_on_tile,
		colors,
	}
	std::optional<Color> read_color_from_cell(const typed_worksheet& cur_worksheet, const typed_cell& cell_value)
	{
		if(!cell_value.cur_typed_value)
		{
			return std::nullopt;
		}
		switch(cell_value.cur_typed_value->type_desc->_type)
		{
			case basic_node_type_descriptor::list:
			case basic_node_type_descriptor::tuple:
			{
				auto opt_color = cell_value.cur_typed_value->get_value<std::tuple<int, int, int>>();
				if(!opt_color)
				{
					return std::nullopt;
				}
				auto real_color = opt_color.value();
				return Color(get<0>(real_color), get<1>(real_color), get<2>(real_color))
			}
			
			case basic_node_type_descriptor::ref_id:
			{
				auto cur_type_detail = cell_value.cur_typed_value->type_desc->get_ref_detail_t();
				if (!cur_type_detail)
				{
					return std::nullopt;
				}
				auto ref_detail = cur_type_detail.value();

				auto [cur_wb_name, cur_ws_name, cur_ref_type] = ref_detail;
				auto ref_value = cell_value.cur_typed_value;
				auto ref_row_value = cur_worksheet.get_ref_row(cur_ws_name, *ref_value);
				if (!ref_row_value)
				{
					return std::nullopt;
				}
				const auto& row_value = ref_row_value.value().get();
				if (row_value.size() == 2)
				{
					auto cur_workbook = cur_worksheet.get_workbook();
					auto other_sheet_idx = cur_workbook->get_sheet_index_by_name(cur_ws_name);
					if (!other_sheet_idx)
					{
						return std::nullopt;
					}
					const auto& other_sheet = cur_workbook->get_worksheet(other_sheet_idx.value());
					return read_color_from_cell(other_sheet, *(row_value.find(2)->second));
				}
				else
				{
					if (row_value.size() != 4)
					{
						return std::nullopt;
					}
					else
					{
						vector<uint32_t> rgb_values;
						for (int i = 2; i < 5; i++)
						{
							auto cur_cell_value_iter = row_value.find(i);
							if (cur_cell_value_iter == row_value.end())
							{
								return std::nullopt;
							}
							auto cur_cell_value = cur_cell_value_iter->second;
							auto cur_int_opt = cur_cell_value->cur_typed_value->get_value<std::uint32_t>();
							if (!cur_int_opt)
							{
								return std::nullopt;
							}
							rgb_values.push_back(cur_int_opt.value());
						}
						return Color(rgb_values[0], rgb_values[1], rgb_values[2]);
					}
				}

			}
			default:
				return std::nullopt;
				
		}
		return std::nullopt;
	}

	std::optional<Point> read_point_from_cell(const typed_worksheet& cur_worksheet, const typed_cell& cell_value)
	{
		if (!cell_value.cur_typed_value)
		{
			return std::nullopt;
		}

		auto opt_point = cell_value.cur_typed_value->get_value<std::tuple<int, int>>();
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
	std::optional<Circle> read_circle_from_row(const typed_worksheet& cur_worksheet, const std::map<std::string_view, const typed_cell*>& row_info)
	{
		auto all_keys = std::vector<std::string>{"circle_id", "inner_radius", "outer_radius", "gap", "color", "filled", "opacity"};
		for(const auto & i : all_keys)
		{
			if(row_info.find(i) == row_info.cend())
			{
				return std::nullopt;
			}
		}
		std::optional<std::uint32_t> inner_radius = row_info.find("inner_radius").second->expect_value<std::uint32_t>();
		if(!inner_radius)
		{
			return std::nullopt;
		}
		std::optional<std::uint32_t> outer_radius = row_info.find("outer_radius").second->expect_value<std::uint32_t>();
		if(!outer_radius)
		{
			return std::nullopt;
		}
		std::optional<std::uint32_t> gap = row_info.find("gap").second->expect_value<std::uint32_t>();
		if(!gap)
		{
			return std::nullopt;
		}
		std::optional<std::string_view> circle_id = row_info.find("circle_id").second->expect_value<std::string_view>();
		if(!circle_id)
		{
			return std::nullopt;
		}

		std::optional<Color> color = read_color_from_cell(cur_worksheet, row_info.find("color")->second);
		if(!color)
		{
			return std::nullopt;
		}
		auto opacity = row_info.find("opacity").second->expect_value<float>();
		auto filled = row_info.find("filled").second->expect_value<bool>();
		return Circle(static_cast<double>(inner_radius.value()), Point(0, 0), color.value(), in_opacity = opacity? opacity.value(), 1.0, in_filled = filled?filled.value(), true);
	}

	std::unordered_map<sheet_type, std::vector<std::string_view>> read_configuration_from_workbook(const workbook<typed_worksheet>&  cur_workbook)
	{
		//default configuration sheet name config
		std::unordered_map<sheet_type, std::vector<std::string_view>> result;
		auto sheet_idx_opt = cur_workbook.get_sheet_index_by_name("config");
		if(!sheet_idx_opt)
		{
			std::cerr<<"cant find config sheet for workbook "<<cur_workbook.get_workbook_name()<<std::endl;
			return result;
		}
		auto sheet_idx = sheet_idx_opt.value();
		// config typed headers (id sheet_type sheet_name)
		const auto& cur_worksheet = cur_workbook.get_worksheet(sheet_idx);
		const auto& cur_headers = cur_worksheet.get_typed_headers();
		std::vector<typed_header> sheet_headers;
		auto id_header = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "id", "");
		auto type_header = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "sheet_type", "");
		auto name_header = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "sheet_name", "")
		sheet_headers.push_back(id_header);
		sheet_headers.push_back(type_header);
		sheet_headers.push_back(name_header);
		if(!cur_worksheet.check_header_match(sheet_headers))
		{
			std::cerr<<"header size should be 3 for column(id(int32), sheet_type(int32), sheet_name(string))"<<std::endl;
			return result;
		}
		//
		for(const auto& one_row: cur_worksheet.get_all_typed_row_info())
		{
			auto type_value = one_row.find(1);
			if(type_value == one_row.end())
			{
				std::cerr<<"cant find value for column sheet_type"<<std::endl;
				return result;
			}
			auto name_value = one_row.find(2);
			if(name_value == one_row.end())
			{
				std::cerr<<"cant find value for column sheet_name"<< std::endl;
			}
			const auto& pre = result.find(type_value->second);
			if(pre == result.end())
			{
				result[type_value->second] = std::vector<std::string_view>({name_value->second});
			}
			else
			{
				pre.push_back(name_value->second);
			}
		}
		return result;
	}

	void read_circle_sheet(const typed_worksheet& circle_sheet, std::unordered_map<std::string_view, model::circle_desc>& all_circles)
	{
		// circle headers circle_id(string) inner_radius(int) outer_radius(int) gap(int) color(RGB) ref_color(ref) opacity(double) filled(bool)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["circle_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "circle_id", "");
		sheet_headers["inner_radius"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "inner_radius", "");
		sheet_headers["outer_radius"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "outer_radius", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_double), "opacity", "");

		sheet_headers["gap"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "gap", "");
		auto header_match = circle_sheet.check_header_match(sheet_headers, "circle_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for circle description mismatch for sheet "<<circle_sheet._name<<std::endl;
			return;
		}

		for(const auto& i: circle_sheet.get_all_typed_row_info())
		{
			model::circle_desc cur_circle;
			for(const auto& j: i->second)
			{
				auto current_header_name = j->second.header_name;
				if(current_header_name == "circle_id")
				{
					auto opt_circle_id = j->second.get_value<std::string_view>();
					if(!opt_circle_id)
					{
						continue;
					}
					cur_circle.circle_id = opt_circle_id.value();
				}
				elif(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<double>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_circle.opacity = opt_opacity.value();
				}
				elif(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_circle.fill_color = opt_color.value();
				}
				elif(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_circle.fill_color = opt_color.value();
				}
				elif(current_header_name == "gap")
				{
					auto opt_gap = j->second.get_value<int>();
					if(!opt_gap)
					{
						continue;
					}
					cur_circle.gap = opt_gap.value();
				}
				elif(current_header_name == "inner_radius")
				{
					auto opt_radius = j->second.get_value<int>();
					if(!opt_radius)
					{
						continue;
					}
					cur_circle.inner_radius = opt_radius.value();
				}
				elif(current_header_name == "outer_radius")
				{
					auto opt_radius = j->second.get_value<int>();
					if(!opt_radius)
					{
						continue;
					}
					cur_circle.outer_radius = opt_radius.value();
				}
			}
			if(!cur_circle.circle_id)
			{
				std::cerr<<"cant find circle for row "<< i.first<<std::endl;
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
	void read_tile_sheet(const typed_worksheet& tile_sheet, std::unordered_map<std::string_view, model::tile_desc>& all_tiles)
	{
		// tile_desc headers tile_id(string) circle_id(string)  width(int) color(RGB) ref_color(ref) opacity(double) sequence(int)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["circle_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "circle_id", "");

		sheet_headers["tile_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "tile_id", "");

		sheet_headers["width"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "width", "");

		sheet_headers["sequence"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "sequence", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_double), "opacity", "");

		auto header_match = tile_sheet.check_header_match(sheet_headers, "tile_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for tile description mismatch for sheet "<<tile_sheet._name<<std::endl;
			return;
		}

		for(const auto& i: tile_sheet.get_all_typed_row_info())
		{
			model::tile_desc cur_tile;
			for(const auto& j: i->second)
			{
				auto current_header_name = j->second.header_name;
				if(current_header_name == "circle_id")
				{
					auto opt_circle_id = j->second.get_value<std::string_view>();
					if(!opt_circle_id)
					{
						continue;
					}
					cur_tile.circle_id = opt_circle_id.value();
				}
				elif(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<double>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_tile.opacity = opt_opacity.value();
				}
				elif(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_tile.fill_color = opt_color.value();
				}
				elif(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_tile.fill_color = opt_color.value();
				}
				elif(current_header_name == "width")
				{
					auto opt_width = j->second.get_value<int>();
					if(!opt_width)
					{
						continue;
					}
					cur_tile.width = opt_width.value();
				}
				elif(current_header_name == "sequence")
				{
					auto opt_seq = j->second.get_value<int>();
					if(!opt_seq)
					{
						continue;
					}
					cur_tile.sequence = opt_seq.value();
				}
				
			}
			if(!cur_tile.tile_id)
			{
				std::cerr<<"cant find tile for row "<< i.first<<std::endl;
				continue;
			}
			if(all_tiles.find(cur_tile.tile_id) != all_tiles.end())
			{
				std::cerr<<"duplicated tile_id "<<cur_tile.tile_id<<std::endl;
				continue;
			}
			if(!cur_tile.circle_id)
			{
				std::cerr<<"missing circle_id for tile "<<cur_tile.tile_id<<std::endl;
			}

			all_tiles[cur_tile.circle_id] = cur_tile;
		}
	} 

	void read_circle_tick_sheet(const typed_worksheet& tick_sheet, std::unordered_map<std::string_view, model::circle_tick>& all_circle_ticks)
	{
		// circle_tick headers circle_id(string)  width(int) height(int) color(RGB) ref_color(ref) opacity(double)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["circle_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "circle_id", "");

		sheet_headers["width"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "width", "");
		sheet_headers["height"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "height", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_double), "opacity", "");

		auto header_match = tick_sheet.check_header_match(sheet_headers, "circle_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for circle_tick description mismatch for sheet "<<tick_sheet._name<<std::endl;
			return;
		}

		for(const auto& i: tick_sheet.get_all_typed_row_info())
		{
			model::circle_tick cur_circle_tick;
			for(const auto& j: i->second)
			{
				auto current_header_name = j->second.header_name;
				if(current_header_name == "circle_id")
				{
					auto opt_circle_id = j->second.get_value<std::string_view>();
					if(!opt_circle_id)
					{
						continue;
					}
					cur_circle_tick.circle_id = opt_circle_id.value();
				}
				elif(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<double>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_circle_tick.opacity = opt_opacity.value();
				}
				elif(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_circle_tick.fill_color = opt_color.value();
				}
				elif(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_circle_tick.fill_color = opt_color.value();
				}
				elif(current_header_name == "width")
				{
					auto opt_width = j->second.get_value<int>();
					if(!opt_width)
					{
						continue;
					}
					cur_circle_tick.width = opt_width.value();
				}
				elif(current_header_name == "height")
				{
					auto opt_height = j->second.get_value<int>();
					if(!opt_height)
					{
						continue;
					}
					cur_circle_tick.height = opt_height.value();
				}
			}
			if(!cur_circle_tick.circle_id)
			{
				std::cerr<<"cant find circle_tick for row "<< i.first<<std::endl;
				continue;
			}
			if(all_circle_ticks.find(cur_circle_tick.tile_id) != all_circle_ticks.end())
			{
				std::cerr<<"duplicated tile_id "<<cur_circle_tick.tile_id<<std::endl;
				continue;
			}

			all_circle_ticks[cur_circle_tick.circle_id] = cur_circle_tick;
		}
	} 

	void read_point_link_sheet(const typed_worksheet& point_link_sheet, std::unordered_map<std::string_view, model::point_link>& all_point_links)
	{
		// point_link headers link_id(string) from_tile_id(string) from_pos_idx(int) to_tile_id(string) to_pos_idx(int)  color(RGB) ref_color(ref) opacity(double)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["link_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "link_id", "");

		sheet_headers["from_tile_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "from_tile_id", "");

		sheet_headers["to_tile_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "to_tile_id", "");

		sheet_headers["from_pos_idx"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "from_pos_idx", "");
		sheet_headers["to_pos_idx"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "to_pos_idx", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_double), "opacity", "");
		control_radius_percent
		sheet_headers["control_radius_percent"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_double), "control_radius_percent", "");

		auto header_match = point_link_sheet.check_header_match(sheet_headers, "link_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for point_link description mismatch for sheet "<<point_link_sheet._name<<std::endl;
			return;
		}

		for(const auto& i: point_link_sheet.get_all_typed_row_info())
		{
			model::point_link cur_point_link;
			for(const auto& j: i->second)
			{
				auto current_header_name = j->second.header_name;
				if(current_header_name == "link_id")
				{
					auto opt_link_id = j->second.get_value<std::string_view>();
					if(!opt_link_id)
					{
						continue;
					}
					cur_point_link.link_id = opt_link_id.value();
				}
				elif(current_header_name == "from_tile_id")
				{
					auto opt_tile_id = j->second.get_value<std::string_view>();
					if(!opt_tile_id)
					{
						continue;
					}
					cur_point_link.from_tile_id = opt_tile_id.value();
				}
				elif(current_header_name == "to_tile_id")
				{
					auto opt_tile_id = j->second.get_value<std::string_view>();
					if(!opt_tile_id)
					{
						continue;
					}
					cur_point_link.to_tile_id = opt_tile_id.value();
				}
				elif(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<double>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_point_link.opacity = opt_opacity.value();
				}
				elif(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_point_link.fill_color = opt_color.value();
				}
				elif(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_point_link.fill_color = opt_color.value();
				}
				elif(current_header_name == "from_pos_idx")
				{
					auto opt_from_pos_idx = j->second.get_value<int>();
					if(!opt_from_pos_idx)
					{
						continue;
					}
					cur_point_link.from_pos_idx = opt_from_pos_idx.value();
				}
				elif(current_header_name == "to_pos_idx")
				{
					auto opt_to_pos_idx = j->second.get_value<int>();
					if(!opt_to_pos_idx)
					{
						continue;
					}
					cur_point_link.to_pos_idx = opt_to_pos_idx.value();
				}
				elif(current_header_name == "control_radius_percent")
				{
					auto opt_control_radius_percent = j->second.get_value<bool>();
					if(!opt_control_radius_percent)
					{
						continue;
					}
					cur_point_link.control_radius_percent = opt_control_radius_percent.value();
				}
			}
			if(!cur_point_link.link_id)
			{
				std::cerr<<"cant find point link for row "<< i.first<<std::endl;
				continue;
			}
			if(all_point_links.find(cur_point_link.link_id) != all_point_links.end())
			{
				std::cerr<<"duplicated link_id "<<cur_point_link.tile_id<<std::endl;
				continue;
			}

			all_point_links[cur_point_link.link_id] = cur_point_link;
		}
	} 

	void read_range_link_sheet(const typed_worksheet& range_link_sheet, std::unordered_map<std::string_view, model::range_link>& all_range_links)
	{
		// point_link headers link_id(string) from_tile_id(string) from_pos_idx(int) to_tile_id(string) to_pos_idx(int)  color(RGB) ref_color(ref) opacity(double)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["link_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "link_id", "");

		sheet_headers["from_tile_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "from_tile_id", "");

		sheet_headers["to_tile_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "to_tile_id", "");

		sheet_headers["from_pos_idx_begin"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "from_pos_idx_begin", "");
		sheet_headers["to_pos_idx_begin"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "to_pos_idx_begin", "");

		sheet_headers["from_pos_idx_end"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "from_pos_idx_end", "");
		sheet_headers["to_pos_idx_end"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "to_pos_idx_end", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_double), "opacity", "");

		sheet_headers["is_cross"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::bool), "is_cross", "");
		
		sheet_headers["control_radius_percent"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_double), "control_radius_percent", "");

		auto header_match = range_link_sheet.check_header_match(sheet_headers, "link_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		if(!header_match)
		{
			std::cerr<<"header for range_link description mismatch for sheet "<<range_link_sheet._name<<std::endl;
			return;
		}

		for(const auto& i: range_link_sheet.get_all_typed_row_info())
		{
			model::range_link cur_range_link;
			for(const auto& j: i->second)
			{
				auto current_header_name = j->second.header_name;
				if(current_header_name == "link_id")
				{
					auto opt_link_id = j->second.get_value<std::string_view>();
					if(!opt_link_id)
					{
						continue;
					}
					cur_range_link.link_id = opt_link_id.value();
				}
				elif(current_header_name == "from_tile_id")
				{
					auto opt_tile_id = j->second.get_value<std::string_view>();
					if(!opt_tile_id)
					{
						continue;
					}
					cur_range_link.from_tile_id = opt_tile_id.value();
				}
				elif(current_header_name == "to_tile_id")
				{
					auto opt_tile_id = j->second.get_value<std::string_view>();
					if(!opt_tile_id)
					{
						continue;
					}
					cur_range_link.to_tile_id = opt_tile_id.value();
				}
				elif(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<double>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_range_link.opacity = opt_opacity.value();
				}
				elif(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_range_link.fill_color = opt_color.value();
				}
				elif(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_range_link.fill_color = opt_color.value();
				}
				elif(current_header_name == "from_pos_idx_begin")
				{
					auto opt_from_pos_idx = j->second.get_value<int>();
					if(!opt_from_pos_idx)
					{
						continue;
					}
					cur_range_link.from_pos_idx_begin = opt_from_pos_idx.value();
				}
				elif(current_header_name == "to_pos_idx_begin")
				{
					auto opt_to_pos_idx = j->second.get_value<int>();
					if(!opt_to_pos_idx)
					{
						continue;
					}
					cur_range_link.to_pos_idx_begin = opt_to_pos_idx.value();
				}
				elif(current_header_name == "from_pos_idx_end")
				{
					auto opt_from_pos_idx = j->second.get_value<int>();
					if(!opt_from_pos_idx)
					{
						continue;
					}
					cur_range_link.from_pos_idx_end = opt_from_pos_idx.value();
				}
				elif(current_header_name == "to_pos_idx_end")
				{
					auto opt_to_pos_idx = j->second.get_value<int>();
					if(!opt_to_pos_idx)
					{
						continue;
					}
					cur_range_link.to_pos_idx_end = opt_to_pos_idx.value();
				}
				elif(current_header_name == "is_cross")
				{
					auto opt_is_cross = j->second.get_value<bool>();
					if(!opt_is_cross)
					{
						continue;
					}
					cur_range_link.is_cross = opt_is_cross.value();
				}
				elif(current_header_name == "control_radius_percent")
				{
					auto opt_control_radius_percent = j->second.get_value<bool>();
					if(!opt_control_radius_percent)
					{
						continue;
					}
					cur_range_link.control_radius_percent = opt_control_radius_percent.value();
				}
			}
			if(!cur_range_link.link_id)
			{
				std::cerr<<"cant find point link for row "<< i.first<<std::endl;
				continue;
			}
			if(all_range_links.find(cur_range_link.link_id) != all_range_links.end())
			{
				std::cerr<<"duplicated link_id "<<cur_range_link.tile_id<<std::endl;
				continue;
			}

			all_range_links[cur_range_link.link_id] = cur_range_link;
		}
	} 

	void read_model_config(const typed_worksheet& config_sheet, model::model_config& cur_config)
	{
		// key value
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["config_key"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "config_key", "");
		sheet_headers["config_value"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "config_value", "");
		auto header_match = config_sheet.check_header_match(sheet_headers, "config_key", std::vector<std::string_view>({}), std::vector<std::string_view>({}));
		if(!header_match)
		{
			std::cerr<<"header for model_config description mismatch for sheet "<<config_sheet._name<<std::endl;
			return;
		}
		for(const auto& i: point_link_sheet.get_all_typed_row_info())
		{
			auto opt_cur_key_name = i.find(1).second->cur_typed_value->get_value<std::string_view>();
			if(!opt_cur_key_name)
			{
				continue;
			}
			auto cur_key_name = opt_cur_key_name.value();

			auto opt_cur_value = i.find(2).second->cur_typed_value->get_value<std::string_view>();
			if(!opt_cur_value)
			{
				continue;
			}
			auto cur_value = opt_cur_value.value();
			if(cur_row_name == "radius")
			{
				cur_config.radius = std::stoi(std::string(cur_value));
			}
			elif(cur_row_name == "background_color")
			{
				auto opt_color = cell_value.cur_typed_value->get_value<std::tuple<int, int, int>>();
				if(!opt_color)
				{
					continue;
				}
				auto real_color = opt_color.value();
				cur_config.background_color Color(get<0>(real_color), get<1>(real_color), get<2>(real_color))
			}

		}
	}
}


