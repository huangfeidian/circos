#include <circos/model.h>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

namespace circos::model
{
	using namespace std;
	void model::to_shapes(shape_collection& pre_collection)
	{
		// 1. 首先输出所有的圆环
		for(const auto& i: circles)
		{
			const auto& cur_circle = i.second;
			auto cur_annulus = Annulus(config.center, cur_circle.fill_color, cur_circle.inner_radius, cur_circle.outer_radius, cur_circle.opacity, cur_circle.filled);
			pre_collection.annuluses.push_back(cur_annulus);
		}
		// 2. 然后计算每个圆环上每条带子的位置
		unordered_map<string_view, vector<tile>> tiles_grouped_by_circle;
		for(const auto& i: tiles)
		{
			auto circle_iter = circles.find(i.second.circle_id);
			if(circle_iter == circles.end())
			{
				continue;
			}
			auto& pre_groups = tiles_grouped_by_circle[i.second.circle_id];
			pre_groups.push_back(i.second);
		}
		// 分好组之后 再排序然后处理
		unordered_map<string_view, int> circle_ranges;
		for(auto& i: tiles_grouped_by_circle)
		{
			auto& cur_circle = circles.find(i.first)->second;
			auto& sorted_vector = i.second;
			sort(sorted_vector.begin(), sorted_vector.end(), [](const tile& a, const tile& b)
			{
				return a.sequence < b.sequence;
			});
			int total_len = cur_circle.gap;
			total_len = accumulate(sorted_vector.begin(), sorted_vector.end(), total_len, [](int pre_total, const tile& cur_tile)
			{
				return pre_total + cur_tile.width;
			});
			circle_ranges[i.first] = total_len;

			double temp_angle_begin = 0;
			double temp_angle_end = 0;
			double angle_by_unit = pi() * 2 / total_len;
			cur_circle.angle_per_unit = angle_by_unit;
			auto temp_gap = cur_circle.gap / sorted_vector.size() * angle_by_unit;
			for(auto& one_tile: sorted_vector)
			{	
				one_tile.angle_begin = temp_angle_begin;
				temp_angle_end = temp_angle_begin + angle_by_unit * one_tile.width;
				Tile cur_tile(config.center, cur_circle.inner_radius, cur_circle.outer_radius, amplify_angle::from_rad(temp_angle_begin), amplify_angle::from_rad(temp_angle_end), one_tile.fill_color, 1, one_tile.is_fill, one_tile.opacity);
				pre_collection.tiles.push_back(cur_tile);
				temp_angle_end +=  temp_gap;
				temp_angle_begin = temp_angle_end;
			}
		}

		// 重新赋值一下tiles
		tiles.clear();
		for(const auto& i: tiles_grouped_by_circle)
		{
			for(const auto& one_tile: i.second)
			{
				tiles[one_tile.tile_id] = one_tile;
			}
		}
		// 3. 然后处理圆上的刻度
		for(const auto& one_tick: circle_ticks)
		{
			const auto& circle_id = one_tick.second.circle_id;
			const auto& circle_iter = circles.find(circle_id);
			if(circle_iter == circles.end())
			{
				continue;
			}
			const auto& cur_circle = circle_iter->second;
			const auto& cur_tick = one_tick.second;
			if(one_tick.second.gap <= 0)
			{
				continue;
			}
			int total_count = circle_ranges[circle_id] / cur_tick.gap;
			double angle_by_unit = pi() * 2 / circle_ranges[circle_id];
			for(int i = 0; i < total_count; i++)
			{
				Line cur_tick_line = Line(Point::radius_point(cur_circle.outer_radius, amplify_angle::from_rad(i * cur_tick.gap * angle_by_unit), config.center), Point::radius_point(cur_circle.outer_radius + cur_tick.height, amplify_angle::from_rad(i * cur_tick.gap * angle_by_unit), config.center), cur_tick.fill_color, cur_tick.width, cur_tick.opacity);
				pre_collection.lines.push_back(cur_tick_line);
			}
		}

		// 4. 开始处理两点之间的连线
		for(const auto& one_point_link: point_links)
		{
			const auto& link_id = one_point_link.first;
			const auto& cur_point_link = one_point_link.second;
			auto from_tile_iter = tiles.find(cur_point_link.from_tile_id);
			if(from_tile_iter == tiles.end())
			{
				continue;
			}
			const auto& from_tile = from_tile_iter->second;
			auto to_tile_iter = tiles.find(cur_point_link.to_tile_id);
			if(to_tile_iter == tiles.end())
			{
				continue;
			}
			const auto& to_tile = to_tile_iter->second;
			const auto& from_circle = circles[from_tile.circle_id];
			const auto& to_circle = circles[to_tile.circle_id];
			if(cur_point_link.control_radius_percent <= 0)
			{
				// 这个是直线
				Line cur_line = Line(Point::radius_point(from_circle.inner_radius, amplify_angle::from_rad(from_tile.angle_begin + from_circle.angle_per_unit * cur_point_link.from_pos_idx)), Point::radius_point(to_circle.outer_radius, amplify_angle::from_rad(to_tile.angle_begin + to_circle.angle_per_unit * cur_point_link.to_pos_idx)), cur_point_link.fill_color, cur_point_link.width, cur_point_link.opacity);
			}
			else
			{
				// 这个是曲线
				auto from_angle = from_tile.angle_begin + from_circle.angle_per_unit * cur_point_link.from_pos_idx;
				auto to_angle = to_tile.angle_begin + to_circle.angle_per_unit * cur_point_link.to_pos_idx;
				if(to_tile.circle_id == from_tile.circle_id)
				{
					Bezier cur_bezier = Bezier(config.center, from_circle.inner_radius, amplify_angle::from_rad(from_angle), amplify_angle::from_rad(to_angle), cur_point_link.fill_color, from_circle.inner_radius * cur_point_link.control_radius_percent, cur_point_link.width, cur_point_link.opacity);
					pre_collection.beziers.push_back(cur_bezier);
				}
				else
				{
					auto control_point = Point::radius_point((from_circle.inner_radius + to_circle.inner_radius) / 2 * cur_point_link.control_radius_percent, amplify_angle::from_rad((from_angle + to_angle) / 2), config.center);
					Bezier cur_bezier = Bezier(Point::radius_point(from_circle.inner_radius, amplify_angle::from_rad(from_angle), config.center), Point::radius_point(to_circle.inner_radius, amplify_angle::from_rad(to_angle), config.center), control_point, cur_point_link.fill_color, cur_point_link.opacity);
					pre_collection.beziers.push_back(cur_bezier);
				}
			}
		}
		// 5. 处理两个条带之间的连线
		for(const auto& one_range_link: range_links)
		{
			const auto& cur_range_link = one_range_link.second;
			auto from_tile_iter = tiles.find(cur_range_link.from_tile_id);
			if(from_tile_iter == tiles.end())
			{
				continue;
			}
			const auto& from_tile = from_tile_iter->second;
			auto to_tile_iter = tiles.find(cur_range_link.to_tile_id);
			if(to_tile_iter == tiles.end())
			{
				continue;
			}
			const auto& to_tile = to_tile_iter->second;
			const auto& from_circle = circles[from_tile.circle_id];
			const auto& to_circle = circles[to_tile.circle_id];
			auto from_begin_angle = from_tile.angle_begin + from_circle.angle_per_unit * cur_range_link.from_pos_begin_idx;
			auto from_end_angle = from_tile.angle_begin + from_circle.angle_per_unit * cur_range_link.from_pos_end_idx;
			auto to_begin_angle = to_tile.angle_begin + to_circle.angle_per_unit * cur_range_link.to_pos_begin_idx;
			auto to_end_angle = to_tile.angle_begin + to_circle.angle_per_unit * cur_range_link.to_pos_end_idx;
			Ribbon cur_ribbon(config.center, from_circle.inner_radius, to_circle.inner_radius, amplify_angle::from_rad(from_begin_angle), amplify_angle::from_rad(from_end_angle), amplify_angle::from_rad(to_begin_angle), amplify_angle::from_rad(to_end_angle), cur_range_link.fill_color, cur_range_link.control_radius_percent, cur_range_link.is_cross, cur_range_link.opacity < 0.001, cur_range_link.opacity);
			pre_collection.ribbons.push_back(cur_ribbon);
		}

		// 6. 处理tile上的刻度
		for (const auto& i : tile_ticks)
		{
			const auto& cur_tick = i.second;
			auto tile_id = cur_tick.tile_id;
			auto cur_tile_iter = tiles.find(tile_id);
			if (cur_tile_iter == tiles.end())
			{
				continue;
			}
			const auto& from_tile = cur_tile_iter->second;
			const auto& from_circle = circles[from_tile.circle_id];
			int total_count = from_tile.width / cur_tick.gap;
			double angle_by_unit = from_circle.angle_per_unit;
			for (int i = 0; i < total_count; i++)
			{
				Line cur_tick_line = Line(Point::radius_point(from_circle.outer_radius, amplify_angle::from_rad(i * cur_tick.gap * angle_by_unit + from_tile.angle_begin), config.center), Point::radius_point(from_circle.outer_radius + cur_tick.height, amplify_angle::from_rad(i * cur_tick.gap * angle_by_unit + from_tile.angle_begin), config.center), cur_tick.fill_color, cur_tick.width, cur_tick.opacity);
				pre_collection.lines.push_back(cur_tick_line);
			}
		}

		// 7. 处理line_text

		for (const auto& i : line_texts)
		{
			const auto& cur_line_text = i.second;
			auto from_tile_iter = tiles.find(cur_line_text.from_tile_id);
			if (from_tile_iter == tiles.end())
			{
				continue;
			}
			const auto& from_tile = from_tile_iter->second;
			const auto& from_circle = circles[from_tile.circle_id];
			auto from_angle = from_tile.angle_begin + from_circle.angle_per_unit * cur_line_text.from_pos_idx;
			auto from_point = Point::radius_point(from_circle.outer_radius, amplify_angle::from_rad(from_angle), config.center);
			if (cur_line_text.from_tile_id == cur_line_text.to_tile_id && cur_line_text.from_pos_idx == cur_line_text.to_pos_idx)
			{
				auto tangent_line = Line::cacl_tangent_clock_wise(config.center, from_point);
				pre_collection.line_texts.push_back(LineText(tangent_line, cur_line_text.utf8_text, cur_line_text.font_name, cur_line_text.font_size, cur_line_text.fill_color, cur_line_text.opacity));
				continue;
			}
			auto to_tile_iter = tiles.find(cur_line_text.to_tile_id);
			if (to_tile_iter == tiles.end())
			{
				continue;
			}
			const auto& to_tile = to_tile_iter->second;
			const auto& to_circle = circles[to_tile.circle_id];
			auto to_angle = to_tile.angle_begin + to_circle.angle_per_unit * cur_line_text.to_pos_idx;
			auto to_point = Point::radius_point(to_circle.outer_radius, amplify_angle::from_rad(to_angle), config.center);
			auto text_line = Line(from_point, to_point);
			pre_collection.line_texts.push_back(LineText(text_line, cur_line_text.utf8_text, cur_line_text.font_name, cur_line_text.font_size, cur_line_text.fill_color, cur_line_text.opacity));

		}

		// 8. 整理所有的value_on_tile 删除不在同一个circle上的track 剩余的数据在track内排序
		for (auto& one_track_data : all_value_on_tile_by_track)
		{
			// 清空所有不在同一个circle上的data
			std::string_view on_circle_id;
			bool invalid_track = false;
			for (const auto& one_value_data : one_track_data.second)
			{
				auto cur_tile_id = one_value_data.tile_id;
				auto cur_tile_iter = tiles.find(cur_tile_id);
				if (cur_tile_iter == tiles.end())
				{
					invalid_track == true;
					std::cout << "invalid tile " << cur_tile_id << " on track " << one_track_data.first << std::endl;
					break;
				}
				auto cur_circle_id = cur_tile_iter->second.circle_id;
				if (cur_circle_id.empty())
				{
					std::cout << " invalid data with empty circle_id for tile  " << cur_tile_id << " cur data id " << one_value_data.data_id << std::endl;
					invalid_track = true;
					break;
				}
				if (on_circle_id.empty())
				{
					on_circle_id = cur_circle_id;
				}
				if (cur_circle_id != on_circle_id)
				{
					std::cout << "data on track " << one_track_data.first << " has more than one circle " << cur_circle_id << " "<< on_circle_id<< std::endl;
					invalid_track = true;
					break;
				}
			}
			if (invalid_track)
			{
				one_track_data.second.clear();
				continue;
			}
			auto cur_circle_iter = circles.find(on_circle_id);
			if (cur_circle_iter == circles.end())
			{
				one_track_data.second.clear();
				continue;
			}
			const auto& cur_circle = cur_circle_iter->second;
			for (auto& one_value_data : one_track_data.second)
			{
				const auto& cur_tile = tiles[one_value_data.tile_id];
				one_value_data.angle = cur_tile.angle_begin + cur_circle.angle_per_unit * 0.5 * (one_value_data.begin_pos + one_value_data.end_pos);
			}
			std::sort(one_track_data.second.begin(), one_track_data.second.end(), [](const value_on_tile& a, const value_on_tile& b)
			{
				return a.angle < b.angle;
			});
		}
		// 9. 输出所有的point track
		for (const auto& one_track_config_iter : point_track_configs)
		{
			auto track_data_iter = all_value_on_tile_by_track.find(one_track_config_iter.first);
			if (track_data_iter == all_value_on_tile_by_track.end())
			{
				std::cout << "invalid point track " << one_track_config_iter.first << std::endl;
				continue;
			}
			const auto& cur_track_data = track_data_iter->second;
			if (cur_track_data.size() == 0)
			{
				continue;
			}
			auto cur_circle_id = tiles[cur_track_data[0].tile_id].circle_id;
			auto origin_circle_radius = circles[cur_circle_id].outer_radius;
			const auto& cur_track_config = one_track_config_iter.second;
			std::vector<Circle> temp_circles;
			for (const auto& one_point_data : cur_track_data)
			{
				float progress = 1.0;
				if (cur_track_config.clamp_data_value.first == cur_track_config.clamp_data_value.second)
				{
					progress = 1.0;
				}
				else
				{
					progress = (one_point_data.data_value - cur_track_config.clamp_data_value.first) / (cur_track_config.clamp_data_value.second - cur_track_config.clamp_data_value.first);
				}
				progress = progress > 1.0 ? 1.0 : progress;
				progress = progress < 0.0 ? 0.0 : progress;
				auto cur_circle_radius = cur_track_config.radius_offset.first * (1 - progress) + cur_track_config.radius_offset.second * progress + origin_circle_radius;
				auto cur_point_center = Point::radius_point(cur_circle_radius, amplify_angle::from_rad(one_point_data.angle)) + config.center;
				
				auto cur_point_color = Color(cur_track_config.clamp_color.first, cur_track_config.clamp_color.second, progress);
				int cur_point_size = cur_track_config.clamp_point_size.first * (1 - progress) + (cur_track_config.clamp_point_size.second) * progress;
				auto cur_circle = Circle(cur_point_size, cur_point_center, cur_point_color, 1.0, true);
				temp_circles.push_back(cur_circle);
				pre_collection.circles.push_back(cur_circle);

			}
			if (cur_track_config.link_width)
			{
				vector<Line> temp_lines;
				for (int i = 0; i < cur_track_data.size() - 1; i++)
				{
					auto temp_line = Line(temp_circles[i].center, temp_circles[i + 1].center, cur_track_config.link_color, cur_track_config.link_width, 1);
					auto last_rad = amplify_angle::from_rad(cur_track_data[i].angle);
					auto cur_rad = amplify_angle::from_rad(cur_track_data[i + 1].angle);
					pre_collection.lines.push_back(temp_line);
					if (cur_track_config.with_shadow && i != 0)
					{
						// 需要绘制阴影 就是arc和几条直线的闭合环路

						Region temp_region;
						temp_region.add_boundary(Arc(origin_circle_radius, last_rad, cur_rad, config.center, cur_track_config.link_color));
						temp_region.add_boundary(Line(Point::radius_point(origin_circle_radius, cur_rad) + config.center, temp_line.to, cur_track_config.link_color));
						temp_region.add_boundary(Line(temp_line.to, temp_line.from, temp_line.color, temp_line.width, temp_line.opacity));
						temp_region.add_boundary(Line(temp_line.from, Point::radius_point(origin_circle_radius, last_rad) + config.center, cur_track_config.link_color));
						
						temp_region.opacity = 1.0;
						temp_region.color = cur_track_config.link_color;
						temp_region.set_inner_point((Point::radius_point(origin_circle_radius, last_rad) + config.center + temp_line.to) * 0.5);
						pre_collection.regions.push_back(temp_region);
					}
					temp_lines.push_back(temp_line);
				}
				
			}
			
		}
		
	}
}