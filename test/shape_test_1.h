#include <circos/drawer/png_drawer.h>
#include <circos/drawer/svg_drawer.h>
#include <circos/shape_collection.h>
#include <map>
using namespace circos;
using namespace std;
constexpr auto pi = 3.1415926;
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
	Circle circle_test_1(360, Point(400, 400), Color(0, 255, 180), 0.5, false);
	Ring ring(center, 300, 350, pi * 11 / 6, pi / 4, Color(200, 230, 108));
	Arc arc(300, 11 * pi / 6, pi / 4, Point(400, 400), Color(0, 255, 100), 0, 1.0, 4);
	Arc arc_1(360, pi / 6, pi / 3, center, Color(128, 128, 128), 0, 1.0, 2);
	Arc arc_2(360, pi * 2 / 3, pi * 5 / 6, center, Color(128, 128, 128), 0, 1.0, 8);
	Arc arc_3(360, pi * 5 / 6 + pi, pi * 2 / 3 + pi, center, Color(128, 128, 128));
	std::unordered_map<string, pair<string, string>> font_info{ {"yahei",make_pair("C:/Windows/Fonts/msyhl.ttc", "microsoft yahei")} };
	auto sme = font_info.lower_bound("nima");
	Bezier t_bezier_1(arc_1.to_point, arc_3.from_point, center, arc_1.color);
	Bezier t_bezier_2(arc_3.to_point, arc_1.from_point, center, arc_1.color);
	Track track_1(arc_1, arc_3, t_bezier_1, t_bezier_2, arc_1.color,true);
	Track track_2(center, 360, pi / 6 + pi, pi / 3 + pi, pi * 2 / 3 + pi, pi * 5 / 6 + pi, arc_1.color,0,false,false
	);
	Track track_3(center, 360, pi / 6 + pi, pi / 3 + pi, pi * 2 / 3 + pi, pi * 5 / 6 + pi, arc_1.color, 50, false, false
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
	svg_graph << ring;
	png_image << ring;
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
	svg_graph << track_3;
	png_image << track_3;
	svg_graph << track_2;
	png_image << track_2;
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
	Arc arc(300, 11 * pi / 6, pi / 4, Point(400, 400), Color(0, 255, 100), 0, 1.0, 4);
	Arc arc_1(360, pi / 6, pi / 3, center, Color(128, 128, 128), 0, 1.0, 2);
	Arc arc_2(360, pi * 2 / 3, pi * 5 / 6, center, Color(128, 128, 128), 0, 1.0, 8);
	Arc arc_3(360, pi * 5 / 6 + pi, pi * 2 / 3 + pi, center, Color(128, 128, 128));
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