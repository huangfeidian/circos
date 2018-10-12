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
    std::optional<Color> read_color_from_cell(const typed_worksheet& cur_worksheet, const typed_cell& cell_value)
    {
        if(!cell_value.cur_typed_value)
        {
            return std::nullopt;
        }
        switch(cell_value.cur_typed_value->type_desc->_type)
        {
            case basic_node_type_descriptor::list:
			{
				auto cur_type_detail = cell_value.cur_typed_value->type_desc->get_list_detail_t();
				if (!cur_type_detail)
				{
					return std::nullopt;
				}
				if (cell_value.cur_typed_value->v_list.size() != 3)
				{
					return std::nullopt;
				}
				if (std::get<0>(cur_type_detail.value())->_type != basic_node_type_descriptor::number_u32)
				{
					return std::nullopt;
				}
				std::uint32_t r, g, b;
				r = cell_value.cur_typed_value->v_list[0]->v_uint32;
				g = cell_value.cur_typed_value->v_list[1]->v_uint32;
				b = cell_value.cur_typed_value->v_list[2]->v_uint32;
				return Color(r, g, b);
			}
                
			case basic_node_type_descriptor::tuple:
			{
				auto cur_type_detail = cell_value.cur_typed_value->type_desc->get_tuple_detail_t();
				if (!cur_type_detail)
				{
					return std::nullopt;
				}
				if (cell_value.cur_typed_value->v_list.size() != 3)
				{
					return std::nullopt;
				}
				auto tuple_detail = cur_type_detail.value().first;
				if (tuple_detail.size() != 3)
				{
					return std::nullopt;
				}

				if (tuple_detail[0]->_type != basic_node_type_descriptor::number_u32)
				{
					return std::nullopt;
				}
				if (tuple_detail[1](cur_type_detail.value())->_type != basic_node_type_descriptor::number_u32)
				{
					return std::nullopt;
				}
				if (tuple_detail[2](cur_type_detail.value())->_type != basic_node_type_descriptor::number_u32)
				{
					return std::nullopt;
				}
				std::uint32_t r, g, b;
				r = cell_value.cur_typed_value->v_list[0]->v_uint32;
				g = cell_value.cur_typed_value->v_list[1]->v_uint32;
				b = cell_value.cur_typed_value->v_list[2]->v_uint32;
				return Color(r, g, b);
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
		switch (cell_value.cur_typed_value->type_desc->_type)
		{
		case basic_node_type_descriptor::list:
		{
			auto cur_type_detail = cell_value.cur_typed_value->type_desc->get_list_detail_t();
			if (!cur_type_detail)
			{
				return std::nullopt;
			}
			if (cell_value.cur_typed_value->v_list.size() != 2)
			{
				return std::nullopt;
			}
			if (std::get<0>(cur_type_detail.value())->_type != basic_node_type_descriptor::number_32)
			{
				return std::nullopt;
			}
			std::int32_t x, y;
			x = cell_value.cur_typed_value->v_list[0]->v_int32;
			y = cell_value.cur_typed_value->v_list[1]->v_int32;
			return Point(x, y);
		}

		case basic_node_type_descriptor::tuple:
		{
			auto cur_type_detail = cell_value.cur_typed_value->type_desc->get_tuple_detail_t();
			if (!cur_type_detail)
			{
				return std::nullopt;
			}
			if (cell_value.cur_typed_value->v_list.size() != 2)
			{
				return std::nullopt;
			}
			auto tuple_detail = cur_type_detail.value().first;
			if (tuple_detail.size() != 2)
			{
				return std::nullopt;
			}

			auto x_opt = cell_value.cur_typed_value->v_list[0].get_value<std::int32_t>();
			if (!x_opt)
			{
				return std::nullopt;
			}
			auto y_opt = cell_value.cur_typed_value->v_list[1].get_value<std::int32_t>();
			if (!y_opt)
			{
				return std::nullopt;
			}
			return Point(x_opt.value(), y_opt.value());
		}
		default:
			return std::nullopt;
	}
	std::optional<Circle> read_circle_from_row(const typed_worksheet& cur_worksheet, const std::map<std::uint32_t, const typed_cell*>& row_info)
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

}


