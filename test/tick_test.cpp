
#include <circos/shape_collection.h>
#include <circos/read_excel.h>
#include <map>
#include <iostream>
#include <string_view>
using namespace spiritsaway::circos;
using namespace std;

void tick_test()
{
	string from_xlsx_name = "../data/tick_test.xlsx";
	string to_png_file_name = "xlsx_tick_test.png";
	string to_svg_file_name = "xlsx_tick_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
}

void tick_label_test()
{
	string from_xlsx_name = "../data/tick_label_test.xlsx";
	string to_png_file_name = "tick_label_test.png";
	string to_svg_file_name = "tick_label_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
}

int main()
{
    tick_label_test();
    tick_test();
    return 0;
}