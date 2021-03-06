﻿#pragma once
#include <string>
#include <string_view>
#include "../basics/point.h"
#include "../basics/color.h"
#include "../basics/constants.h"
#include "line.h"
namespace spiritsaway::circos
{
	using std::string_view;
	struct LineText
	{
		string_view font_name;
		string_view utf8_text;
		std::uint16_t font_size;
		Line on_line;
		Color color;
		float opacity;
		LineText(Line in_on_line, string_view in_utf8_text, string_view in_font_name, int in_font_size, Color in_color, float in_opacity = 1.0)
			:on_line(in_on_line), utf8_text(in_utf8_text), font_name(in_font_name), font_size(in_font_size), color(in_color), opacity(in_opacity)
		{

		}
	};
}