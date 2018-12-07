#include <circos/drawer/png_drawer.h>
#include <circos/drawer/svg_drawer.h>
#include <circos/shape_collection.h>
#include <circos/basics/constants.h>
#include <map>
#include <iostream>
using namespace circos;
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
	auto c_pi = 180 * amplify_angle::factor;
	Circle circle_test_1(360, Point(400, 400), Color(0, 255, 180), 0.5, false);
	Tile tile(center, 300, 350, c_pi * 11 / 6, c_pi / 4, Color(200, 230, 108));
	Arc arc(300, 11 * c_pi / 6, c_pi / 4, Point(400, 400), Color(0, 255, 100), 0, 1.0, 4);
	Arc arc_1(360, c_pi / 6, c_pi / 3, center, Color(128, 128, 128), 0, 1.0, 2);
	Arc arc_2(360, c_pi * 2 / 3, c_pi * 5 / 6, center, Color(128, 128, 128), 0, 1.0, 8);
	Arc arc_3(360, c_pi * 5 / 6 + c_pi, c_pi * 2 / 3 + c_pi, center, Color(128, 128, 128));
	std::unordered_map<string, pair<string, string>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	auto sme = font_info.lower_bound("nima");
	Bezier t_bezier_1(arc_1.to_point, arc_3.from_point, center, arc_1.color);
	Bezier t_bezier_2(arc_3.to_point, arc_1.from_point, center, arc_1.color);
	Ribbon ribbon_1(arc_1, arc_3, t_bezier_1, t_bezier_2, arc_1.color,true);
	Ribbon ribbon_2(center, 360, c_pi / 6 + c_pi, c_pi / 3 + c_pi, c_pi * 2 / 3 + c_pi, c_pi * 5 / 6 + c_pi, arc_1.color,0,false,false
	);
	Ribbon ribbon_3(center, 360, c_pi / 6 + c_pi, c_pi / 3 + c_pi, c_pi * 2 / 3 + c_pi, c_pi * 5 / 6 + c_pi, arc_1.color, 50, false, false
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
	std::unordered_map<string, pair<string, string>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	Point center(400, 400);
	auto c_pi = 180 * amplify_angle::factor;
	Arc arc(300, 11 * c_pi / 6, c_pi / 4, Point(400, 400), Color(0, 255, 100), 0, 1.0, 4);
	Arc arc_1(360, c_pi / 6, c_pi / 3, center, Color(128, 128, 128), 0, 1.0, 2);
	Arc arc_2(360, c_pi * 2 / 3, c_pi * 5 / 6, center, Color(128, 128, 128), 0, 1.0, 8);
	Arc arc_3(360, c_pi * 5 / 6 + c_pi, c_pi * 2 / 3 + c_pi, center, Color(128, 128, 128));
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
void circle_test()
{
	auto c_pi = 180 * amplify_angle::factor;
	auto path_points = Arc::arc_path(0, c_pi / 4, 100);
	for (const auto& i : path_points)
	{
		cout << i.x << "," << i.y<<endl;
	}

}
void flood_test()
{
	int radius = 2000;
	Point center(radius, radius);
	Color background_color = Color(255, 255, 255);
	Tile cur_tile(center, 1600, 1700, amplify_angle::rad_to_angle_percent(4.4369), amplify_angle::rad_to_angle_percent(4.9811), Color(0, 0, 0), 1, true, 1);
	string png_filename = "circos_arc_test_1.png";
	std::unordered_map<string, pair<string, string>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	PngImage png_image(font_info, png_filename, radius, background_color);
	shape_collection shapes;
	shapes.tiles.push_back(cur_tile);
	draw_collections(png_image, shapes);
}
void line_test()
{
	Line temp_line_1(Point(2451, 362), Point(2448, 360));
	auto path = temp_line_1.path();
	for (auto one_point : path)
	{
		cout << "x " << one_point.x << " y" << one_point.y << endl;
	}
}