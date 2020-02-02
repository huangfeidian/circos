#pragma once
#include "arc.h"
#include "../basics/constants.h"

namespace spiritsaway::circos
{
    struct ArcText
    {
        Arc on_arc;
        std::string_view font_name;
		std::string_view utf8_text;
		std::uint16_t font_size;
        Color color;
		float opacity;
        ArcText(Arc in_on_arc, std::string_view in_utf8_text, std::string_view in_font_name, int in_font_size, Color in_color, float in_opacity = 1.0)
        : on_arc(in_on_arc), utf8_text(in_utf8_text), font_name(in_font_name), font_size(in_font_size), color(in_color), opacity(in_opacity)
        {

        }
	};
}