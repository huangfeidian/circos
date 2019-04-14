#include <circos/drawer/png_drawer.h>
#include <circos/drawer/svg_drawer.h>
#include <circos/model.h>
#include <map>
#include <vector>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <numeric>
#include <circos/basics/color.h>
#include <random>
#include <algorithm>
#include <memory>
#include <string_view>

using namespace std;
using namespace circos;
void pi_test_1(void)
{
	ifstream pi_file("../data/pi100000.txt");
	string pi_str((istreambuf_iterator<char>(pi_file)), istreambuf_iterator<char>());
	vector<uint8_t> pi_digits;
	std::unordered_map<string_view, pair<string, string>> font_info{ { "yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei") } };
	pi_digits.reserve(pi_str.size());
	int i = 0;
	for (; i < pi_str.size() - 1; i++)
	{
		if (pi_str[i] == '3' && pi_str[i + 1] == '.')
		{
			break;
		}
	}
	i += 1;
	for (; i < pi_str.size(); i++)
	{
		if (pi_str[i] >= '0' && pi_str[i] <= '9')
		{
			pi_digits.push_back(pi_str[i] - '0');
		}

	}
	vector<std::unique_ptr<string>> all_temp_string;
	i = 0;
	vector<int> chunk_size(10, 0); // 记录每个块的大小
	chunk_size[3] = 1;
	vector<vector<pair<int, pair<int, int>>>> connections(10, vector<pair<int, pair<int, int>>>()); // 记录每个块到其他块的链接
	int pre_char = 3;
	for (i = 0; i < pi_digits.size(); i++)
	{
		auto cur_char = pi_digits[i];
		chunk_size[cur_char] += 1;
		if (cur_char != pre_char)
		{
			connections[pre_char].push_back(make_pair(chunk_size[pre_char] - 1, make_pair(cur_char, chunk_size[cur_char] - 1)));
		}
		pre_char = cur_char;
	}
	// 位置处理结束 开始生成所有链接
	float gap = pi() / 4;
	auto total_size = std::accumulate(chunk_size.begin(), chunk_size.end(), 0);
	float angle_per_size = (2 * pi() - gap) / total_size;
	vector<circos::Color> strand_color(10, Color());
	strand_color[0] = Color(15, 66, 90);
	strand_color[1] = Color(50, 163, 231);
	strand_color[2] = Color(77, 119, 25);
	strand_color[3] = Color(70, 207, 110);
	strand_color[4] = Color(105, 4, 4);
	strand_color[5] = Color(201, 15, 25);
	strand_color[6] = Color(148, 82, 0);
	strand_color[7] = Color(207, 105, 3);
	strand_color[8] = Color(63, 30, 78);
	strand_color[9] = Color(140, 81, 195);

	int radius = 2000;
	float inner_radius_ratio = 0.8;
	float outer_radius_ratio = 0.85;
	float control_radius_ration = 0.3;
	int inner_radius = inner_radius_ratio * radius;
	int outer_radius = outer_radius_ratio * radius;
	int control_radius = control_radius_ration * radius;

	model::model pi_model;
	model::circle cur_circle;
	all_temp_string.push_back(std::make_unique<std::string>("pi"));
	cur_circle.circle_id = std::string_view(all_temp_string.back()->data(), all_temp_string.back()->size());
	cur_circle.inner_radius = inner_radius;
	cur_circle.outer_radius = outer_radius;
	cur_circle.gap = pi_str.size() / 10;
	cur_circle.fill_color = Color(125, 145, 130);
	cur_circle.opacity = 0.0;
	pi_model.circles[cur_circle.circle_id] = cur_circle;

	int id_count = 1;

	vector<model::tile> tiles(10, model::tile());

	int _pre_total_size = 0;
	for (i = 0; i < 10; i++)
	{
		tiles[i].circle_id = cur_circle.circle_id;
		all_temp_string.push_back(std::make_unique<std::string>(to_string(i)));
		tiles[i].tile_id = std::string_view(all_temp_string.back()->data(), all_temp_string.back()->size());
		tiles[i].width = chunk_size[i];
		tiles[i].fill_color = strand_color[i];
		tiles[i].opacity = 1.0;
		tiles[i].sequence = i;
		bool is_fill = true;
		tiles[i].is_fill = is_fill;
		pi_model.tiles[to_string(i)] = tiles[i];
	}
	for (int i = 0; i < 10; i++)
	{
		string_view from_tile_id = tiles[i].tile_id;
		for (const auto& one_link : connections[i])
		{
			auto from_pos_idx = one_link.first;
			auto to_tile_id = tiles[one_link.second.first].tile_id;
			auto to_pos_idx = one_link.second.second;
			model::point_link temp_link;
			all_temp_string.push_back(std::make_unique<std::string>(to_string(id_count++)));
			temp_link.link_id = std::string_view(all_temp_string.back()->data(), all_temp_string.back()->size());
			temp_link.from_tile_id = from_tile_id;
			temp_link.from_pos_idx = from_pos_idx;
			temp_link.to_tile_id = to_tile_id;
			temp_link.to_pos_idx = to_pos_idx;
			temp_link.control_radius_percent = control_radius_ration;
			temp_link.fill_color = strand_color[one_link.second.first];
			temp_link.opacity = 0.1;
			temp_link.width = 1;
			pi_model.point_links[temp_link.link_id] = temp_link;
		}

	}

	Point center(radius, radius);

	Color background_color = Color(255, 255, 255);
	pi_model.config.background_color = background_color;
	pi_model.config.center = center;
	pi_model.config.radius = radius;
	string svg_filename = "circos_pi_test_1.svg";
	string png_filename = "circos_pi_test_1.png";
	// std::unordered_map<string, pair<string, string>> font_info;
	circos::SvgGraph svg_graph(font_info, svg_filename, radius, background_color);
	circos::PngImage png_image(font_info, png_filename, radius, background_color);
	shape_collection cur_collection;
	pi_model.to_shapes(cur_collection);
	draw_collections(svg_graph, cur_collection);
	draw_collections(png_image, cur_collection);

}
