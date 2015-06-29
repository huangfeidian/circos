#include "colors.h"
#include "stroke.h"
namespace circos
{
	struct band_setting
	{
		color band_color;
		int begin;
		int end;
		float opacity;
		band_setting() :band_color(), begin(0), end(0), opacity(1)
		{

		}
	};


}