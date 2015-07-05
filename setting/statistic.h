#include "colors.h"
#include "stroke.h"
#include <vector>
using std::vector;
namespace circos
{

	struct value_region
	{
		color statistic_color;
		stat_type draw_type;
		float begin_angle;
		float end_angle;
		float opacity;
		float value;
		value_region() :statistic_color(), begin_angle(0), end_angle(0), opacity(1), draw_type(stat_type::histogram)
		{

		}
	};
	void normalize(vector<vector<value_region>>& in_values,int )
	{
		float min_value, max_value;
		max_value = min_value = in_values[0][0].value;
		for (auto& i : in_values)
		{
			for (auto& j : i)
			{
				if (j.value > max_value)
				{
					max_value = j.value;
				}
				if (j.value < min_value)
				{
					min_value = j.value;
				}
			}
		}
		float gap = max_value - min_value;
		for (auto& i : in_values)
		{
			for (auto& j : i)
			{
				j.value = (j.value - min_value) / gap;
			}
		}
	}


}