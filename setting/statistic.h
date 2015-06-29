#include "colors.h"
#include "stroke.h"
namespace circos
{
	struct statistic_setting
	{
		color statistic_color;
		int begin;
		int end;
		float opacity;
		float value;
		statistic_setting() :statistic_color(), begin(0), end(0), opacity(1)
		{

		}
	};


}