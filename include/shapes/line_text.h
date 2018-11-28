#pragma once
#include "../basics/point.h"
#include "../basics/color.h"
namespace circos
{
	struct LineText
	{
		string font_name;
		string utf8_text;
		int font_size;
		Line on_line;
		Color color;
		double opacity;
		LineText(Line in_on_line, const string& in_utf8_text, const string& in_font_name, int in_font_size, Color in_color, double in_opacity = 1.0)
			:on_line(in_on_line), utf8_text(in_utf8_text), font_name(in_font_name), font_size(in_font_size), color(in_color), opacity(in_opacity)
		{

		}
	};
}