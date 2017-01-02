#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <png.h>
#include <zlib.h>
#include <stack>
//#include <ft2build.h>

#include "../shapes/line.h"
#include "../shapes/circle.h"
#include "../shapes/arc.h"
#include "../shapes/bezier.h"
#include "../shapes/ring.h"
#include "../shapes/track.h"
#include "../shapes/rectangle.h"

using namespace std;
#define PI 3.1415926
#define EPS 0.0001
namespace circos
{
	//这里png的坐标系同svg的坐标系 左上方为0，0 向右是x正向，向下时y正向
	class PngImage
	{
	public:
		int height;
		int width;
		const int radius;
		Color backgroud_color;
		int _compression_level;
		vector<Color> _buffer;//这个是真正的存储区域 存储了所有的color信息
		vector<Color*> _image;//这个是png的行指针，只能存行，因为libpng规定了这个

		vector<char> flood_buffer;
		vector<char*> flood_map;//这个按照png里的惯例 ，也弄成行指针吧

		string file_name;
		int bit_depth = 8;
		const string author = "Author:spiritsaway";
		const string description = "Mail:spiritsaway@outlook.com";
		const string software = "circos implemented in c++";
		const string title = "circos.png";
		unordered_map<int, vector<Point>> circle_cache;
		PngImage(string in_file_name, int in_radius, Color back_color, int compress=8)
		: file_name(in_file_name)
		, radius(in_radius)
		, width(2*in_radius)
		, height(2*in_radius)
		, backgroud_color(back_color)
		, _compression_level(compress)
		, _image(2*in_radius, nullptr)
		{
			flood_buffer = vector<char>(width*height, 0);
			flood_map = vector<char*>(height, nullptr);
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
				_buffer[i]=backgroud_color;
			}
		}
		void plot(ColorPoint input, float blend = 1.0)
		{
			_image[input.pos.y][input.pos.x].blend(input.color, blend);
		}
		void plot(Point pos, Color color,float blend = 1.0)
		{
			_image[pos.y][pos.x].blend(color,blend);
		}
		void plot(int x, int y, Color color, float blend=1.0)
		{
			_image[y][x].blend(color,blend);
		}
		Color read(Point pos)
		{
			return _image[pos.y][pos.x];
		}
		void draw_path(const vector<Point>& path, Color color, int stroke = 1, float opacity = 1.0)
		{
			int offset_left = (stroke / 2) * -1;
			int offset_right = (stroke + 1) / 2;
			for (const auto& i : path)
			{
				for (int j = offset_left;j < offset_right;j++)
				{
					plot(i.x + j, i.y, color, opacity);
				}
			}
		}
		~PngImage()
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
		bool can_flood(Point current)
			//判断一个点是否可以将他的相邻点加入洪范列表
			//这里为了保险起见才加了这个判断函数
			//只要有一个处于边界就停止，避免可能的边界没有完全闭合的情况
		{
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
		void flood(const vector<Point> boundary, vector<Point> interiors, Color fill_color, double opacity =1.0)
			//要求interior一定在boundary里面
		{
			vector<Point> fill_points;
			for (const auto& i : boundary)
			{
				flood_map[i.y][i.x] = 2;//2代表边界，1代表已经着色，0 代表还未访问
				fill_points.push_back(i);
			}
			//下面的这两个点一定在track内部
			stack<Point> all_points;
			for (const auto& i : interiors)
			{
				all_points.push(i);
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
				flood_map[current.y][current.x] = 1;
				
				if (can_flood(current))
				{
					Point up(current.x, current.y + 1);
					Point down(current.x, current.y - 1);
					Point left(current.x - 1, current.y);
					Point right(current.x + 1, current.y);
					all_points.push(up);
					all_points.push(down);
					all_points.push(left);
					all_points.push(right);
				}
				
			}
			for (const auto& i : fill_points)
			{
				flood_map[i.y][i.x] = 0;
			}
		}
		vector<Point> path(const Line& line)
		{
			vector<Point> result;
			int xfrom = line.from.x;
			int yfrom = line.from.y;
			int xto = line.to.x;
			int yto = line.to.y;
			//  Bresenham Algorithm.
			//
			int dy = yto - yfrom;
			int dx = xto - xfrom;
			int stepx, stepy;

			if (dy < 0)
			{
				dy = -dy;
				stepy = -1;
			}
			else
			{
				stepy = 1;
			}
			if (dx < 0)
			{
				dx = -dx;
				stepx = -1;
			}
			else
			{
				stepx = 1;
			}
			dy <<= 1;     // dy is now 2*dy
			dx <<= 1;     // dx is now 2*dx
			
			result.emplace_back(xfrom, yfrom);
			if (dx > dy)
			{
				int fraction = dy - (dx >> 1);
				while (xfrom != xto)
				{
					if (fraction >= 0)
					{
						yfrom += stepy;
						fraction -= dx;
					}
					xfrom += stepx;
					fraction += dy;
					result.emplace_back(xfrom , yfrom);
				}
			}
			else
			{
				int fraction = dx - (dy >> 1);
				while (yfrom != yto)
				{
					if (fraction >= 0)
					{
						xfrom += stepx;
						fraction -= dy;
					}
					yfrom += stepy;
					fraction += dx;
					result.emplace_back(xfrom, yfrom);
				}
			}
			return result;
		}
		PngImage& operator<<( const Line& line)
		{
			draw_path(path(line), line.color, line.width, line.opacity);
			return *this;
			
		}
		const vector<Point>& get_circle(int radius)
		{
			if (circle_cache.find(radius) != circle_cache.end())
			{
				return circle_cache[radius];
			}
			vector<Point> path_points;
			int x =0;
			int y = radius;
			int p =(5-radius*4)/4;
			while(x<y)
			{
				x++;
				if(p<0)
				{
					p+=2*x+1;
				}
				else
				{
					y--;
					p+=2*(x-y)+1;
				}
				path_points.emplace_back(x,y);
			}
			circle_cache[radius] = std::move(path_points);
			return circle_cache[radius];
		}

		vector<Point> arc_path(double angle_begin,double angle_end, int radius)
		{
			//这个函数只负责画不大于PI/4区域的路径
			if (abs(angle_end - angle_begin) < EPS)
			{
				return vector<Point>();
			}
			int idx = angle_begin * 4 / PI;
			if (idx >= 4)
			{
				//大于半圆的话 直接反转
				angle_begin = 2 * PI - angle_begin;
				angle_end = 2 * PI - angle_end;
				auto result = arc_path(angle_end,angle_begin,  radius);
				//将y轴坐标反转
				for (auto & i : result)
				{
					i.y = -i.y;
				}
				return result;
			}
			if (idx >= 2)
			{
				//在左半球的话，直接向右折叠
				angle_begin = PI - angle_begin;
				angle_end = PI - angle_end;
				auto result = arc_path(angle_end,angle_begin,  radius);
				//将x坐标反转
				for (auto& i : result)
				{
					i.x = -i.x;
				}
				return result;
			}
			if (idx == 0)
			{
				//如果在第一象限右半区的话 对折到第一象限的上半区
				angle_begin = PI / 2 - angle_begin;
				angle_end = PI / 2 - angle_end;
				auto result = arc_path(angle_end,angle_begin, radius);
				for (auto& i : result)
				{
					auto temp = i.x;
					i.x = i.y;
					i.y = temp;
				}
				return result;
			}
			//现在确信是在第一象限上半区

			const auto& cache = circle_cache[radius];
			double begin_x = radius*cos(angle_end);
			double end_x = radius*cos(angle_begin);
			auto begin_iter = lower_bound(cache.begin(), cache.end(), begin_x, [](const Point& a, double x)
			{
				return a.x < x;
			});
			auto end_iter = lower_bound(cache.begin(), cache.end(),end_x, [](const Point& a, double x)
			{
				return a.x < x;
			});
			vector<Point> result;
			copy(begin_iter, end_iter, back_inserter(result));
			return result;

		}
		vector<Point> path(const Arc& arc)
		{
			auto angle_begin = arc.begin_angle;
			auto angle_end = arc.end_angle;
			auto radius = arc.radius;
			vector<Point> result;
			if(abs(angle_begin-angle_end) < EPS)
			{
				return result;
			} 
			const auto& points = get_circle(radius);;
			if(angle_begin >angle_end)
			{
				auto result_1 = path(Arc(arc.radius,0,angle_begin,arc.center,arc.color));
				auto result_2 = path(Arc(arc.radius,angle_end,2*PI,arc.center,arc.color));
				copy(result_2.begin(),result_2.end(), back_inserter(result_1));
				return result_1;
			}
			int begin_idx = angle_begin*4/PI;
			int end_idx = angle_end*4/PI;
			if(begin_idx == end_idx)
			{
				result = arc_path(angle_begin,angle_end, radius);
				for (auto& i : result)
				{
					i += arc.center;
				}
				return result;
			}
			else
			{
				result = arc_path(angle_begin+EPS, (begin_idx+1)*PI/4-EPS, radius);
				begin_idx++;
				while(begin_idx < end_idx)
				{
					auto temp = arc_path(begin_idx*PI/4+EPS, (begin_idx+1)*PI/4-EPS, radius);
					copy(temp.begin(),temp.end(), back_inserter(result));
					begin_idx++;
				}
				auto temp = arc_path(begin_idx*PI/4, angle_end, radius);
				copy(temp.begin(),temp.end(), back_inserter(result));
				for (auto& i : result)
				{
					i += arc.center;
				}
				return result;
			}
		}
		PngImage& operator<<(const Arc& arc)
		{
			draw_path(path(arc), arc.color, arc.stroke, arc.opacity);
			return *this;
		}
		vector<Point> path(const Bezier& bezier)
		{
			vector<Point> result;
			const auto&  p1 = bezier.begin_point;
			const auto& p2 = bezier.end_point;
			const auto& cp = bezier.control_point;
			Point c1;
			Point c2;
			Point px;
			int total_len = Line(bezier.begin_point, bezier.control_point).len() + Line(bezier.control_point, bezier.end_point).len();
			if (total_len == 0)
			{
				return result;
			}
			float step = 1 / total_len;
			float inc = 0;
			c1 = p1 + (cp - p1)*inc;
			c2 = cp + (p2 - cp)*inc;
			px = c1 + (c2 - c1)*inc;
			result.push_back(px);
			inc += step;
			while (inc <= 1)
			{
				c1 = p1 + (cp - p1)*inc;
				c2 = cp + (p2 - cp)*inc;
				px = c1 + (c2 - c1)*inc;
				if (!(px==result.back()))
				{
					result.push_back(px);
				}
				
			}
			return result;
		}
		PngImage& operator<<(const Bezier& bezier)
		{
			draw_path(path(bezier), bezier.color,bezier.stroke_width, bezier.opacity);
			return *this;
		}
		PngImage& operator<<(const Rectangle& rect)
		{
			Line right_vec(rect.left, rect.right, rect.color);
			Point diff = rect.right - rect.left;
			//由于我们当前的坐标系的设置，diff向量的up方向是(diff.y,diff.x) 而不是(diff.y,-diff.x)
			Point cur = rect.left;
			Line up_vec(rect.left, rect.left + Point(-diff.y, diff.x), rect.color);
			Line up_right(rect.left + Point(-diff.y, diff.x), rect.right + Point(-diff.y, diff.x), rect.color);
			Line right_up(rect.right, rect.right + Point(-diff.y, diff.x), rect.color);
			*this << right_vec << up_vec << up_right << right_up;
			if (rect.fill)
			{
				//这里我们为了避免浮点误差 采取洪范法
				auto path_right = path(right_vec);
				auto path_up = path(up_vec);
				auto path_up_right = path(up_right);
				auto path_right_up = path(right_up);
				vector<Point> path_points;
				std::copy(path_right.begin(), path_right.end(), std::back_inserter(path_points));
				std::copy(path_up.begin(), path_up.end(), std::back_inserter(path_points));
				std::copy(path_up_right.begin(), path_up_right.end(), std::back_inserter(path_points));
				std::copy(path_right_up.begin(), path_right_up.end(), std::back_inserter(path_points));
				Point center = (right_vec.from + up_right.to)*0.5;
				flood(path_points, vector<Point>(1, center), rect.color, rect.opacity);
			}
			return *this;
		}
		vector<Point> path(const Circle& circle)
		{
			const auto& one_eight = get_circle(circle.radius);
			vector<Point> one_four;
			copy(one_eight.begin(), one_eight.end(), back_inserter(one_four));
			for (const auto& i : one_eight)
			{
				one_four.emplace_back(i.y, i.x);
			}
			vector<Point> one_half;
			copy(one_four.begin(), one_four.end(), back_inserter(one_half));
			for (const auto& i : one_four)
			{
				one_half.emplace_back(-i.x, i.y);
			}
			vector<Point> one_total;
			copy(one_half.begin(), one_half.end(), back_inserter(one_total));
			for (const auto& i : one_half)
			{
				one_total.push_back(Point(i.x, -i.y));
			}
			return one_total;
		}
		PngImage& operator<<(const Circle& circle)
		{
			//需要考虑是否填充
			//目前只考虑填充
			const auto& one_eight = get_circle(circle.radius);
			vector<Point> one_four;
			copy(one_eight.begin(), one_eight.end(), back_inserter(one_four));
			for (const auto& i : one_eight)
			{
				one_four.emplace_back(i.y, i.x);
			}
			vector<Point> one_half;
			copy(one_four.begin(), one_four.end(), back_inserter(one_half));
			for (const auto& i : one_four)
			{
				one_half.emplace_back(-i.x, i.y);
			}
			vector<Point> one_total;
			copy(one_half.begin(), one_half.end(), back_inserter(one_total));
			for (const auto& i : one_half)
			{
				one_total.push_back(Point(i.x, -i.y));
			}
			for (auto& i : one_total)
			{
				i += circle.center;
			}
			for (const auto& i : one_total)
			{
				plot(i, circle.color, circle.opacity);
			}
			if (circle.filled)
			{
				flood(one_total, vector<Point>{circle.center}, circle.color, circle.opacity);
			}
			return *this;
		}
		
		PngImage& operator<<(const Track& track)
		{
			const auto& arc_1 = path(track.arc_1);
			const auto& arc_2 = path(track.arc_2);
			const auto& bezier_1 = path(track.bezier_1);
			const auto& bezier_2 = path(track.bezier_2);
			vector<Point> path_points;
			path_points.reserve(arc_1.size() + arc_2.size() + bezier_1.size()+bezier_2.size());
			copy(arc_1.begin(), arc_1.end(), back_inserter(path_points));
			copy(arc_2.begin(), arc_2.end(), back_inserter(path_points));
			copy(bezier_1.begin(), bezier_1.end(), back_inserter(path_points));
			copy(bezier_2.begin(), bezier_2.end(), back_inserter(path_points));
			for (const auto& i : path_points)
			{
				plot(i, track.color, track.opacity);
			}
			if (track.fill)
			{
				vector<Point> fill_points;

				//下面的这两个点一定在track内部
				Point middle_1 = (track.arc_1.from_point + track.arc_1.to_point)*0.5;
				Point middle_2 = (track.arc_2.from_point + track.arc_2.to_point)*0.5;
				flood(path_points, vector<Point>{middle_1, middle_2}, track.color, track.opacity);
			}
		}
	};
}