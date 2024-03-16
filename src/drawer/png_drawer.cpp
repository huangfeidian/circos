#include <unordered_map>
#include <string>
#include <sstream>
#include <circos/drawer/png_drawer.h>
#include <circos/shape_collection.h>
#include <iostream>
#include <fstream>
#include <circos/basics/utf8_util.h>

namespace spiritsaway::circos
{
	using namespace std;
	PngImage::PngImage(const unordered_map<std::string, pair<std::string, std::string>>& in_font_info, string in_file_name, int in_radius, Color back_color, int compress)
	: ft_wrapper(in_font_info)
	,file_name(in_file_name)
	, radius(in_radius)
	, width(2*in_radius)
	, height(2*in_radius)
	, background_color(back_color)
	, _compression_level(compress)
	, _image(2*in_radius, nullptr)
	{
		flood_buffer = vector<uint8_t>(width*height, 0);
		flood_map = vector<uint8_t*>(height, nullptr);
		auto begin = &flood_buffer[0];
		for (int i = 0;i < height;i++)
		{
			flood_map[i] = begin + i*width;
		}
		_buffer = vector<Color>(width*height,Color());
		for(int i=0;i<height;i++)
		{
			_image[i]=&_buffer[0]+i*width;
		}
		for(int i=0;i<width*height;i++)
		{
			_buffer[i]=background_color;
		}

		
	}


