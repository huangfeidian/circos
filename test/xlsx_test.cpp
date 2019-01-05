#include <circos/xlsx/file_convert.h>
using namespace circos;
int main(int argc, char** argv)
{
	string from_xlsx_name = "../data/pi10000.xlsx";
	string to_png_file_name = "xlsx_shape_test.png";
	string to_svg_file_name = "xlsx_shape_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
}