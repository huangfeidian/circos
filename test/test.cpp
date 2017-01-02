#include <drawer/png_drawer.h>
#include <drawer/svg_drawer.h>
using namespace circos;
using namespace std;

int main()
{
	int radius = 400;
	Color background_color = Color(255, 105, 180);
	string svg_filename = "circos.svg";
	string png_filename = "circos.png";
	Line line(Point(400, 400), Point(400, 500), Color(0, 255, 180), 4);
	Rectangle rect_test_1(Point(100, 100), Point(200, 100), Color(128, 105, 180), 100, true);
	Rectangle rect_test_2(Point(100, 100), Point(200, 200), Color(0, 105, 180), 141, true,0.5);
	Circle circle_test_1(200, Point(300, 300), Color(0, 255, 180), 0.5, true);
	Arc arc(300, 0, 3.1415, Point(400, 400), Color(0, 255, 100));
	SvgGraph svg_graph(svg_filename, radius, background_color);
	PngImage png_image(png_filename, radius, background_color);
	svg_graph << rect_test_1;
	png_image << rect_test_1;
	svg_graph << rect_test_2;
	png_image << rect_test_2;
	svg_graph << circle_test_1;
	png_image << circle_test_1;
	svg_graph << line;
	png_image << line;
	svg_graph << arc;
	png_image << arc;
}