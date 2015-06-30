#include "colors.h"
#include "stroke.h"
namespace circos
{
	struct statistic_setting
	{
		color statistic_color;
		float begin_angle;
		float end_angle;
		float opacity;
		float value;
		statistic_setting() :statistic_color(), begin_angle(0), end_angle(0), opacity(1)
		{

		}
	};


}