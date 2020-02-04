#include <circos/drawer/png_drawer.h>

#include <circos/shape_collection.h>
#include <fstream>
#include <circos/basics/utf8_util.h>
#include <ctime>

using namespace std;
using namespace spiritsaway::circos;
using namespace spiritsaway::string_util;

int main()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y-%H-%M-%S", timeinfo);

	auto text = utf8_util::utf8_to_uint("0123456789"sv);
	std::unordered_map<string_view, pair<string_view, string_view>> font_info{ { "yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei") } };
	Color font_color(24, 107, 178);
	std::string png_file_name = "freetype_test_" + std::string(buffer) + ".png";
	std::string font_name = "yahei";
	std::uint8_t font_size = 64;
	auto cur_png = PngImage(font_info, png_file_name, 1000, Color());
	vector<Line> lines;
	lines.push_back(Line(Point(1000, 1000), Point(1000, 1100)));
	lines.push_back(Line(Point(1100, 1000), Point(1200, 1200)));
	lines.push_back(Line(Point(1200, 1000), Point(1300, 1300)));
	lines.push_back(Line(Point(1300, 1000), Point(1400, 1400)));
	for (auto one_line : lines)
	{
		cur_png.draw_text(one_line, text, font_name, font_size, font_color, 1.0);
	}

}