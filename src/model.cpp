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
			if(cur_circle.opacity <= 0.001)
			{
				continue;
			}
			auto cur_ring = Ring(config.center, cur_circle.inner_radius, cur_circle.outer_radius, 0, pi() * 2, cur_circle.fill_color, 1, true, cur_circle.opacity);
			pre_collection.rings.push_back(cur_ring);
		}
		// 2. 然后计算每个圆环上每条带子的位置
		unordered_map<string_view, vector<band>> bands_grouped_by_circle;
		for(const auto& i: bands)
		{
			auto circle_iter = circles.find(i.second.circle_id);
			if(circle_iter == circles.end())
			{
				continue;
			}
			auto& pre_groups = bands_grouped_by_circle[i.second.circle_id];
			pre_groups.push_back(i.second);
		}
		// 分好组之后 再排序然后处理
		unordered_map<string_view, int> circle_ranges;
		for(auto& i: bands_grouped_by_circle)
		{
			auto& cur_circle = circles.find(i.first)->second;
			auto& sorted_vector = i.second;
			sort(sorted_vector.begin(), sorted_vector.end(), [](const band& a, const band& b)
			{
				return a.sequence < b.sequence;
			});
			int total_len = (sorted_vector.size() - 1) * cur_circle.gap;
			accumulate(sorted_vector.begin(), sorted_vector.end(), total_len, [](int pre_total, const band& cur_band)
			{
				return pre_total + cur_band.width;
			});
			circle_ranges[i.first] = total_len;

			double temp_angle_begin = 0;
			double temp_angle_end = 0;
			double angle_by_unit = pi() * 2 / total_len;
			cur_circle.angle_per_unit = angle_by_unit;
			for(auto& one_band: sorted_vector)
			{	
				one_band.angle_begin = temp_angle_begin;
				temp_angle_end = temp_angle_begin + angle_by_unit * one_band.width;
				Ring cur_ring = Ring(config.center, cur_circle.inner_radius, cur_circle.outer_radius, temp_angle_begin, temp_angle_end, one_band.fill_color, 1, one_band.opacity <= 0.001, one_band.opacity);
				pre_collection.rings.push_back(cur_ring);
				temp_angle_begin += cur_circle.gap * angle_by_unit;
			}
		}

		// 重新赋值一下bands
		bands.clear();
		for(const auto& i: bands_grouped_by_circle)
		{
			for(const auto& one_band: i.second)
			{
				bands[one_band.band_id] = one_band;
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
				Line cur_tick_line = Line(Point::radius_point(cur_circle.outer_radius, i * cur_circle.gap * angle_by_unit), Point::radius_point(cur_circle.outer_radius + cur_tick.height, i * cur_circle.gap * angle_by_unit), cur_tick.fill_color, cur_tick.width, cur_tick.opacity);
				pre_collection.lines.push_back(cur_tick_line);
			}
		}

		// 4. 开始处理两点之间的连线
		for(const auto& one_point_link: point_links)
		{
			const auto& link_id = one_point_link.first;
			const auto& cur_point_link = one_point_link.second;
			auto from_band_iter = bands.find(cur_point_link.from_band_id);
			if(from_band_iter == bands.end())
			{
				continue;
			}
			const auto& from_band = from_band_iter->second;
			auto to_band_iter = bands.find(cur_point_link.to_band_id);
			if(to_band_iter == bands.end())
			{
				continue;
			}
			const auto& to_band = to_band_iter->second;
			const auto& from_circle = circles[from_band.circle_id];
			const auto& to_circle = circles[to_band.circle_id];
			if(cur_point_link.control_radius_percent <= 0)
			{
				// 这个是直线
				Line cur_line = Line(Point::radius_point(from_circle.inner_radius, from_band.angle_begin + from_circle.angle_per_unit * cur_point_link.from_pos_idx), Point::radius_point(to_circle.outer_radius, to_band.angle_begin + to_circle.angle_per_unit * cur_point_link.to_pos_idx), cur_point_link.fill_color, cur_point_link.width, cur_point_link.opacity);
			}
			else
			{
				// 这个是曲线
				auto from_angle = from_band.angle_begin + from_circle.angle_per_unit * cur_point_link.from_pos_idx;
				auto to_angle = to_band.angle_begin + to_circle.angle_per_unit * cur_point_link.to_pos_idx;
				if(to_band.circle_id == from_band.circle_id)
				{
					Bezier cur_bezier = Bezier(config.center, from_circle.inner_radius, from_angle, to_angle, cur_point_link.fill_color, from_circle.inner_radius * cur_point_link.control_radius_percent, cur_point_link.width, cur_point_link.opacity);
					pre_collection.beziers.push_back(cur_bezier);
				}
				else
				{
					auto control_point = Point::radius_point((from_circle.inner_radius + to_circle.inner_radius) / 2 * cur_point_link.control_radius_percent, (from_angle + to_angle) / 2, config.center);
					Bezier cur_bezier = Bezier(Point::radius_point(from_circle.inner_radius, from_angle, config.center), Point::radius_point(to_circle.inner_radius, to_angle, config.center), control_point, cur_point_link.fill_color, cur_point_link.opacity);
					pre_collection.beziers.push_back(cur_bezier);
				}
			}
		}
		// 5. 处理两个条带之间的连线
		for(const auto& one_range_link: range_links)
		{
			const auto& cur_range_link = one_range_link.second;
			auto from_band_iter = bands.find(cur_range_link.from_band_id);
			if(from_band_iter == bands.end())
			{
				continue;
			}
			const auto& from_band = from_band_iter->second;
			auto to_band_iter = bands.find(cur_range_link.to_band_id);
			if(to_band_iter == bands.end())
			{
				continue;
			}
			const auto& to_band = to_band_iter->second;
			const auto& from_circle = circles[from_band.circle_id];
			const auto& to_circle = circles[to_band.circle_id];
			auto from_begin_angle = from_band.angle_begin + from_circle.angle_per_unit * cur_range_link.from_pos_begin_idx;
			auto from_end_angle = from_band.angle_begin + from_circle.angle_per_unit * cur_range_link.from_pos_end_idx;
			auto to_begin_angle = to_band.angle_begin + to_circle.angle_per_unit * cur_range_link.to_pos_begin_idx;
			auto to_end_angle = to_band.angle_begin + to_circle.angle_per_unit * cur_range_link.to_pos_end_idx;
			Track cur_track = Track(config.center, from_circle.inner_radius, to_circle.inner_radius, from_begin_angle, from_end_angle, to_begin_angle, to_end_angle, cur_range_link.fill_color, cur_range_link.control_radius_percent, cur_range_link.is_cross, cur_range_link.opacity < 0.001, cur_range_link.opacity);
			pre_collection.tracks.push_back(cur_track);
		}

	}
}