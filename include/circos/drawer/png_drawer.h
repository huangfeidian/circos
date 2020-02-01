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



#include "../shapes/line.h"
#include "../shapes/circle.h"
#include "../shapes/arc.h"
#include "../shapes/bezier.h"
#include "../shapes/tile.h"
#include "../shapes/ribbon.h"
#include "../shapes/rectangle.h"
#include "../shapes/line_text.h"
#include "../shapes/annulus.h"
#include "../shapes/region.h"

#ifdef USE_TEXT
#include <ft2build.h>
#include FT_FREETYPE_H

#endif


namespace spiritsaway::circos
{
	using std::vector;
	using std::string;
	//这里png的坐标系同svg的坐标系 左上方为0，0 向右是x正向，向下时y正向
	class PngImage
	{
	public:
		int height;
		int width;
		const int radius;
		Color background_color;
		int _compression_level;
		vector<Color> _buffer;//这个是真正的存储区域 存储了所有的color信息
		vector<Color*> _image;//这个是png的行指针，只能存行，因为libpng规定了这个

		vector<uint8_t> flood_buffer;
		vector<uint8_t*> flood_map;//这个按照png里的惯例 ，也弄成行指针吧

		string file_name;
		int bit_depth = 8;
		const string author = "Author:spiritsaway";
		const string description = "Mail:spiritsaway@outlook.com";
		const string software = "circos implemented in c++";
		const string title = "circos.png";
		std::unordered_map<int, vector<Point>> circle_cache;
		const std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>& font_info;//所有字体相关文件的存储路径映射
		//下面是跟freetype相关的成员
#ifdef USE_TEXT
		std::unordered_map<std::string_view, vector<unsigned char>> font_cache;//字体文件读入内存
		FT_Library ft_library;
#endif

		PngImage(const std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>& in_font_info,string in_file_name, int in_radius, Color back_color, int compress=8);
#ifdef USE_TEXT
		const vector<unsigned char>& get_font_mem(std::string_view font_name);
#endif
		void plot(Colorbasic_point input, float opacity = 1.0);
		void plot(Point pos, Color color,float opacity = 1.0);
		void plot(int x, int y, Color color, float opacity=1.0);
		Color read(Point pos);
		void draw_path(const vector<Point>& path, Color color, int stroke = 1, float opacity = 1.0);
		~PngImage();

		bool on_boundary(Point current) const; // 判断一个点是否在边界上
		//判断一个点是否可以将他的相邻点加入洪范列表
		//这里为了保险起见才加了这个判断函数
		//只要有一个处于边界就停止，避免可能的边界没有完全闭合的情况
		bool can_flood(Point current);
		//要求interior一定在boundary里面
		void flood(const vector<vector<Point>>& boundary, vector<Point> interiors, Color fill_color, float opacity =1.0);
			
#ifdef USE_TEXT
		vector<uint32_t> utf8_to_uint(std::string_view text) const;
		//这里要处理一下utf8
		void draw_text(const Line& base_line, std::string_view text, std::string_view , int font_size, Color color, float alpha);
			
		void draw_bitmap(const FT_GlyphSlot& slot,Line on_line, Color color, float alpha);
		// 获得某一点相对于某条线的对称点
#endif
		vector<Point> path(const Line& line) const;
		PngImage& operator<<( const Line& line);

		PngImage& operator<<(const LineText& line_text);
		vector<Point> path(const Arc& arc) const;
		PngImage& operator<<(const Arc& arc);

		vector<Point> path(const Bezier& bezier) const;
		PngImage& operator<<(const Bezier& bezier);
		PngImage& operator<<(const Rectangle& rect);
		vector<Point> path(const Circle& circle) const;
		PngImage& operator<<(const Circle& circle);
		PngImage& operator<<(const Tile& tile);
		PngImage& operator<<(const Ribbon& ribbon);
		PngImage& operator<<(const Annulus& annulus);
		PngImage& operator<<(const Region& region);
	};
}