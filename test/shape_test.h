#include <circos/drawer/png_drawer.h>
#include <circos/drawer/svg_drawer.h>
#include <circos/shape_collection.h>
#include <circos/basics/constants.h>
#include <map>
#include <iostream>
#include <string_view>
using namespace spiritsaway::circos;
using namespace std;
void shape_test_1()
{
	int radius = 400;
	Color background_color = Color(255, 105, 180);
	string svg_filename = "circos_shape_test_1.svg";
	string png_filename = "circos_shape_test_1.png";
	Line line(Point(400, 400), Point(500, 400), Color(0, 255, 180), 4);
	Rectangle rect_test_1(Point(100, 100), Point(200, 100), Color(128, 105, 180), 100, true);
	Rectangle rect_test_2(Point(100, 100), Point(200, 200), Color(0, 105, 180), 141, true,0.5);
	Point center(400, 400);
	auto c_pi = free_angle::from_angle(180);
	Circle circle_test_1(360, Point(400, 400), Color(0, 255, 180), 0.5, false);
	Tile tile(center, 300, 350, c_pi * 11 / 6, c_pi / 3, Color(200, 230, 108));
	Arc arc(300, c_pi * 11 / 6, c_pi / 3, Point(400, 400),false,  Color(0, 255, 100), 0, 1.0, 4);
	Arc arc_1(360, c_pi / 6, c_pi / 6, center, false,  Color(128, 128, 128), 0, 1.0, 2);
	Arc arc_2(360, c_pi * 2 / 3, c_pi * 1 / 6, center, false, Color(128, 128, 128), 0, 1.0, 8);
	Arc arc_3(360, c_pi * 5 / 6 + c_pi, c_pi, center, false, Color(128, 128, 128));
	std::unordered_map<string_view, pair<string_view, string_view>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	auto sme = font_info.lower_bound("nima");
	Bezier t_bezier_1(arc_1.to_point(), arc_3.from_point(), center, arc_1.color);
	Bezier t_bezier_2(arc_3.to_point(), arc_1.from_point(), center, arc_1.color);

	Ribbon ribbon_2(center, 360, c_pi / 6 + c_pi, c_pi / 6, c_pi * 2 / 3 + c_pi, c_pi * 1 / 6, arc_1.color, false, 0,false,false
	);
	Ribbon ribbon_3(center, 360, c_pi / 6 + c_pi, c_pi / 6, c_pi * 2 / 3 + c_pi, c_pi * 1 / 6, arc_1.color,false,  50, false
	);
	Bezier bezier_1(Point(500, 500), Point(200, 700), Point(400, 600), Color(200, 230, 108),2);
	Bezier bezier_2(Point(500, 500), Point(200, 700), Point(300, 600), Color(200, 230, 108), 2);
	Bezier bezier_3(Point(500, 500), Point(200, 700), Point(200, 600), Color(200, 230, 108), 2);
	SvgGraph svg_graph(font_info, svg_filename, radius, background_color);
	PngImage png_image(font_info, png_filename, radius, background_color);
	svg_graph << rect_test_1;
	png_image << rect_test_1;
	svg_graph << rect_test_2;
	png_image << rect_test_2;
	svg_graph << circle_test_1;
	png_image << circle_test_1;
	svg_graph << tile;
	png_image << tile;
	svg_graph << line;
	png_image << line;
	svg_graph << arc;
	png_image << arc;
	svg_graph << arc_1;
	png_image << arc_1;
	svg_graph << arc_2;
	png_image << arc_2;
	svg_graph << arc_3;
	png_image << arc_3;
	svg_graph << ribbon_3;
	png_image << ribbon_3;
	svg_graph << ribbon_2;
	png_image << ribbon_2;
	svg_graph << bezier_1;
	png_image << bezier_1;
	svg_graph << bezier_2;
	png_image << bezier_2;
	svg_graph << bezier_3;
	png_image << bezier_3;
	int font_size = 64;
	//LineText line_text(line, u8"笑死人了", "yahei", font_size, Color(128, 128, 128), 1.0);
	//LineText line_text_2(line, u8"笑死人了", "yahei", font_size*1.5, Color(128, 128, 128), 1.0);
	//png_image << line_text;
	//svg_graph << line_text;
}
void arc_test()
{
	int radius = 400;
	Color background_color = Color(255, 105, 180);
	string svg_filename = "circos_arc_test_1.svg";
	string png_filename = "circos_arc_test_1.png";
	std::unordered_map<string_view, pair<string_view, string_view>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	Point center(400, 400);
	auto c_pi = free_angle::from_angle(180);
	Arc arc(300, c_pi * 11 / 6, c_pi / 4, Point(400, 400), false, Color(0, 255, 100), 0, 1.0, 4);
	Arc arc_1(320, c_pi / 4, c_pi / 3, center, false,  Color(128, 128, 128), 0, 1.0, 2);
	Arc arc_2(340, c_pi * 2 / 3, c_pi * 5 / 6, center, false,  Color(128, 128, 128), 0, 1.0, 8);
	Arc arc_3(360, c_pi * 5 / 6 + c_pi, c_pi * 2 / 3 + c_pi, center, false,  Color(128, 128, 128));
	SvgGraph svg_graph(font_info, svg_filename, radius, background_color);
	PngImage png_image(font_info, png_filename, radius, background_color);
	shape_collection shapes;
	shapes.arcs.push_back(arc);
	shapes.arcs.push_back(arc_1);
	shapes.arcs.push_back(arc_2);
	shapes.arcs.push_back(arc_3);
	draw_collections(svg_graph, shapes);
	draw_collections(png_image, shapes);
}


