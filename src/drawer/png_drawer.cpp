#include <unordered_map>
#include <string>
#include <sstream>
#include <circos/drawer/png_drawer.h>
#include <circos/shape_collection.h>
#include <iostream>
#include <fstream>

namespace spiritsaway::circos
{
	using namespace std;
	PngImage::PngImage(const unordered_map<string_view, pair<string_view, string_view>>& in_font_info, string in_file_name, int in_radius, Color back_color, int compress)
	: font_info(in_font_info)
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
#ifdef USE_TEXT

		auto error = FT_Init_FreeType(&ft_library);
		if (error)
		{
			cerr << "cant init ft_library" << endl;
			exit(1);
		}
#endif
	}

#ifdef USE_TEXT
	const vector<unsigned char>& PngImage::get_font_mem(std::string_view font_name)
	{
		if (font_info.find(font_name) == font_info.end())
		{
			cerr << "unknown font name " << font_name << endl;
			cerr << "all avail fonts is " << endl;
			for (const auto& i : font_info)
			{
				cerr << i.first << endl;
			}
			exit(1);
		}
		auto iter = font_cache.find(font_name);
		if (iter != font_cache.end())
		{
			return iter->second;
		}
		font_cache[font_name] = vector<unsigned char>();

		auto& result = font_cache[font_name];
		auto font_iter = font_info.find(font_name);
		if (font_iter == font_info.end())
		{
			std::cerr << "unknown font " << font_name << std::endl;
			exit(1);
		}
		ifstream font_file(string(font_iter->second.first), ios::binary);
		std::copy(std::istreambuf_iterator<char>(font_file), std::istreambuf_iterator<char>(), std::back_inserter(result));
		return result;

	}
#endif
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
					if (flood_map[i.y + k][i.x + j] != 1)
					{
						flood_map[i.y + k][i.x + j] = 1;
						final_path.push_back(Point(i.x + j, i.y + k));
					}
				}
			}
		}
		for (const auto& i : final_path)
		{
			plot(i, color, opacity);
			flood_map[i.y][i.x] = 0;
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
	bool PngImage::can_flood(Point current)
		//判断一个点是否可以将他的相邻点加入洪范列表
		//这里为了保险起见才加了这个判断函数
		//只要有一个处于边界就停止，避免可能的边界没有完全闭合的情况
	{
		if (current.x == 0 || current.x == width - 1 || current.y == 0 || current.y == height - 1)
		{
			return false;
		}
		Point up(current.x, current.y + 1);
		Point down(current.x, current.y - 1);
		Point left(current.x - 1, current.y);
		Point right(current.x + 1, current.y);
		if (flood_map[up.y][up.x] == 2)
		{
			return false;
		}
		if (flood_map[down.y][down.x] == 2)
		{
			return false;

		}
		if (flood_map[left.y][left.x] == 2)
		{
			return false;
		}
		if (flood_map[right.y][right.x] == 2)
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
				flood_map[one_gap_point.y][one_gap_point.x] = 2;//2代表边界，1代表已经着色，0 代表还未访问
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
				flood_map[i.y][i.x] = 2;//2代表边界，1代表已经着色，0 代表还未访问
				fill_points.push_back(i);
			}
		}
		//下面的这两个点一定在ribbon内部
		stack<Point> all_points;
		for (auto one_point : interiors)
		{
			all_points.push(one_point);
		}
		while (!all_points.empty())
		{
			auto current = all_points.top();
			all_points.pop();
			if (current.y <= 0 || current.y >= height-1 || current.x<=0 || current.x>=width-1 || flood_map[current.y][current.x] != 0)
			{
				continue;
			}
			fill_points.push_back(current);
			plot(current, fill_color, opacity);
			count += 1;
			flood_map[current.y][current.x] = 1;
			auto flood_flag = can_flood(current);
			if (flood_flag)
			{
				all_points.push(Point(current.x, current.y + 1));
				all_points.push(Point(current.x, current.y - 1));
				all_points.push(Point(current.x + 1, current.y + 1));
				all_points.push(Point(current.x + 1, current.y - 1));
				all_points.push(Point(current.x - 1, current.y - 1));
				all_points.push(Point(current.x - 1, current.y + 1));
				all_points.push(Point(current.x + 1, current.y));
				all_points.push(Point(current.x - 1, current.y));
			}
			
		}
		for (const auto& i : fill_points)
		{
			flood_map[i.y][i.x] = 0;
		}
		// cout << "flood count " << count << endl;
	}
