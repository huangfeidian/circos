#include "colors.h"
#include <vector>
namespace circos
{
	struct track_setting
	{
		color tract_color;
		float begin_1, end_1;//all the angles
		float begin_2, end_2;
		float opacity;
		bool twist;
	};
}