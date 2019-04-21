#include <circos/xlsx/read_excel.h>
using namespace circos;
void pi_test()
{
	string from_xlsx_name = "../data/pi10000.xlsx";
	string to_png_file_name = "xlsx_pi10000_test.png";
	string to_svg_file_name = "xlsx_pi10000_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
}
void tick_test()
{
	string from_xlsx_name = "../data/tick_test.xlsx";
	string to_png_file_name = "xlsx_tick_test.png";
	string to_svg_file_name = "xlsx_tick_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
}
void range_links_test()
{
	string from_xlsx_name = "../data/range_link_test.xlsx";
	string to_png_file_name = "xlsx_range_link_test.png";
	string to_svg_file_name = "xlsx_range_link_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
}
void tick_label_test()
{
	string from_xlsx_name = "../data/tick_label_test.xlsx";
	string to_png_file_name = "tick_label_test.png";
	string to_svg_file_name = "tick_label_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
}

void point_track_test()
{
	string from_xlsx_name = "../data/point_track_test.xlsx";
	string to_png_file_name = "point_track_test.png";
	string to_svg_file_name = "point_track_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
}
int main(int argc, char** argv)
{
	point_track_test();
}