	void PngImage::plot(Colorbasic_point input, float opacity)
	{
		_image[input.pos.y][input.pos.x].blend(input.color, opacity);
	}
	void PngImage::plot(Point pos, Color color,float opacity)
	{
		_image[pos.y][pos.x].blend(color,opacity);
	}
	void PngImage::plot(int x, int y, Color color, float opacity)
	{
		x = x % width;
		y = y % height;
		_image[y][x].blend(color, opacity);
	}
	Color PngImage::read(Point pos)
	{
		return _image[pos.y][pos.x];
	}
	void PngImage::draw_path(const vector<Point>& path, Color color, int stroke, float opacity)
	{
		if (stroke == 1)
		{
			for(const auto& i:path)
			{
				plot(i.x, i.y, color, opacity);
			}
			return;
		}
		int offset_left = (stroke / 2) * -1;
		int offset_right = (stroke + 1) / 2;
		vector<Point> final_path;
		final_path.reserve(stroke * stroke * path.size());
		for (const auto& i : path)
		{
			for (int j = offset_left;j < offset_right;j++)
			{
				for (int k = offset_left;k < offset_right;k++)
				{
					auto cur_point = Point(i.x + j, i.y + k);
					if (get_flood_flag(cur_point) != 1)
					{
						set_flood_flag(cur_point, 1);
						final_path.push_back(cur_point);
					}
				}
			}
		}
		for (const auto& i : final_path)
		{
			plot(i, color, opacity);
			set_flood_flag(i, 0);
		}
	}
	PngImage::~PngImage()
	{
		double filegama_ = 0.5;
		FILE* fp = fopen(file_name.c_str(), "wb");
		auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		auto info_ptr = png_create_info_struct(png_ptr);
		png_init_io(png_ptr, fp);
		png_set_compression_level(png_ptr, _compression_level);
		png_set_IHDR(png_ptr,info_ptr,width,height,bit_depth,PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		png_set_gAMA(png_ptr, info_ptr, filegama_);
		time_t gmt;
		png_time mod_time;
		int entries = 4;
		time(&gmt);
		png_convert_from_time_t(&mod_time, gmt);
		png_set_tIME(png_ptr, info_ptr, &mod_time);
		/* key is a 1-79 character description of type char* */
		png_text text_ptr[5];
		char key_title[] = "Title";
		text_ptr[0].key = key_title;
		text_ptr[0].text = const_cast<char*>(title.c_str());
		text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
		char key_author[] = "Author";
		text_ptr[1].key = key_author;
		text_ptr[1].text = const_cast<char*>(author.c_str());
		text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
		char key_descr[] = "Description";
		text_ptr[2].key = key_descr;
		text_ptr[2].text = const_cast<char*>(description.c_str());
		text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
		char key_software[] = "Software";
		text_ptr[3].key = key_software;
		text_ptr[3].text = const_cast<char*>(software.c_str());
		text_ptr[3].compression = PNG_TEXT_COMPRESSION_NONE;
		#if defined(PNG_TIME_RFC1123_SUPPORTED)
			char key_create[] = "Creation Time";
			text_ptr[4].key = key_create;
			char textcrtime[29] = "tIME chunk is not present...";
			#if (PNG_LIBPNG_VER < 10600)
				memcpy(textcrtime,
						png_convert_to_rfc1123(png_ptr, &mod_time),
						29);
			#else
				png_convert_to_rfc1123_buffer(textcrtime, &mod_time);
			#endif
			textcrtime[sizeof(textcrtime) - 1] = '\0';
			text_ptr[4].text = textcrtime;
			text_ptr[4].compression = PNG_TEXT_COMPRESSION_NONE;
			entries++;
		#endif
		png_set_text(png_ptr, info_ptr, text_ptr, entries);

		png_write_info(png_ptr, info_ptr);
		png_write_image(png_ptr, reinterpret_cast<unsigned char**>(&_image[0]));
		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
	}
	bool PngImage::on_boundary(Point current) const
	{
		if (current.x == 0 || current.x == width - 1 || current.y == 0 || current.y == height - 1)
		{
			return true;
		}
		return false;
	}
	std::uint8_t PngImage::get_flood_flag(Point current) const
	{
		return flood_map[current.y][current.x];
	}
	void PngImage::set_flood_flag(Point current, std::uint8_t flag)
	{
		flood_map[current.y][current.x] = flag;
	}
	bool PngImage::can_flood(Point current)
		//判断一个点是否可以将他的相邻点加入洪范列表
		//这里为了保险起见才加了这个判断函数
		//只要有一个处于边界就停止，避免可能的边界没有完全闭合的情况
	{
		if (on_boundary(current))
		{
			return false;
		}
		Point up(current.x, current.y + 1);
		Point down(current.x, current.y - 1);
		Point left(current.x - 1, current.y);
		Point right(current.x + 1, current.y);
		if (get_flood_flag(up) == 2 || get_flood_flag(down) == 2 || get_flood_flag(left) == 2 || get_flood_flag(right) == 2)
		{
			return false;
		}
		
		return true;

	}
	void PngImage::flood(const vector<vector<Point>>& boundary, vector<Point> interiors, Color fill_color, float opacity)
		//要求interior一定在boundary里面
	{
		// 首先补全缺失的点
		vector<Point> fill_points;
		vector<int> fill_index;
		int count = 0;
		for(std::uint32_t i = 0; i < boundary.size(); i++)
		{
			if(boundary[i].size())
			{
				fill_index.push_back(i);
				//cout << "begin_point " << boundary[i].front().x << " "<<boundary[i].front().y << " end point " << boundary[i].back().x << " " << boundary[i].back().y << endl;
			}
		}
		if (fill_index.size())
		{
			fill_index.push_back(fill_index[0]);
		}
		for(std::uint32_t i = 0; i< fill_index.size() - 1; i++)
		{
			auto pre_point = boundary[fill_index[i]].back();
			auto next_point = boundary[fill_index[i + 1]].front();
			auto gap_points = Line::connect_points(pre_point, next_point);

			for (const auto& one_gap_point : gap_points)
			{
				set_flood_flag(one_gap_point, 2);
				plot(one_gap_point, fill_color, opacity);
				fill_points.push_back(one_gap_point);
			}
			//if (gap_points.size() > 10)
			//{
			//	cout << "new point:" << gap_points[0].x << " " << gap_points[0].y << endl;
			//	cout << "new point:" << gap_points.back().x << " " << gap_points.back().y << endl;
			//}
		}
		for (const auto& one_boundary : boundary)
		{
			for (const auto& i : one_boundary)
			{
				set_flood_flag(i, 2);
				fill_points.push_back(i);
			}
		}
		//下面的这两个点一定在ribbon内部
		deque<Point> all_points;
		for (auto one_point : interiors)
		{
			all_points.push_back(one_point);
		}
		std::uint32_t access_count = 0;
		while (!all_points.empty())
		{
			auto current = all_points.front();
			all_points.pop_front();
			access_count++; 
			if (get_flood_flag(current) != 0 || on_boundary(current))
			{
				continue;
			}
			fill_points.push_back(current);
			plot(current, fill_color, opacity);
			count += 1;
			set_flood_flag(current,  1);
			auto flood_flag = can_flood(current);
			if (flood_flag)
			{
				all_points.push_back(Point(current.x, current.y + 1));
				all_points.push_back(Point(current.x, current.y - 1));
				
				all_points.push_back(Point(current.x + 1, current.y));
				all_points.push_back(Point(current.x - 1, current.y));
			}
			
		}
		// 这里可能有一些边界附近的节点漏了没有填充 补充填充一下
		for (const auto& one_boundary : boundary)
		{
			for (const auto& current : one_boundary)
			{

				Point temp_points[4];
				temp_points[0] = Point(current.x, current.y + 1);
				temp_points[1] = Point(current.x, current.y - 1);
				temp_points[2] = Point(current.x - 1, current.y);
				temp_points[3] = Point(current.x + 1, current.y);
				//temp_points[4] = Point(current.x + 1, current.y + 1);
				//temp_points[5] = Point(current.x + 1, current.y - 1);
				//temp_points[6] = Point(current.x - 1, current.y + 1);
				//temp_points[7] = Point(current.x + 1, current.y + 1);
				for (auto one_point : temp_points)
				{
					if (on_boundary(one_point))
					{
						continue;
					}
					if (get_flood_flag(one_point) != 0)
					{
						continue;
					}
					if(get_flood_flag(Point(one_point.x, one_point.y + 1)) ==1 || \
						get_flood_flag(Point(one_point.x, one_point.y - 1)) == 1|| \
						get_flood_flag(Point(one_point.x + 1, one_point.y)) == 1 || \
						get_flood_flag(Point(one_point.x - 1, one_point.y)) == 1)
					{
						plot(one_point, fill_color, opacity);
						set_flood_flag(one_point ,1);
						fill_points.push_back(one_point);
					}

				}
			}
		}
		for (const auto& i : fill_points)
		{
			set_flood_flag(i ,0);
		}
		//cout << "flood count " << count <<" access count "<<access_count<< endl;
	}

	void PngImage::draw_text(const Line& base_line, std::vector<std::uint32_t> text, const std::string& font_name, int font_size, Color color, float alpha)
		//这里要处理一下utf8
	{
		auto result =  ft_wrapper.draw_text_along_line(base_line.from.to_pair(), base_line.to.to_pair(), text, font_size, font_name);
		draw_grey_map(result, color, alpha);
	}

	void PngImage::draw_grey_map(const std::vector<std::pair<std::pair<std::int32_t, std::int32_t>, std::uint8_t>>& grey_bitmap, Color color, float alpha)
	{
		for (auto[pos, grey_v] : grey_bitmap)
		{
			if (pos.first < 0 || pos.second < 0)
			{
				continue;
			}
			plot(pos.first, pos.second, color, alpha * (grey_v / 256.0));
		}
	}
	PngImage& PngImage::operator<<(const LineText& line_text)
	{

		draw_text(line_text.on_line, spiritsaway::string_util::utf8_util::utf8_to_uint(line_text.utf8_text), line_text.font_name, line_text.font_size, line_text.color, line_text.opacity);

		return *this;
	}
	PngImage& PngImage::operator<<(const ArcText& arc_text)
	{
		// 这里就简单的分割为多个线段组成的文字
		auto text = spiritsaway::string_util::utf8_util::utf8_to_uint(arc_text.utf8_text);
		auto begin_rad = arc_text.on_arc.from_angle();
		auto total_width = arc_text.on_arc.width;
		if (text.empty())
		{
			return *this;
		}
		auto rad_per_text = free_angle(total_width) / text.size();
		auto ratio = arc_text.on_arc.reverse_end ? -1: 1;
		for (auto one_text : text)
		{
			auto begin_point = Point::radius_point(arc_text.on_arc.radius, begin_rad, arc_text.on_arc.center);
			begin_rad = begin_rad + rad_per_text * ratio;
			auto end_point = Point::radius_point(arc_text.on_arc.radius, begin_rad, arc_text.on_arc.center);
			auto temp_line = Line(begin_point, end_point);
			auto temp_textes = std::vector<std::uint32_t>();
			temp_textes.push_back(one_text);
			draw_text(temp_line, temp_textes, arc_text.font_name, arc_text.font_size, arc_text.color, arc_text.opacity);
		}
		return *this;
	}

	vector<Point> PngImage::path(const Line& line) const
	{
		return line.path();
	}
	PngImage& PngImage::operator<<( const Line& line)
	{
		if (line.width <= 4)
		{
			draw_path(path(line), line.color, line.width, line.opacity);
		}
		else
		{
			// convert to rect
			int pre_diff_y = line.to.y - line.from.y;
			int pre_diff_x = line.to.x - line.from.x;
			int new_diff_x = pre_diff_y * -1;
			int new_diff_y = pre_diff_x;
			int width_x = static_cast<int>(new_diff_x * line.width * 0.5/ sqrtf(new_diff_x * new_diff_x + new_diff_y * new_diff_y));
			int width_y = static_cast<int>(new_diff_y * line.width * 0.5/ sqrtf(new_diff_x * new_diff_x + new_diff_y * new_diff_y));

			Point rect_left(line.from.x + width_x, line.from.y + width_y);
			Point rect_right(line.from.x - width_x, line.from.y - width_y);
			Rectangle cur_rect(rect_left, rect_right, line.color, line.len(), true, line.opacity);
			*this << cur_rect;
		}
		return *this;
		
	}
	vector<Point> PngImage::path(const Arc& arc) const
	{
		return arc.path();
	}
	PngImage& PngImage::operator<<(const Arc& arc)
	{
		auto arc_points = path(arc);
		draw_path(arc_points, arc.color, arc.stroke, arc.opacity);
		if (arc.fill_flag)
		{
			vector<vector<Point>> flood_points;
			auto line_1 = Line(arc.center, arc.from_point(), arc.color);
			auto line_2 = Line(arc.to_point(), arc.center, arc.color);
			*this << line_1 << line_2;
			auto path_line1 = path(line_1);
			auto path_line2 = path(line_2);
			Line::connect_paths(flood_points, path_line1);
			Line::connect_paths(flood_points, arc_points);
			Line::connect_paths(flood_points, path_line2);
			Point middle_point = Point::radius_point(arc.radius, arc.middle_angle()) + arc.center;
			flood(flood_points, vector<Point>{middle_point}, arc.color, arc.opacity);
		}
		return *this;
	}

	vector<Point> PngImage::path(const Bezier& bezier) const
	{
		return bezier.path();
	}
	PngImage& PngImage::operator<<(const Bezier& bezier)
	{
		draw_path(path(bezier), bezier.color,bezier.stroke_width, bezier.opacity);
		return *this;
	}
	PngImage& PngImage::operator<<(const Rectangle& rect)
	{
		Line right_vec(rect.left, rect.right, rect.color);
		Point diff = rect.right - rect.left;
		//由于我们当前的坐标系的设置，diff向量的up方向是(diff.y,diff.x) 而不是(diff.y,-diff.x)
		Point cur = rect.left;
		float length_scale = (rect.height * 1.0f / sqrtf(diff.x * diff.x + diff.y * diff.y));
		Point p_left_up = rect.left + Point(-diff.y, diff.x) * length_scale;
		Point p_right_up = rect.right + Point(-diff.y, diff.x) * length_scale;
		Line up_vec(p_left_up, rect.left, rect.color);
		Line up_right(p_right_up, p_left_up,rect.color);
		Line right_up(rect.right, p_right_up, rect.color);
		*this << right_vec << up_vec << up_right << right_up;
		if (rect.fill)
		{
			//这里我们为了避免浮点误差 采取洪范法
			auto path_right = path(right_vec);
			auto path_up = path(up_vec);
			auto path_up_right = path(up_right);
			auto path_right_up = path(right_up);
			vector<vector<Point>> path_points;
			path_points.push_back(std::move(path_right));
			path_points.push_back(std::move(path_right_up));
			path_points.push_back(std::move(path_up_right));
			path_points.push_back(std::move(path_up));
			Point center = (rect.left + rect.right)*0.5f + Point(diff.y * -0.5f, diff.x * 0.5f) * length_scale;
			flood(path_points, vector<Point>(1, center), rect.color, rect.opacity);
		}
		return *this;
	}

	vector<Point> PngImage::path(const Circle& circle) const
	{
		return circle.path();
	}
	PngImage& PngImage::operator<<(const Circle& circle)
	{
		//需要考虑是否填充
		//目前只考虑填充
		auto one_total = circle.path();
		for (const auto& i : one_total)
		{
			plot(i, circle.color, circle.opacity);
		}
		if (circle.filled)
		{
			flood(vector<vector<Point>>{one_total}, vector<Point>{circle.center}, circle.color, circle.opacity);
		}
		return *this;
	}
	PngImage& PngImage::operator<<(const Tile& tile)
	{
		vector<Point> path_points;
		Arc arc_1(tile.inner_radius, tile.begin_angle, tile.width, tile.center, false, tile.color);
		Arc arc_2(tile.outer_radius, tile.begin_angle, tile.width, tile.center, true, tile.color);
		Line line_1(Point::radius_point(tile.inner_radius, free_angle(tile.begin_angle) + tile.width, tile.center), Point::radius_point(tile.outer_radius, free_angle(tile.begin_angle) + tile.width, tile.center), tile.color);
		Line line_2(Point::radius_point(tile.outer_radius, tile.begin_angle, tile.center), Point::radius_point(tile.inner_radius, tile.begin_angle, tile.center), tile.color);
		auto arc_path1 = path(arc_1);
		auto arc_path2 = path(arc_2);
		auto line_path1 = path(line_1);
		auto line_path2 = path(line_2);
		draw_path(arc_path1, tile.color, tile.stroke, tile.opacity);
		draw_path(arc_path2, tile.color, tile.stroke, tile.opacity);
		draw_path(line_path1, tile.color, tile.stroke, tile.opacity);
		draw_path(line_path2, tile.color, tile.stroke, tile.opacity);

		if (tile.fill)
		{
			free_angle middle_angle = free_angle(tile.begin_angle) + free_angle(tile.width) / 2;
			
			auto middle_point = Point::radius_point((tile.inner_radius + tile.outer_radius) / 2, middle_angle, tile.center);
			vector<vector<Point>> flood_points;
			Line::connect_paths(flood_points, arc_path1);
			Line::connect_paths(flood_points, line_path1);
			Line::connect_paths(flood_points, arc_path2);
			Line::connect_paths(flood_points, line_path2);
			flood(flood_points, vector<Point>{middle_point}, tile.color, tile.opacity);
		}
		return *this;
	}
	PngImage& PngImage::operator<<(const Ribbon& ribbon)
	{
		auto arc_1 = path(ribbon.arc_1);
		auto arc_2 = path(ribbon.arc_2);
		auto bezier_1 = path(ribbon.bezier_1);
		auto bezier_2 = path(ribbon.bezier_2);
		draw_path(arc_1, ribbon.color, 1, ribbon.opacity);
		draw_path(arc_2, ribbon.color, 1, ribbon.opacity);
		draw_path(bezier_1, ribbon.color, 1, ribbon.opacity);
		draw_path(bezier_2, ribbon.color, 1, ribbon.opacity);
		
		if (ribbon.fill)
		{
			vector<vector<Point>> flood_points;
			Line::connect_paths(flood_points, arc_1);
			Line::connect_paths(flood_points, bezier_2);
			Line::connect_paths(flood_points, arc_2);
			Line::connect_paths(flood_points, bezier_2);
			//下面的这两个点一定在ribbon内部
			Point middle_1 = (ribbon.arc_1.from_point() + ribbon.arc_1.to_point())*0.5;
			Point middle_2 = (ribbon.arc_2.from_point() + ribbon.arc_2.to_point())*0.5;
			flood(flood_points, vector<Point>{middle_1, middle_2}, ribbon.color, ribbon.opacity);
		}
		return *this;
	}
	PngImage& PngImage::operator<<(const Annulus& annulus)
	{
		if (annulus.opacity < 0.01)
		{
			return *this;
		}
		Arc arc_1(annulus.inner_radius, free_angle::from_angle(0), free_angle::from_angle(360), annulus.center, false, annulus.color);
		Arc arc_2(annulus.outer_radius, free_angle::from_angle(0), free_angle::from_angle(360), annulus.center, false, annulus.color);
		auto arc_path1 = path(arc_1);
		auto arc_path2 = path(arc_2);
		draw_path(arc_path1, annulus.color, 1, annulus.opacity);
		draw_path(arc_path2, annulus.color, 1, annulus.opacity);
		if(annulus.filled)
		{
			Point middle_point = Point::radius_point((annulus.inner_radius + annulus.outer_radius) / 2, free_angle::from_angle(180), annulus.center);
			vector<vector<Point>> flood_points;
			flood_points.emplace_back(move(arc_path1));
			flood_points.emplace_back(move(arc_path2));
			flood(flood_points, vector<Point>{middle_point}, annulus.color, annulus.opacity);
		}
		return *this;
	}
	PngImage& PngImage::operator<<(const Region& region)
	{
		vector<vector<Point>> flood_points;
		for(const auto& one_boundary: region.boundaries)
		{
			std::visit([&flood_points](auto&& arg){
				flood_points.push_back(arg.path());
			}, one_boundary);
		}
		for (const auto& one_path : flood_points)
		{
			draw_path(one_path, region.color, 1, region.opacity);
		}
		flood(flood_points, vector<Point>{region.inner_point}, region.color, region.opacity);
		return *this;
	}
}