void line_test()
{
	int radius = 400;
	Color background_color = Color(255, 105, 180);
	string svg_filename = "circos_line_test_1.svg";
	string png_filename = "circos_line_test_1.png";
	std::unordered_map<string_view, pair<string_view, string_view>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	Point center(400, 400);
	auto c_pi = free_angle::from_angle(180);
	Line line_1(Point(450, 400), Point(450, 450), Color(0, 255, 100), 10, 1.0);
	Line line_2(Point(500, 400), Point(500, 450), Color(0, 255, 100), 20, 1.0);
	Line line_3(Point(550, 400), Point(550, 450), Color(0, 255, 100), 30, 1.0);
	Line line_4(Point(600, 400), Point(600, 450), Color(0, 255, 100), 40, 1.0);
	SvgGraph svg_graph(font_info, svg_filename, radius, background_color);
	PngImage png_image(font_info, png_filename, radius, background_color);
	shape_collection shapes;
	
	shapes.lines.push_back(line_1);
	shapes.lines.push_back(line_2);
	shapes.lines.push_back(line_3);
	shapes.lines.push_back(line_4);

	draw_collections(svg_graph, shapes);
	draw_collections(png_image, shapes);
}
void circle_test()
{
	int radius = 400;
	Color background_color = Color(255, 105, 180);
	string svg_filename = "circos_circle_test_1.svg";
	string png_filename = "circos_circle_test_1.png";
	std::unordered_map<string_view, pair<string_view, string_view>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	Point center(400, 400);
	auto c_pi = free_angle::from_angle(180);
	Circle circle_1(10, Point(400, 400), Color(0, 255, 100), 1.0, true);
	Circle circle_2(20, Point(450, 400), Color(0, 255, 100), 1.0, true);
	Circle circle_3(40, Point(500, 400), Color(0, 255, 100), 1.0, true);
	SvgGraph svg_graph(font_info, svg_filename, radius, background_color);
	PngImage png_image(font_info, png_filename, radius, background_color);
	shape_collection shapes;
	shapes.circles.push_back(circle_1);
	shapes.circles.push_back(circle_2);
	shapes.circles.push_back(circle_3);
	draw_collections(svg_graph, shapes);
	draw_collections(png_image, shapes);
}