#include <circos/model.h>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace spiritsaway::circos::model
{
	using namespace std;
	void model::to_shapes(shape_collection& pre_collection)
	{
		// 1. 首先输出所有的圆环
		for(const auto& i: circles)
		{
			const auto& cur_circle = i.second;
			to_shapes(pre_collection, cur_circle);
		}
		// 2. 输出所有的贴片
		draw_tiles(pre_collection);
		// 3. 然后处理圆上的刻度
		for(const auto& one_tick: circle_ticks)
		{
			to_shapes(pre_collection, one_tick.second);
		}

		// 4. 开始处理两点之间的连线
		for(const auto& one_point_link: point_links)
		{
			to_shapes(pre_collection, one_point_link.second);
		}
		// 5. 处理两个条带之间的连线
		for(const auto& one_range_link: range_links)
		{
			to_shapes(pre_collection, one_range_link.second);
		}

		// 6. 处理tile上的刻度
		for (const auto& one_tick : tile_ticks)
		{
			to_shapes(pre_collection, one_tick.second);
		}

		// 7. 处理line_text

		for (const auto& i : line_texts)
		{
			to_shapes(pre_collection, i.second);
		}

		// 8. 整理所有的value_on_tile 删除不在同一个circle上的track 剩余的数据在track内排序
		for (auto& one_track_data : all_value_on_tile_by_track)
		{
			// 清空所有不在同一个circle上的data
			auto cur_track_config_iter = track_configs.find(one_track_data.first);
			if (cur_track_config_iter == track_configs.end())
			{
				continue;
			}
			const auto& cur_track_config = cur_track_config_iter->second;
			prepare_value_on_track(cur_track_config, one_track_data.second);
			to_shapes(pre_collection, cur_track_config, one_track_data.second);

			
		}
		
	}
	void model::to_shapes(shape_collection& pre_collection, const tick_on_tile& cur_tick)
	{
		
		auto tile_id = cur_tick.tile_id;
		auto cur_tile_iter = tiles.find(tile_id);
		if (cur_tile_iter == tiles.end())
		{
			return;
		}
		const auto& from_tile = cur_tile_iter->second;
		const auto& from_circle = circles[from_tile.circle_id];
		int total_count = from_tile.width / cur_tick.gap;
		float angle_by_unit = from_circle.angle_per_unit;
		for (int i = 0; i < total_count; i++)
		{
			Line cur_tick_line = Line(Point::radius_point(from_circle.outer_radius, free_angle::from_angle(i * cur_tick.gap * angle_by_unit + from_tile.angle_begin), config.center), Point::radius_point(from_circle.outer_radius + cur_tick.height, free_angle::from_angle(i * cur_tick.gap * angle_by_unit + from_tile.angle_begin), config.center), cur_tick.fill_color, cur_tick.width, cur_tick.opacity);
			pre_collection.lines.push_back(cur_tick_line);
		}
	}
	void model::to_shapes(shape_collection& pre_collection, const point_link& cur_point_link)
	{
		const auto& link_id = cur_point_link.link_id;
		auto from_tile_iter = tiles.find(cur_point_link.from.tile_id);
		if (from_tile_iter == tiles.end())
		{
			return;
		}
		const auto& from_tile = from_tile_iter->second;
		auto to_tile_iter = tiles.find(cur_point_link.to.tile_id);
		if (to_tile_iter == tiles.end())
		{
			return;
		}
		const auto& to_tile = to_tile_iter->second;
		const auto& from_circle = circles[from_tile.circle_id];
		const auto& to_circle = circles[to_tile.circle_id];
		if (cur_point_link.control_radius_percent <= 0)
		{
			// 这个是直线
			Line cur_line = Line(Point::radius_point(from_circle.inner_radius, free_angle::from_angle(from_tile.angle_begin + from_circle.angle_per_unit * cur_point_link.from.pos_idx)), Point::radius_point(to_circle.outer_radius, free_angle::from_angle(to_tile.angle_begin + to_circle.angle_per_unit * cur_point_link.to.pos_idx)), cur_point_link.fill_color, cur_point_link.width, cur_point_link.opacity);
		}
		else
		{
			// 这个是曲线
			auto from_angle = from_tile.angle_begin + from_circle.angle_per_unit * cur_point_link.from.pos_idx;
			auto to_angle = to_tile.angle_begin + to_circle.angle_per_unit * cur_point_link.to.pos_idx;
			if (to_tile.circle_id == from_tile.circle_id)
			{
				Bezier cur_bezier = Bezier(config.center, from_circle.inner_radius, free_angle::from_angle(from_angle), free_angle::from_angle(to_angle), cur_point_link.fill_color, from_circle.inner_radius * cur_point_link.control_radius_percent, cur_point_link.width, cur_point_link.opacity);
				pre_collection.beziers.push_back(cur_bezier);
			}
			else
			{
				auto control_point = Point::radius_point((from_circle.inner_radius + to_circle.inner_radius) / 2 * cur_point_link.control_radius_percent, free_angle::from_angle((from_angle + to_angle) / 2), config.center);
				Bezier cur_bezier = Bezier(Point::radius_point(from_circle.inner_radius, free_angle::from_angle(from_angle), config.center), Point::radius_point(to_circle.inner_radius, free_angle::from_angle(to_angle), config.center), control_point, cur_point_link.fill_color, cur_point_link.opacity);
				pre_collection.beziers.push_back(cur_bezier);
			}
		}
	}
	void model::to_shapes(shape_collection& pre_collection, const line_text& cur_line_text)
	{
		
		auto from_tile_iter = tiles.find(cur_line_text.from.tile_id);
		if (from_tile_iter == tiles.end())
		{
			return;
		}
		const auto& from_tile = from_tile_iter->second;
		const auto& from_circle = circles[from_tile.circle_id];
		auto from_angle = from_tile.angle_begin + from_circle.angle_per_unit * cur_line_text.from.pos_idx;
		auto from_point = Point::radius_point(from_circle.outer_radius, free_angle::from_angle(from_angle), config.center);
		if (cur_line_text.from.tile_id == cur_line_text.to.tile_id && cur_line_text.from.pos_idx == cur_line_text.to.pos_idx)
		{
			auto tangent_line = Line::cacl_tangent_clock_wise(config.center, from_point);
			pre_collection.line_texts.push_back(LineText(tangent_line, cur_line_text.utf8_text, cur_line_text.font_name, cur_line_text.font_size, cur_line_text.fill_color, cur_line_text.opacity));
			return;
		}
		auto to_tile_iter = tiles.find(cur_line_text.to.tile_id);
		if (to_tile_iter == tiles.end())
		{
			return;
		}
		const auto& to_tile = to_tile_iter->second;
		const auto& to_circle = circles[to_tile.circle_id];
		auto to_angle = to_tile.angle_begin + to_circle.angle_per_unit * cur_line_text.to.pos_idx;
		auto to_point = Point::radius_point(to_circle.outer_radius, free_angle::from_angle(to_angle), config.center);
		auto text_line = Line(from_point, to_point);
		pre_collection.line_texts.push_back(LineText(text_line, cur_line_text.utf8_text, cur_line_text.font_name, cur_line_text.font_size, cur_line_text.fill_color, cur_line_text.opacity));
	}
	void model::to_shapes(shape_collection& pre_collection, const range_link& cur_range_link)
	{
		auto from_tile_iter = tiles.find(cur_range_link.from.tile_id);
		if (from_tile_iter == tiles.end())
		{
			return;
		}
		const auto& from_tile = from_tile_iter->second;
		auto to_tile_iter = tiles.find(cur_range_link.to.tile_id);
		if (to_tile_iter == tiles.end())
		{
			return;
		}
		const auto& to_tile = to_tile_iter->second;
		const auto& from_circle = circles[from_tile.circle_id];
		const auto& to_circle = circles[to_tile.circle_id];
		auto from_begin_angle = from_tile.angle_begin + from_circle.angle_per_unit * cur_range_link.from.begin_pos;

		auto to_begin_angle = to_tile.angle_begin + to_circle.angle_per_unit * cur_range_link.to.begin_pos;

		Ribbon cur_ribbon(config.center, from_circle.inner_radius, to_circle.inner_radius, free_angle::from_angle(from_begin_angle), free_angle::from_angle(from_circle.angle_per_unit * cur_range_link.from.end_pos), free_angle::from_angle(to_begin_angle), free_angle::from_angle(to_circle.angle_per_unit * cur_range_link.to.end_pos), cur_range_link.fill_color, cur_range_link.is_cross, cur_range_link.control_radius_percent, cur_range_link.opacity < 0.001, cur_range_link.opacity);
		pre_collection.ribbons.push_back(cur_ribbon);
	}

	void model::to_shapes(shape_collection& pre_collection, const circle_tick& cur_tick)
	{
		const auto& circle_id = cur_tick.circle_id;
		const auto& circle_iter = circles.find(circle_id);
		if (circle_iter == circles.end())
		{
			return;
		}
		const auto& cur_circle = circle_iter->second;
		if (cur_tick.gap <= 0)
		{
			return;
		}
		int total_count = circle_ranges[circle_id] / cur_tick.gap;
		float angle_by_unit = static_cast<float>(360.0 / circle_ranges[circle_id]);
		for (int i = 0; i < total_count; i++)
		{
			Line cur_tick_line = Line(Point::radius_point(cur_circle.outer_radius, free_angle::from_angle(i * cur_tick.gap * angle_by_unit), config.center), Point::radius_point(cur_circle.outer_radius + cur_tick.height, free_angle::from_angle(i * cur_tick.gap * angle_by_unit), config.center), cur_tick.fill_color, cur_tick.width, cur_tick.opacity);
			pre_collection.lines.push_back(cur_tick_line);
		}
	}
	void model::to_shapes(shape_collection& pre_collection, const tile& data)
	{

	}
	void model::to_shapes(shape_collection& pre_collection, const circle& cur_circle)
	{
		auto cur_annulus = Annulus(config.center, cur_circle.fill_color, cur_circle.inner_radius, cur_circle.outer_radius, cur_circle.opacity, cur_circle.filled);
		pre_collection.annuluses.push_back(cur_annulus);
	}
	void model::draw_tiles(shape_collection& pre_collection)
	{
		
		// 分好组之后 再排序然后处理
		unordered_map<string_view, vector<tile>> tiles_grouped_by_circle;
		for (const auto& i : tiles)
		{
			auto circle_iter = circles.find(i.second.circle_id);
			if (circle_iter == circles.end())
			{
				continue;
			}
			auto& pre_groups = tiles_grouped_by_circle[i.second.circle_id];
			pre_groups.push_back(i.second);
		}
		for (auto& i : tiles_grouped_by_circle)
		{
			sort(i.second.begin(), i.second.end(), [](const tile& a, const tile& b)
			{
				return a.sequence < b.sequence;
			});
			
		}
		tiles.clear();
		for (auto& i : tiles_grouped_by_circle)
		{
			auto& cur_circle = circles.find(i.first)->second;
			auto& sorted_vector = i.second;
			draw_tiles_in_circle(pre_collection, cur_circle, sorted_vector);
			for (auto& one_tile : i.second)
			{
				tiles[one_tile.tile_id] = one_tile;
			}
		}
		
	}
	void model::draw_tiles_in_circle(shape_collection& pre_collection, circle& cur_circle, std::vector<tile>& tiles_in_circle)
	{
		int total_len = cur_circle.gap;
		total_len = accumulate(tiles_in_circle.begin(), tiles_in_circle.end(), total_len, [](int pre_total, const tile& cur_tile)
		{
			return pre_total + cur_tile.width;
		});
		circle_ranges[cur_circle.circle_id] = total_len;

		float temp_angle_begin = 0;
		float temp_angle_end = 0;
		float angle_by_unit = static_cast<float>(360.0 / total_len);
		cur_circle.angle_per_unit = static_cast<float>(angle_by_unit);
		auto temp_gap = cur_circle.gap / tiles_in_circle.size() * angle_by_unit;
		for (auto& one_tile : tiles_in_circle)
		{
			one_tile.angle_begin = temp_angle_begin;
			temp_angle_end = temp_angle_begin + angle_by_unit * one_tile.width;
			Tile cur_tile(config.center, cur_circle.inner_radius, cur_circle.outer_radius, free_angle::from_angle(temp_angle_begin), free_angle::from_angle(angle_by_unit * one_tile.width), one_tile.fill_color, 1, one_tile.is_fill, one_tile.opacity);
			pre_collection.tiles.push_back(cur_tile);
			temp_angle_end += temp_gap;
			temp_angle_begin = temp_angle_end;
		}
	}

	void model::prepare_value_on_track(const track_config& config, std::vector<value_on_tile>& one_track_data)
	{
		std::string_view on_circle_id = config.circle_id;
		auto cur_circle_iter = circles.find(on_circle_id);
		if (cur_circle_iter == circles.end())
		{
			one_track_data.clear();
			return;
		}
		const auto& cur_circle = cur_circle_iter->second;
		for (auto& one_value_data : one_track_data)
		{
			auto cur_tile_id = one_value_data.tile_id;
			auto cur_tile_iter = tiles.find(cur_tile_id);
			if (cur_tile_iter == tiles.end())
			{
				std::cout << "invalid tile " << cur_tile_id << " on track " << config.track_id << std::endl;
				one_track_data.clear();
				return;
			}
			auto cur_circle_id = cur_tile_iter->second.circle_id;
			if (cur_circle_id.empty())
			{
				std::cout << " invalid data with empty circle_id for tile  " << cur_tile_id << " cur data id " << one_value_data.data_id << std::endl;
				one_track_data.clear();
				return;
			}
			if (cur_circle_id != on_circle_id)
			{
				std::cout << "data on track " << config.track_id << " has more than one circle " << cur_circle_id << " " << on_circle_id << std::endl;
				one_track_data.clear();
				return;
			}
			const auto& cur_tile = tiles[one_value_data.tile_id];
			one_value_data.angle_begin = static_cast<float>(cur_tile.angle_begin + cur_circle.angle_per_unit *one_value_data.begin_pos);
			one_value_data.angle_end = static_cast<float>(cur_tile.angle_begin + cur_circle.angle_per_unit *one_value_data.end_pos);
			float progress = 1.0f;
			if (config.clamp_data_value.first == config.clamp_data_value.second)
			{
				progress = 1.0f;
			}
			else
			{
				progress = (one_value_data.data_value - config.clamp_data_value.first) / (config.clamp_data_value.second - config.clamp_data_value.first);
			}
			progress = progress > 1.0f ? 1.0f : progress;
			progress = progress < 0.0f ? 0.0f : progress;
			one_value_data.data_percentage = progress;
		}
		std::sort(one_track_data.begin(), one_track_data.end(), [](const value_on_tile& a, const value_on_tile& b)
		{
			return a.angle_begin < b.angle_begin;
		});
	}
	void model::to_shapes(shape_collection& pre_collection, const track_config& cur_track_config, std::vector<value_on_tile>& cur_track_data)
	{
		if (cur_track_data.size() == 0)
		{
			return;
		}
		auto cur_circle_id = tiles[cur_track_data[0].tile_id].circle_id;
		auto origin_circle_radius = circles[cur_circle_id].outer_radius;
		switch (cur_track_config.draw_type)
		{
		case track_draw_type::point:
		{
			for (const auto& cur_point_data : cur_track_data)
			{
				
				auto cur_circle_radius = cur_track_config.radius_offset.first * (1 - cur_point_data.data_percentage) + cur_track_config.radius_offset.second * cur_point_data.data_percentage + origin_circle_radius;
				auto cur_point_center = Point::radius_point(cur_circle_radius, (free_angle::from_angle((cur_point_data.angle_begin + cur_point_data.angle_end))/ 2)) + config.center;
				auto cur_rad_range = free_angle::from_angle((cur_point_data.angle_end - cur_point_data.angle_begin) / 2);
				auto cur_point_color = Color(cur_track_config.clamp_color.first, cur_track_config.clamp_color.second, cur_point_data.data_percentage);
				std::uint32_t cur_point_size = (cur_track_config.radius_offset.second - cur_track_config.radius_offset.first) * cur_point_data.data_percentage;
				if (cur_track_config.fixed_size)
				{
					cur_point_size = cur_track_config.fixed_size;
				}
				cur_point_size = std::min(cur_point_size, static_cast<std::uint32_t>(cur_circle_radius * cur_rad_range.normal().sin()));

				auto cur_circle = Circle(cur_point_size, cur_point_center, cur_point_color, 1.0f, true);
				pre_collection.circles.push_back(cur_circle);
			}
			return;
		}
		case track_draw_type::radius_point:
		{
			for (const auto& cur_point_data : cur_track_data)
			{
				auto cur_circle_radius = (cur_track_config.radius_offset.first  + cur_track_config.radius_offset.second) * 0.5  + origin_circle_radius;
				auto cur_rad_range = free_angle::from_angle((cur_point_data.angle_end - cur_point_data.angle_begin) / 2);
				auto cur_point_center = Point::radius_point(cur_circle_radius, (free_angle::from_angle((cur_point_data.angle_begin + cur_point_data.angle_end)) / 2)) + config.center;

				auto cur_point_color = Color(cur_track_config.clamp_color.first, cur_track_config.clamp_color.second, cur_point_data.data_percentage);
				std::uint32_t cur_point_size = (cur_track_config.radius_offset.second - cur_track_config.radius_offset.first) * cur_point_data.data_percentage;
				if (cur_track_config.fixed_size)
				{
					cur_point_size = cur_track_config.fixed_size;
				}
				cur_point_size = std::min(cur_point_size, static_cast<std::uint32_t>(cur_circle_radius * cur_rad_range.normal().sin()));
				auto cur_circle = Circle(cur_point_size, cur_point_center, cur_point_color, 1.0f, true);
				pre_collection.circles.push_back(cur_circle);
			}
			return;
		}
		case track_draw_type::link:
		{
			Point pre_point;
			std::uint32_t stroke = cur_track_config.fixed_size ? cur_track_config.fixed_size : 1;
			for (std::size_t i = 0; i < cur_track_data.size(); i++)
			{
				const auto& cur_point_data = cur_track_data[i];
				auto cur_point_radius = (cur_track_config.radius_offset.second * cur_point_data.data_percentage + cur_track_config.radius_offset.first * (1 - cur_point_data.data_percentage)) + origin_circle_radius;
				auto cur_point_center = Point::radius_point(cur_point_radius, (free_angle::from_angle((cur_point_data.angle_begin + cur_point_data.angle_end)) / 2)) + config.center;

				if(i != 0)
				{
					auto cur_point_color = Color(cur_track_config.clamp_color.first, cur_track_config.clamp_color.second, cur_point_data.data_percentage);
					
					auto temp_line = Line(pre_point, cur_point_center, cur_point_color, stroke, 1);
					pre_collection.lines.push_back(temp_line);
				}
				pre_point = cur_point_center;
			}
			return;
		}
		case track_draw_type::area:
		{
			for (std::uint32_t i = 1; i < cur_track_data.size(); i++)
			{
				// pre point
				const auto& last_point_data = cur_track_data[i - 1];
				auto last_point_radius = (cur_track_config.radius_offset.second * last_point_data.data_percentage + cur_track_config.radius_offset.first * (1 - last_point_data.data_percentage)) + origin_circle_radius;
				auto last_rad = free_angle::from_angle((last_point_data.angle_begin + last_point_data.angle_end) / 2);
				auto last_point_center = Point::radius_point(last_point_radius, last_rad)  + config.center;

				// cur_point
				const auto& cur_point_data = cur_track_data[i];
				auto cur_point_radius = (cur_track_config.radius_offset.second * cur_point_data.data_percentage + cur_track_config.radius_offset.first * (1 - cur_point_data.data_percentage)) + origin_circle_radius;
				auto cur_rad = free_angle::from_angle((cur_point_data.angle_begin + cur_point_data.angle_end) / 2);
				auto cur_point_center = Point::radius_point(cur_point_radius, cur_rad) + config.center;


				auto cur_point_color = Color(cur_track_config.clamp_color.first, cur_track_config.clamp_color.second, cur_point_data.data_percentage);
				auto temp_line = Line(last_point_center, cur_point_center, cur_point_color, 1, 1);
				pre_collection.lines.push_back(temp_line);


				// 需要绘制阴影 就是arc和几条直线的闭合环路

				Region temp_region;
				temp_region.add_boundary(Arc(origin_circle_radius, last_rad, (free_angle(cur_rad) - last_rad), config.center, false, cur_track_config.clamp_color.second));
				temp_region.add_boundary(Line(Point::radius_point(origin_circle_radius, cur_rad) + config.center, temp_line.to, cur_track_config.clamp_color.second));
				temp_region.add_boundary(Line(temp_line.to, temp_line.from, temp_line.color, temp_line.width, temp_line.opacity));
				temp_region.add_boundary(Line(temp_line.from, Point::radius_point(origin_circle_radius, last_rad) + config.center, cur_track_config.clamp_color.second));

				temp_region.opacity = 1.0f;
				temp_region.color = cur_point_color;
				temp_region.set_inner_point((Point::radius_point(origin_circle_radius, last_rad) + config.center + temp_line.to) * 0.5f);
				pre_collection.regions.push_back(temp_region);

			}
			return;
		}
		case track_draw_type::histogram:
		{
			for (const auto& cur_point_data : cur_track_data)
			{

				auto cur_circle_radius = cur_track_config.radius_offset.first * (1 - cur_point_data.data_percentage) + cur_track_config.radius_offset.second * cur_point_data.data_percentage + origin_circle_radius;
				auto cur_rad = free_angle::from_angle((cur_point_data.angle_begin + cur_point_data.angle_end) / 2);
				auto cur_rad_range = free_angle::from_angle((cur_point_data.angle_end - cur_point_data.angle_begin) / 2);
				auto cur_point_center = Point::radius_point(cur_circle_radius, cur_rad ) + config.center;

				auto cur_point_bottom = Point::radius_point(origin_circle_radius + cur_track_config.radius_offset.first, cur_rad) + config.center;
				auto line_thickness = (origin_circle_radius + cur_track_config.radius_offset.first) * (cur_rad_range.normal().sin()) * 2;
				if (cur_track_config.fixed_size)
				{
					line_thickness = cur_track_config.fixed_size;
				}
				auto  cur_point_color = Color(cur_track_config.clamp_color.first, cur_track_config.clamp_color.second, cur_point_data.data_percentage);
				auto temp_line = Line(cur_point_bottom, cur_point_center, cur_point_color, line_thickness, 1);
				pre_collection.lines.push_back(temp_line);
			}
			return;
		}
		case track_draw_type::fan:
		{
			for (const auto& cur_point_data : cur_track_data)
			{

				auto cur_circle_radius = cur_track_config.radius_offset.first * (1 - cur_point_data.data_percentage) + cur_track_config.radius_offset.second * cur_point_data.data_percentage + origin_circle_radius;
				auto bottom_radius = cur_track_config.radius_offset.first + origin_circle_radius;
				auto begin_rad = free_angle::from_angle(cur_point_data.angle_begin);
				auto end_rad = free_angle::from_angle(cur_point_data.angle_end);

				auto  cur_point_color = Color(cur_track_config.clamp_color.first, cur_track_config.clamp_color.second, cur_point_data.data_percentage);
				auto temp_region = Region::make_fan(bottom_radius, cur_circle_radius, begin_rad, end_rad, config.center, cur_point_color);
				
				temp_region.color = cur_point_color;
				temp_region.opacity = 1.0;

				pre_collection.regions.push_back(temp_region);
			}
			return;
		}
		default:
			break;
		}
	}

}