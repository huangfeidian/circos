#pragma once
#include "line.h"
namespace circos
{
	struct line_text
	{
		File* font_file;//�����ļ�
		const string utf8_text;//����
		int font_size;//�����С
		Line on_line;//���ڵ�����
		Color color;
		double opacity;
	};
}