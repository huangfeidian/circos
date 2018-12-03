//#include <circos/drawer/png_drawer.h>
//#include <circos/drawer/svg_drawer.h>
//#include <map>
//#include <vector>
//#include <fstream>
//#include <streambuf>
//#include <algorithm>
//#include <numeric>
//#include <circos/basics/color.h>
//#include <random>
//#include <algorithm>
//
//using namespace std;
//using namespace circos;
//void pi_test_1(void)
//{
//	ifstream pi_file("../data/pi10000.txt");
//    string pi_str((istreambuf_iterator<char>(pi_file)), istreambuf_iterator<char>());
//    vector<char> pi_digits;
//	std::unordered_map<string, pair<string, string>> font_info{ { "yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei") } };
//    pi_digits.reserve(pi_str.size());
//    int i = 0;
//    for (; i < pi_str.size() - 1; i++)
//    {
//        if (pi_str[i] == '3' && pi_str[i + 1] == '.')
//        {
//            break;
//        }
//    }
//    i += 1;
//    for (; i < pi_str.size(); i++)
//    {
//		if (pi_str[i] >= '0' && pi_str[i] <= '9')
//		{
//			pi_digits.push_back(pi_str[i] - '0');
//		}
//        
//    }
//    i = 0;
//    vector<int> chunk_size(10,0); // 记录每个块的大小
//    chunk_size[3] = 1;
//    vector<vector<pair<int, pair<int,int>>>> connections(10, vector<pair<int, pair<int,int>>>()); // 记录每个块到其他块的链接
//    int pre_char = 3;
//    for(i = 0; i<pi_digits.size(); i++)
//    {
//        auto cur_char = pi_digits[i];
//        chunk_size[cur_char] += 1;
//        if(cur_char != pre_char)
//        {
//            connections[pre_char].push_back(make_pair(chunk_size[pre_char] -1, make_pair(cur_char, chunk_size[cur_char] -1)));
//        }
//        pre_char = cur_char;
//    }
//    // 位置处理结束 开始生成所有链接
//    float gap = PI/4;
//    auto total_size = std::accumulate(chunk_size.begin(), chunk_size.end(), 0);
//    float angle_per_size = (2*PI - gap)/total_size;
//    vector<circos::Color> strand_color(10, Color());
//    strand_color[0] = Color(15, 66, 90);
//    strand_color[1] = Color(50, 163, 231);
//    strand_color[2] = Color(77, 119, 25);
//    strand_color[3] = Color(70, 207, 110);
//    strand_color[4] = Color(105, 4, 4);
//    strand_color[5] = Color(201, 15, 25);
//    strand_color[6] = Color(148, 82, 0);
//    strand_color[7] = Color(207, 105, 3);
//    strand_color[8] = Color(63, 30, 78);
//    strand_color[9] = Color(140, 81, 195);
//  //  vector<circos::strand> all_strands(10, circos::strand());
//  //  int _pre_total_size = 0;
//  //  for(i =0; i< 10; i++)
//  //  {
//  //      float angle_begin = i*gap/10 + _pre_total_size*angle_per_size;
//  //      all_strands[i].strand_color = strand_color[i];
//  //      all_strands[i].begin = 0;
//  //      all_strands[i].end = chunk_size[i] - 1;
//  //      all_strands[i].angle_begin = angle_begin;
//  //      all_strands[i].angle_end = angle_begin + (chunk_size[i]-1)*angle_per_size;
//  //      all_strands[i].opacity = 1.0;
//		//_pre_total_size += chunk_size[i];
//  //  }
//    int radius = 1000;
//    Point center(radius, radius);
//    float inner_radius_ratio = 0.8;
//    float outer_radius_ratio = 0.85;
//	float control_radius_ration = 0.3;
//    int inner_radius = inner_radius_ratio*radius;
//    int outer_radius = outer_radius_ratio*radius;
//	int control_radius = control_radius_ration*radius;
//    Color background_color = Color(0,0,0);
//    string svg_filename = "circos_pi_test_1.svg";
//    string png_filename = "circos_pi_test_1.png";
//    // std::unordered_map<string, pair<string, string>> font_info;
//    circos::SvgGraph svg_graph(font_info, svg_filename, radius, background_color);
//    circos::PngImage png_image(font_info, png_filename, radius, background_color);
//	// draw strands
//    //for(int i=0; i<10; i++)
//    //{
//    //    Ring cur_ring(center, inner_radius, outer_radius, all_strands[i].angle_begin, all_strands[i].angle_end, all_strands[i].strand_color, 1, true);
//    //    svg_graph<<cur_ring;
//    //    png_image<<cur_ring;
//    //}
//	// draw bezier
//	//for (int i = 0; i < 10; i++)
//	//{
//	//	for (const auto& link : connections[i])
//	//	{
//	//		float from_angle = all_strands[i].position_to_angle(link.first);
//	//		float to_angle = all_strands[link.second.first].position_to_angle(link.second.second);
//	//		Bezier connection(center, inner_radius, from_angle, to_angle, strand_color[i], control_radius, 1, 0.1);
//	//		svg_graph << connection;
//	//		png_image << connection;
//
//	//	}
//	//}
//
//	// draw histogram
//	// draw ticks
//	//float line_radius_ratio = 0.95;
//	//int line_radius = radius*line_radius_ratio;
//	//Color tick_color(255, 0, 0);
//	//int tick_thickness = 1;
//	//for (int i = 0; i < 10; i++)
//	//{
//	//	int gap = 0;
//	//	for (const auto& link : connections[i])
//	//	{
//	//		gap = (gap + 1) ;
//	//		if (gap % 10 == 0)
//	//		{
//	//			float from_angle = all_strands[i].position_to_angle(link.first);
//	//			float tick_hight_ratio = 0.5;
//	//			int tick_out_radius = outer_radius*(1 - tick_hight_ratio) + line_radius*tick_hight_ratio;
//	//			Line tick_line(radius_point(outer_radius, from_angle, center), radius_point(tick_out_radius, from_angle, center), tick_color, tick_thickness);
//	//			svg_graph << tick_line;
//	//			png_image << tick_line;
//	//		}
//	//		if (gap % 50 == 0)
//	//		{
//	//			float from_angle = all_strands[i].position_to_angle(link.first);
//	//			float tick_hight_ratio = 1;
//	//			int tick_out_radius = outer_radius*(1 - tick_hight_ratio) + line_radius*tick_hight_ratio;
//	//			Line tick_line(radius_point(outer_radius, from_angle, center), radius_point(tick_out_radius, from_angle, center), tick_color, tick_thickness);
//	//			svg_graph << tick_line;
//	//			png_image << tick_line;
//	//		}
//	//	}
//	//}
//	// draw line chart
//	//float line_radius_ratio = 0.95;
//	//int line_radius = radius*line_radius_ratio;
//	//Color tick_color(255, 0, 0);
//	//int tick_thickness = 2;
//	//for (int i = 0; i < 10; i++)
//	//{
//	//	int gap = 0;
//	//	Point pre_point = radius_point(outer_radius, all_strands[i].angle_begin, center);
//	//	for (const auto& link : connections[i])
//	//	{
//	//		gap = (gap + 1) % 10;
//	//		if (gap == 0)
//	//		{
//	//			float from_angle = all_strands[i].position_to_angle(link.first);
//	//			float tick_hight_ratio = link.second.first *0.1;
//	//			int tick_out_radius = outer_radius*(1 - tick_hight_ratio) + line_radius*tick_hight_ratio;
//	//			Point cur_point = radius_point(tick_out_radius, from_angle, center);
//	//			Line tick_line(pre_point, cur_point, tick_color, tick_thickness);
//	//			svg_graph << tick_line;
//	//			png_image << tick_line;
//	//			pre_point = cur_point;
//	//		}
//	//	}
//	//}
//
//
//	// draw label
//	int label_font_size = 64;
//	Color font_color(255, 255, 255);
//	for (int i = 0; i < 10; i++)
//	{
//		float angle = (all_strands[i].angle_begin + all_strands[i].angle_end) / 2;
//		Line on_line(radius_point(outer_radius, angle, center), radius_point(line_radius, angle, center));
//		LineText line_text(on_line, to_string(i), "yahei", label_font_size, font_color);
//		svg_graph << line_text;
//		png_image << line_text;
//	}
//	// draw track
//	vector<int> basic_pool{ 0,1,2,3,4,5,6,7,8,9, };
//	random_device rd;
//	mt19937 g(rd());
//	for (int i = 0; i < 100; i++)
//	{
//		shuffle(basic_pool.begin(), basic_pool.end(), g);
//		int from_idx = basic_pool[0];
//		int to_idx = basic_pool[1];
//
//	}
//}