#ifdef USE_TEXT
	vector<uint32_t> PngImage::utf8_to_uint(std::string_view text) const
	{
		unsigned char u, v, w, x, y, z;
		vector<uint32_t> utf8_result;
		int num_chars = 0;
		uint32_t num_bytes = text.length();
		std::uint32_t iii = 0;
		while (iii < num_bytes)
		{
			uint32_t cur_utf8_char = 0;
			z = text[iii];
			if (z <= 127)
			{
				cur_utf8_char = z;
			}
			if (z >= 192 && z <= 223)
			{
				iii++;
				y = text[iii];
				cur_utf8_char = (z - 192) * 64 + (y - 128);
			}
			if (z >= 224 && z <= 239)
			{
				iii++; y = text[iii];
				iii++; x = text[iii];
				cur_utf8_char = (z - 224) * 4096 + (y - 128) * 64 + (x - 128);
			}
			if ((240 <= z) && (z <= 247))
			{
				iii++; y = text[iii];
				iii++; x = text[iii];
				iii++; w = text[iii];
				cur_utf8_char = (z - 240) * 262144 + (y - 128) * 4096 + (x - 128) * 64 + (w - 128);
			}
			if ((248 <= z) && (z <= 251))
			{
				iii++; y = text[iii];
				iii++; x = text[iii];
				iii++; w = text[iii];
				iii++; v = text[iii];
				cur_utf8_char = (z - 248) * 16777216 + (y - 128) * 262144 + (x - 128) * 4096 + (w - 128) * 64 + (v - 128);
			}

			if ((252 == z) || (z == 253))
			{
				iii++; y = text[iii];
				iii++; x = text[iii];
				iii++; w = text[iii];
				iii++; v = text[iii];
				u = text[iii];
				cur_utf8_char = (z - 252) * 1073741824 + (y - 128) * 16777216 + (x - 128) * 262144 + (w - 128) * 4096 + (v - 128) * 64 + (u - 128);
			}
			if (z >= 254)
			{
				std::cerr << "convert string to utf8 char fail with first byte larger than 234" << std::endl;
				exit(1);
			}
			utf8_result.push_back(cur_utf8_char);
			iii++;
		}
		return utf8_result;
	}
	void PngImage::draw_text(const Line& base_line, std::string_view text, std::string_view font_name, int font_size, Color color, float alpha)
		//这里要处理一下utf8
	{
		FT_Face face;
		auto font_mem = get_font_mem(font_name);
		FT_UInt glyph_index;
		FT_Error error;
		FT_Matrix matrix;
		FT_Vector pen;

		FT_Bool use_kerning;
		FT_UInt previous = 0;
		double angle = atan2(base_line.to.y - base_line.from.y,base_line.to.x-base_line.from.x) ;
		double cos_angle = cos(angle);
		double sin_angle = sin(angle);
		matrix.xx = static_cast<FT_Fixed>(cos_angle * 65536);
		matrix.xy = static_cast<FT_Fixed>(-sin_angle * 65536);
		matrix.yx = static_cast<FT_Fixed>(sin_angle * 65536);
		matrix.yy = static_cast<FT_Fixed>(cos_angle * 65536);
		pen.x = base_line.from.x * 64;
		pen.y = base_line.from.y * 64;
		error = FT_New_Memory_Face(ft_library, &font_mem[0], font_mem.size(), 0, &face);
		if (error)
		{
			cerr << "error loading font:" << font_name << " from memory" << endl;
			return;
		}
		error = FT_Set_Pixel_Sizes(face, font_size, 0);
		if (error)
		{
			cerr << "Freetype set char size error, font:" << font_name << " size:" << font_size << endl;
			return ;
		}
		FT_GlyphSlot slot = face->glyph;
		use_kerning = FT_HAS_KERNING(face);
		const auto& u8_text = utf8_to_uint(text);
		for (uint32_t i : u8_text)
		{
			glyph_index = FT_Get_Char_Index(face, i);
			if (use_kerning&& previous&&glyph_index)
			{
				FT_Vector delta;
				FT_Get_Kerning(face, previous, glyph_index, ft_kerning_default, &delta);
				pen.x += static_cast<int>(delta.x*cos_angle);
				pen.y += static_cast<int>(delta.y*sin_angle);
			}
			FT_Set_Transform(face, &matrix, &pen);
			error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
			if (error)
			{
				cerr << "freetype cant load glyph, font:" << font_name << " index:" << glyph_index << endl;
				return;
			}
			error = FT_Render_Glyph(face->glyph, ft_render_mode_normal);
			if (error)
			{
				cerr << "freetype cant load glyph, font:" << font_name << " index:" << glyph_index << endl;
				return;
			}
			//这里应该开始画bitmap了
			draw_bitmap(slot, base_line, color, alpha);
			pen.x += slot->advance.x;
			pen.y += slot->advance.y;
			previous = glyph_index;
		}
		FT_Done_Face(face);
	}
	void PngImage::draw_bitmap(const FT_GlyphSlot& slot,Line on_line, Color color, float alpha)
	{
		const FT_Bitmap* bitmap = &slot->bitmap;
		int left = slot->bitmap_left;
		int top = slot->bitmap_top;
		float temp;
		for (uint32_t i = 1;i < bitmap->rows;i++)
		{
			for (uint32_t j = 1; j < bitmap->width;j++)
			{
				temp = static_cast<uint8_t>(bitmap->buffer[(i - 1)*bitmap->width + (j - 1)]);
				if (temp)
				{
					auto sym_point = on_line.symmetric_point(Point(left + j, top - i));
					plot(sym_point.x, sym_point.y, color, alpha*temp/255.0f);
					//plot(left + j, top - i, color, alpha*temp / 255.0);
				}
			}
		}
	}
#endif
	PngImage& PngImage::operator<<(const LineText& line_text)
	{
#ifdef USE_TEXT
		draw_text(line_text.on_line, line_text.utf8_text, line_text.font_name, line_text.font_size, line_text.color, line_text.opacity);
#endif // USE_TEXT

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