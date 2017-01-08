#pragma once
#include "line.h"
namespace circos
{
	struct line_text
	{
		File* font_file;//字体文件
		const string utf8_text;//文字
		int font_size;//字体大小
		Line on_line;//所在的射线
		Color color;
		double opacity;
	};
}