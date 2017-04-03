#pragma once
#include "line.h"
namespace circos
{
	struct LineText
	{
		string font_name;//字体名称
		string utf8_text;//文字
		int font_size;//字体大小
		Line on_line;//所在的射线
		Color color;//字体大小
		double opacity;
		LineText(Line in_on_line, const string& in_utf8_text, const string& in_font_name, int in_font_size, Color in_color, double in_opacity = 1.0)
			:on_line(in_on_line), utf8_text(in_utf8_text), font_name(in_font_name), font_size(in_font_size), color(in_color), opacity(in_opacity)
		{

		}
	};
}