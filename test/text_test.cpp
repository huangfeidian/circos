#include <circos/drawer/png_drawer.h>
#include <circos/drawer/svg_drawer.h>
#include <circos/shape_collection.h>
#include <circos/basics/constants.h>
#include <circos/xlsx/read_excel.h>

#include <map>
#include <iostream>
#include <string_view>
using namespace spiritsaway::circos;
using namespace std;

int main()
{
    string from_xlsx_name = "../data/text_test.xlsx";
	string to_png_file_name = "text_test.png";
	string to_svg_file_name = "text_test.svg";
	read_xlsx_and_draw(from_xlsx_name, to_png_file_name, to_svg_file_name);
    return 0;
}