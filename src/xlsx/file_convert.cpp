#include <optional>
#include <vector>
#include <unordered_map>
#include <map>
#include <string_view>
#include <tuple>

#include <circos/model.h>

#include <xlsx_typed.h>
#include <xlsx_cell_extend.h>
#include <xlsx_workbook.h>

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
	}
	std::optional<Color> read_color_from_cell(const typed_worksheet& cur_worksheet, const typed_cell& cell_value)
	{
		static unordered_map<string_view, unordered_map<string_view, Color>> color_caches;
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
				cell_value.cur_typed_value->type_desc->get_ref_detail_t();
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
					return read_color_from_cell();
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
		return Circle(static_cast<float>(inner_radius.value()), Point(0, 0), color.value(), in_opacity = opacity? opacity.value(), 1.0, in_filled = filled?filled.value(), true);
	}

	std::unordered_map<string_view, std::vector<std::string_view>> read_configuration_from_workbook(const workbook<typed_worksheet>&  cur_workbook)
	{
		//default configuration sheet name config
		std::unordered_map<string_view, std::vector<std::string_view>> result;
		auto sheet_idx_opt = cur_workbook.get_sheet_index_by_name("role_config");
		if(!sheet_idx_opt)
		{
			std::cerr<<"cant find role_config sheet for workbook "<<cur_workbook.get_workbook_name()<<std::endl;
			return result;
		}
		auto sheet_idx = sheet_idx_opt.value();
		// config typed headers (id sheet_type sheet_name)
		const auto& cur_worksheet = cur_workbook.get_worksheet(sheet_idx);
		const auto& cur_headers = cur_worksheet.get_typed_headers();
		std::vector<typed_header> sheet_headers;
		auto id_header = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "id", "");
		auto type_header = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "sheet_type", "");
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
		// circle headers circle_id(string) inner_radius(int) outer_radius(int) gap(int) color(RGB) ref_color(ref) opacity(float) filled(bool)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["circle_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "circle_id", "");
		sheet_headers["inner_radius"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "inner_radius", "");
		sheet_headers["outer_radius"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "outer_radius", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_float), "opacity", "");

		sheet_headers["gap"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "gap", "");
		auto header_match = circle_sheet.check_header_match(sheet_headers, "circle_id", std::vector<std::string_view>({}), std::vector<std::string_view>({"ref_color"}));
		sheet_headers["filled"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::bool), "filled", "");
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
				else if(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<float>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_circle.opacity = opt_opacity.value();
				}
				else if(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_circle.fill_color = opt_color.value();
				}
				else if(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_circle.fill_color = opt_color.value();
				}
				else if(current_header_name == "gap")
				{
					auto opt_gap = j->second.get_value<int>();
					if(!opt_gap)
					{
						continue;
					}
					cur_circle.gap = opt_gap.value();
				}
				else if(current_header_name == "inner_radius")
				{
					auto opt_radius = j->second.get_value<int>();
					if(!opt_radius)
					{
						continue;
					}
					cur_circle.inner_radius = opt_radius.value();
				}
				else if(current_header_name == "outer_radius")
				{
					auto opt_radius = j->second.get_value<int>();
					if(!opt_radius)
					{
						continue;
					}
					cur_circle.outer_radius = opt_radius.value();
				}
				else if(current_header_name == "filled")
				{
					auto opt_filled = j->second.get_value<bool>();
					if(!opt_filled)
					{
						continue;
					}
					cur_circle.filled = opt_filled.value();
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
		// tile_desc headers tile_id(string) circle_id(string)  width(int) color(RGB) ref_color(ref) opacity(float) sequence(int) filled(bool)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["circle_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "circle_id", "");

		sheet_headers["tile_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "tile_id", "");

		sheet_headers["width"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "width", "");

		sheet_headers["sequence"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "sequence", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_float), "opacity", "");

		sheet_headers["filled"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::bool), "filled", "");

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
				else if(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<float>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_tile.opacity = opt_opacity.value();
				}
				else if(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_tile.fill_color = opt_color.value();
				}
				else if(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_tile.fill_color = opt_color.value();
				}
				else if(current_header_name == "width")
				{
					auto opt_width = j->second.get_value<int>();
					if(!opt_width)
					{
						continue;
					}
					cur_tile.width = opt_width.value();
				}
				else if(current_header_name == "sequence")
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
		// circle_tick headers tick_id(string) circle_id(string)  width(int) height(int) color(RGB) ref_color(ref) opacity(float)

		sheet_headers["tick_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "tick_id", "");

		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["circle_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "circle_id", "");

		sheet_headers["width"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "width", "");
		sheet_headers["height"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "height", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_float), "opacity", "");

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
				if(current_header_name == "tick_id")
				{
					auto opt_tick_id = j->second.get_value<std::string_view>();
					if(!opt_tick_id)
					{
						continue;
					}
					cur_circle_tick.tick_id = opt_tick_id.value();
				}
				else if(current_header_name == "circle_id")
				{
					auto opt_circle_id = j->second.get_value<std::string_view>();
					if(!opt_circle_id)
					{
						continue;
					}
					cur_circle_tick.circle_id = opt_circle_id.value();
				}
				else if(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<float>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_circle_tick.opacity = opt_opacity.value();
				}
				else if(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_circle_tick.fill_color = opt_color.value();
				}
				else if(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_circle_tick.fill_color = opt_color.value();
				}
				else if(current_header_name == "width")
				{
					auto opt_width = j->second.get_value<int>();
					if(!opt_width)
					{
						continue;
					}
					cur_circle_tick.width = opt_width.value();
				}
				else if(current_header_name == "height")
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
		// point_link headers link_id(string) from_tile_id(string) from_pos_idx(int) to_tile_id(string) to_pos_idx(int)  color(RGB) ref_color(ref) opacity(float)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["link_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "link_id", "");

		sheet_headers["from_tile_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "from_tile_id", "");

		sheet_headers["to_tile_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "to_tile_id", "");

		sheet_headers["from_pos_idx"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "from_pos_idx", "");
		sheet_headers["to_pos_idx"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "to_pos_idx", "");
		
		auto color_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 2, ',');
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(color_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_float), "opacity", "");
		control_radius_percent
		sheet_headers["control_radius_percent"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_float), "control_radius_percent", "");

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
				else if(current_header_name == "from_tile_id")
				{
					auto opt_tile_id = j->second.get_value<std::string_view>();
					if(!opt_tile_id)
					{
						continue;
					}
					cur_point_link.from_tile_id = opt_tile_id.value();
				}
				else if(current_header_name == "to_tile_id")
				{
					auto opt_tile_id = j->second.get_value<std::string_view>();
					if(!opt_tile_id)
					{
						continue;
					}
					cur_point_link.to_tile_id = opt_tile_id.value();
				}
				else if(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<float>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_point_link.opacity = opt_opacity.value();
				}
				else if(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_point_link.fill_color = opt_color.value();
				}
				else if(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_point_link.fill_color = opt_color.value();
				}
				else if(current_header_name == "from_pos_idx")
				{
					auto opt_from_pos_idx = j->second.get_value<int>();
					if(!opt_from_pos_idx)
					{
						continue;
					}
					cur_point_link.from_pos_idx = opt_from_pos_idx.value();
				}
				else if(current_header_name == "to_pos_idx")
				{
					auto opt_to_pos_idx = j->second.get_value<int>();
					if(!opt_to_pos_idx)
					{
						continue;
					}
					cur_point_link.to_pos_idx = opt_to_pos_idx.value();
				}
				else if(current_header_name == "control_radius_percent")
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
		// point_link headers link_id(string) from_tile_id(string) from_pos_idx_begin(int) from_pos_idx_end(int) to_tile_id(string) to_pos_idx_begin(int)  to_pos_idx_end(int) color(RGB) ref_color(ref) opacity(float)
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

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_float), "opacity", "");

		sheet_headers["is_cross"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::bool), "is_cross", "");
		
		sheet_headers["control_radius_percent"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_float), "control_radius_percent", "");

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
				else if(current_header_name == "from_tile_id")
				{
					auto opt_tile_id = j->second.get_value<std::string_view>();
					if(!opt_tile_id)
					{
						continue;
					}
					cur_range_link.from_tile_id = opt_tile_id.value();
				}
				else if(current_header_name == "to_tile_id")
				{
					auto opt_tile_id = j->second.get_value<std::string_view>();
					if(!opt_tile_id)
					{
						continue;
					}
					cur_range_link.to_tile_id = opt_tile_id.value();
				}
				else if(current_header_name == "opacity")
				{
					auto opt_opacity = j->second.get_value<float>();
					if(!opt_opacity)
					{
						continue;
					}
					cur_range_link.opacity = opt_opacity.value();
				}
				else if(current_header_name == "color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_range_link.fill_color = opt_color.value();
				}
				else if(current_header_name == "ref_color")
				{
					auto opt_color = read_color_from_cell(cur_worksheet, j);
					if(!opt_color)
					{
						continue;
					}
					cur_range_link.fill_color = opt_color.value();
				}
				else if(current_header_name == "from_pos_idx_begin")
				{
					auto opt_from_pos_idx = j->second.get_value<int>();
					if(!opt_from_pos_idx)
					{
						continue;
					}
					cur_range_link.from_pos_idx_begin = opt_from_pos_idx.value();
				}
				else if(current_header_name == "to_pos_idx_begin")
				{
					auto opt_to_pos_idx = j->second.get_value<int>();
					if(!opt_to_pos_idx)
					{
						continue;
					}
					cur_range_link.to_pos_idx_begin = opt_to_pos_idx.value();
				}
				else if(current_header_name == "from_pos_idx_end")
				{
					auto opt_from_pos_idx = j->second.get_value<int>();
					if(!opt_from_pos_idx)
					{
						continue;
					}
					cur_range_link.from_pos_idx_end = opt_from_pos_idx.value();
				}
				else if(current_header_name == "to_pos_idx_end")
				{
					auto opt_to_pos_idx = j->second.get_value<int>();
					if(!opt_to_pos_idx)
					{
						continue;
					}
					cur_range_link.to_pos_idx_end = opt_to_pos_idx.value();
				}
				else if(current_header_name == "is_cross")
				{
					auto opt_is_cross = j->second.get_value<bool>();
					if(!opt_is_cross)
					{
						continue;
					}
					cur_range_link.is_cross = opt_is_cross.value();
				}
				else if(current_header_name == "control_radius_percent")
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

	unordered_map<string_view, extend_node_value*> get_config_values_from_sheet(const typed_config_sheet& config_sheet)
	{

		// config_key(str), config_value(str), config_value_type(str)
		unordered_map<string_view, extend_node_value*> config_values;
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["config_key"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "config_key", "");
		sheet_headers["config_value"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "config_value", "");
		sheet_headers["config_value_type"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "config_value_type", "");
		auto header_match = config_sheet.check_header_match(sheet_headers, "config_key", std::vector<std::string_view>({}), std::vector<std::string_view>({}));
		if(!header_match)
		{
			std::cerr<<"header for model_config description mismatch for sheet "<<config_sheet._name<<std::endl;
			return config_values;
		}
		
		
		for(const auto& i: point_link_sheet.get_all_typed_row_info())
		{
			string_view cur_config_key, cur_config_value, cur_config_value_type;
			for(const auto& j: i)
			{
				auto cur_header_name = j->second.header_name;
				if(cur_header_name == "config_key")
				{
					auto opt_config_key = j.second.get_value<string_view>();
					if(!opt_config_key)
					{
						cout<<"no config key found"<endl;
						break;
					}
					cur_config_key = opt_config_key.value();
				}
				else if(cur_header_name == "config_value")
				{
					auto opt_config_value = j.second.get_value<string_view>();
					if(!opt_config_value)
					{
						cout<<"no config value found"<endl;
						break;
					}
					cur_config_value = opt_config_value.value();
				}
				else if(cur_header_name == "config_value_type")
				{
					auto opt_type_value = j.second.get_value<string_view>();
					if(!opt_type_value)
					{
						cout<<"no opt_type value found"<endl;
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
			auto current_parsed_type = extend_node_value_constructor::parse_type(cur_config_value_type);
			auto current_parsed_value = extend_node_value_constructor::parse_value_with_type(current_parsed_type, cur_config_value);
			if(!current_parsed_value)
			{
				cout<<"cant parse "<< cur_config_value << " for type "<< cur_config_value_type<<endl;
				if(current_parsed_type)
				{
					current_parsed_type->~extend_node_type_descriptor();
				}
				continue;
			}
			config_values[cur_config_key] = current_parsed_value;
		}
		return config_values;
	}
	bool convert_config_to_model(const unordered_map<string_view, extend_node_value*> config_values, model::model_config& cur_config)
	{
		model::model_config new_model_config;
		auto radius_value_ptr = config_values["radius"];
		if(radius_value_ptr)
		{
			auto cur_radius_opt = radius_value_ptr.get_value<int>();
			if(!cur_radius_opt)
			{
				return false;
			}
			new_model_config.radius = cur_radius_opt.value();
		}
		else
		{
			return false;
		}
		auto background_color_ptr = config_values["background_color"];
		if(background_color_ptr)
		{
			auto cur_background_color_opt = radius_value_ptr.get_value<std::tuple<int, int, int>>();
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
			i.second->type_desc->~extend_node_type_descriptor();
			i.second->~extend_node_value();
		}
		config_map.clear();

	}
	void read_sheet_content_by_role(string_view sheet_role, const typed_worksheet& sheet_content, model::model& in_model)
	{
		const static unordered_map<string_view, sheet_type> avail_types{{"image_config", sheet_type::config}, {"circle", sheet_type::circle}, {"tile", sheet_type::tile}, {"point_link", sheet_type::point_link}, {"range_link", sheet_type::range_link}, "color", sheet_type::colors};
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
		case sheet_type::
		default:
			break;
		}
	}
	void read_role_sheet(const typed_worksheet& role_sheet, unordered_map<string_view, string_view>& sheet_role_map)
	{
		// headers sheet_name(string) role(string)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["sheet_name"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "sheet_name", "");

		sheet_headers["role"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "role", "");
		auto header_match = role_sheet.check_header_match(sheet_headers, "sheet_name", std::vector<std::string_view>({}), std::vector<std::string_view>({}));
		if(!header_match)
		{
			cout<<"header not match for role_sheet"<<endl;
			return;
		}
		for(const auto& i: role_sheet.get_all_typed_row_info())
		{
			string_view cur_sheet_name, cur_sheet_role;
			for(const auto& j: i.second)
			{
				auto current_header_name = j->second.header_name;
				if(current_header_name == "sheet_name")
				{
					auto opt_sheet_name = j->second.get_value<string_view>();
					if(!opt_sheet_name)
					{
						break;
					}
					cur_sheet_name = opt_sheet_name.value();
				}
				else if(current_header_name == "role")
				{
					auto opt_role = j->second.get_value<string_view>();
					if(!opt_role)
					{
						break;
					}
					cur_sheet_role = opt_role.value();
				}
			}
			if(!cur_sheet_name ||!cur_sheet_role)
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
		
	}
	void read_xlsx_and_draw(const string& xlsx_path, const string& png_output_file, const string& svg_output_file)
	{
		auto the_archive = make_shared<archive>(xlsx_path);
		if(!the_archive || !the_archive->is_valie())
		{
			return;
		}
		workbook<typed_worksheet> cur_workbook(the_archive);
		auto the_model = read_model_from_workbook(cur_workbook);
		if(model.config.radius == 0)
		{
			return;
		}
		td::unordered_map<string, pair<string, string>> font_info{ { "yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei") } };
		if(!png_output_file && !svg_output_file)
		{
			return;
		}
		shape_collection cur_collection;
		the_model.to_shapes(cur_collection);
		if(png_output_file)
		{
			PngImage png_image(font_info, png_output_file, model.config.radius, model.config.background_color);
			draw_collections(png_image, cur_collection);
		}
		if(svg_output_file)
		{
			SvgGraph svg_graph(font_info, svg_output_file, model.config.radius, model.config.background_color);
			draw_collections(svg_graph, cur_collection);
		}

	}
}


