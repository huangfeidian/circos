#include "colors.h"
#include "stroke.h"
#include "enum.h"
#include <string>
namespace circos
{
	using std::string;
	struct onband
	{
		color band_color;
		int begin;
		int end;
		float opacity;
		onband() :band_color(), begin(0), end(0), opacity(1)
		{

		}
	};
	struct fill_onband
	{
		color fill_color;
		int begin;
		int end;
		float opacity;
		fill_onband() :fill_color(), begin(0), end(0), opacity(1)
		{

		}
	};
	struct label_onband
	{
		color font_color;
		int begin;
		int end;
		int align;//左对齐=1 居中为2，右对齐为3
		int font_size;
		string text;
		float opacity;
	};
	struct tick_onband
	{
		color tick_color;
		int position;
		int width;
		int height;
		float opacity;
	};
	struct value_onband
	{
		color value_color;
		stat_type draw_type;
		float begin_angle;
		float end_angle;
		float opacity;
		float value;
		value_onband() :value_color(), begin_angle(0), end_angle(0), opacity(1), draw_type(stat_type::histogram)
		{

		}
	};

}