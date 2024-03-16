#include <circos/drawer/png_drawer.h>
#include <circos/drawer/svg_drawer.h>
#include <circos/shape_collection.h>
#include <circos/basics/constants.h>
#include <map>
#include <iostream>
#include <string>
using namespace spiritsaway::circos;
using namespace std;

void flood_test()
{
	int radius = 1500;
	Point center(radius, radius);
	Color background_color = Color(255, 255, 255);
	Tile cur_tile(center, 600, 700, free_angle::from_angle(240), free_angle::from_angle(30), Color(0, 0, 0), 1, true, 1);
	Point line_begin = Point(radius, radius);
	Point line_end = Point(radius + 6, radius);
	Line cur_line(line_begin, line_end, Color(0, 0, 0), 5);

	int cur_circle_radius = 1022;
	int bottom_radius = 970;
	float angle_begin = 224.063;
	float angle_end = 225.864;
	auto temp_region = Region::make_fan(bottom_radius, cur_circle_radius, free_angle::from_angle(angle_begin), free_angle::from_angle(angle_end), center, Color(128, 128, 128));
	string png_filename = "circos_flood_test_1.png";
	string svg_filename = "circos_flood_test_1.svg";
	std::unordered_map<string, pair<string, string>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	PngImage png_image(font_info, png_filename, radius, background_color);
	SvgGraph svg_graph(font_info, svg_filename, radius, background_color);
	shape_collection shapes;
	//shapes.tiles.push_back(cur_tile);
	shapes.lines.push_back(cur_line);
	//shapes.regions.push_back(temp_region);
	draw_collections(png_image, shapes);
	draw_collections(svg_graph, shapes);
}
int main()
{
    flood_test();
    return 0;
}