
#include <circos/shape_collection.h>
#include <circos/read_excel.h>

#include <map>
#include <iostream>
#include <string_view>
using namespace spiritsaway::circos;
using namespace std;

int main()
{
    string from_xlsx_name = "../data/pi10000.xlsx";
	string to_png_file_name = "xlsx_pi10000_test.png";
	string to_svg_file_name = "xlsx_pi10000_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
    return 0;
}