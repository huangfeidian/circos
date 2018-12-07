#include <circos/model.h>
#include <algorithm>
#include <numeric>
#include <unordered_map>
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
		unordered_map<string, vector<tile>> tiles_grouped_by_circle;
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
		unordered_map<string, int> circle_ranges;
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
				Tile cur_tile(config.center, cur_circle.inner_radius, cur_circle.outer_radius, amplify_angle::rad_to_angle_percent(temp_angle_begin), amplify_angle::rad_to_angle_percent(temp_angle_end), one_tile.fill_color, 1, one_tile.is_fill, one_tile.opacity);
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
			const auto& circle_id = one_tick.first;
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
			int total_count = circle_ranges[circle_id] / cur_circle.gap;
			double angle_by_unit = pi() * 2 / circle_ranges[circle_id];
			for(int i = 0; i < total_count; i++)
			{
				Line cur_tick_line = Line(Point::radius_point(cur_circle.outer_radius, amplify_angle::rad_to_angle_percent(i * cur_circle.gap * angle_by_unit)), Point::radius_point(cur_circle.outer_radius + cur_tick.height, amplify_angle::rad_to_angle_percent(i * cur_circle.gap * angle_by_unit)), cur_tick.fill_color, cur_tick.width, cur_tick.opacity);
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
				Line cur_line = Line(Point::radius_point(from_circle.inner_radius, amplify_angle::rad_to_angle_percent(from_tile.angle_begin + from_circle.angle_per_unit * cur_point_link.from_pos_idx)), Point::radius_point(to_circle.outer_radius, amplify_angle::rad_to_angle_percent(to_tile.angle_begin + to_circle.angle_per_unit * cur_point_link.to_pos_idx)), cur_point_link.fill_color, cur_point_link.width, cur_point_link.opacity);
			}
			else
			{
				// 这个是曲线
				auto from_angle = from_tile.angle_begin + from_circle.angle_per_unit * cur_point_link.from_pos_idx;
				auto to_angle = to_tile.angle_begin + to_circle.angle_per_unit * cur_point_link.to_pos_idx;
				if(to_tile.circle_id == from_tile.circle_id)
				{
					Bezier cur_bezier = Bezier(config.center, from_circle.inner_radius, amplify_angle::rad_to_angle_percent(from_angle), amplify_angle::rad_to_angle_percent(to_angle), cur_point_link.fill_color, from_circle.inner_radius * cur_point_link.control_radius_percent, cur_point_link.width, cur_point_link.opacity);
					pre_collection.beziers.push_back(cur_bezier);
				}
				else
				{
					auto control_point = Point::radius_point((from_circle.inner_radius + to_circle.inner_radius) / 2 * cur_point_link.control_radius_percent, amplify_angle::rad_to_angle_percent((from_angle + to_angle) / 2), config.center);
					Bezier cur_bezier = Bezier(Point::radius_point(from_circle.inner_radius, amplify_angle::rad_to_angle_percent(from_angle), config.center), Point::radius_point(to_circle.inner_radius, amplify_angle::rad_to_angle_percent(to_angle), config.center), control_point, cur_point_link.fill_color, cur_point_link.opacity);
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
			Ribbon cur_ribbon(config.center, from_circle.inner_radius, to_circle.inner_radius, amplify_angle::rad_to_angle_percent(from_begin_angle), amplify_angle::rad_to_angle_percent(from_end_angle), amplify_angle::rad_to_angle_percent(to_begin_angle), amplify_angle::rad_to_angle_percent(to_end_angle), cur_range_link.fill_color, cur_range_link.control_radius_percent, cur_range_link.is_cross, cur_range_link.opacity < 0.001, cur_range_link.opacity);
			pre_collection.ribbons.push_back(cur_ribbon);
		}

	}
}