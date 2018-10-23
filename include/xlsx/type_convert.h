#pragma once
#include "../shapes/line.h"
#include "../shapes/circle.h"
#include "../shapes/arc.h"
#include "../shapes/bezier.h"
#include "../shapes/ring.h"
#include "../shapes/track.h"
#include "../shapes/rectangle.h"
#include "../shapes/line_text.h"
#include <optional>
#include <xlsx_cell_extend.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <string_view>
#include <tuple>
#include <xlsx_typed.h>
namespace circos
{
    using namespace xlsx_reader;
	using namespace std;
	enum class sheet_type
	{
		circle,
		band,
		fill_onband,
		lable_on_band,
		tick_on_band,
		value_on_band,
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

	void read_circle_sheet(const typed_worksheet& circle_sheet, std::unordered_map<std::string_view, Cricle>& all_circles)
	{
		// circle headers circle_id(string) inner_radius(int) outer_radius(int) gap(int) color(RGB) ref_color(ref) opacity(double) filled(bool)
		std::unordered_map<std::string_view, typed_header> sheet_headers;
		sheet_headers["circle_id"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::string), "circle_id", "");
		sheet_headers["inner_radius"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "inner_radius", "");
		sheet_headers["outer_radius"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "outer_radius", "");

		auto point_type_detail = make_tuple(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), 3, ',');
		sheet_headers["center"] = typed_header(new extend_node_type_descriptor(point_type_detail), "center", "");
		
		sheet_headers["color"] = typed_header(new extend_node_type_descriptor(point_type_detail), "color", "");

		sheet_headers["opacity"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_double), "opacity", "");
		sheet_headers["filled"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_bool), "filled", "");

		sheet_headers["gap"] = typed_header(new extend_node_type_descriptor(basic_node_type_descriptor::number_32), "gap", "");
		for(const auto& i: circle_sheet.get_typed_headers())
		{
			auto cur_iter = sheet_headers.find(i.header_name);
			if(cur_iter == sheet_headers.end())
			{
				continue;
			}
			if(!(cur_iter->second == i))
			{
				return;
			}
		}

		for(const auto& i: circle_sheet.get_all_typed_row_info())
		{
			Circle cur_circle;
			for(const auto& j: i->second)
			{
				if(j->second.header_name == "circle_id")
				{
					
				}
			}
		}
	} 
}


