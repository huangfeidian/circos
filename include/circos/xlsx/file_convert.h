#pragma once
#include <string>
#include "../model.h"

#ifdef WITH_XLSX_SUPPORT
#include <xlsx_reader/xlsx_archive>

namespace circos
{
	void read_xlsx_and_draw(const string& xlsx_path, const string& png_output_file, const string& svg_output_file);
	model::model read_model_from_workbook(const xlsx_reader::workbook<xlsx_reader::typed_worksheet>& in_workbook);
}
#endif


