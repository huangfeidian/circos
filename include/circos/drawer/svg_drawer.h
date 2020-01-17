#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <string_view>

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

using std::vector;
using std::endl;
namespace spiritsaway::circos
{

	class SvgGraph
	{
		std::ostringstream output;
		const std::string file_name;
		int path_index = 0;
		const int background_radius;
		const Color background_color;
		const std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>& font_info;//first is png font filepath second is svn font name
	public:
		SvgGraph(const std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>& in_font_info,
			string in_file_name, int in_background_radius, Color in_background_color);
			
		SvgGraph& operator<<(std::string_view input_str);
		SvgGraph& operator<<(int value);
		SvgGraph& operator<<(double value);
		SvgGraph& operator<<(Color color);
		SvgGraph& operator<<(const Point& point);
		void add_to_path(const Line& line);
		SvgGraph& operator<<(const Line& line);
		std::string_view get_font_name(std::string_view input_name)const;

		void add_to_path(const Arc& arc);
		SvgGraph& operator<<( const Arc& arc);

		SvgGraph& operator<<(const Bezier& bezier);
		void add_to_path(const Bezier& bezier);
		SvgGraph& operator<<(const Circle& circle);
		SvgGraph& operator<<(const Rectangle& rect);
		SvgGraph& operator<<(const Tile& ring);
		SvgGraph& operator<<(const Ribbon& ribbon);
		SvgGraph& operator<<(const LineText& line_text);
		SvgGraph& operator<<(const Annulus& annulus);
		SvgGraph& operator<<(const Region& region);
		~SvgGraph();
	};
}