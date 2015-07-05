#include "colors.h"
#include "stroke.h"
namespace circos
{
	struct circle_setting
	{
		int inner_radius;
		int outer_radius;
		color circle_color;
		color heat_color[2];
		float opacity;
		bool label_on;
		stroke_setting circle_stroke;
		int gap;
		circle_setting() :inner_radius(0), outer_radius(0), circle_color(), opacity(1), label_on(false), circle_stroke(), gap(20)
		{
			heat_color[0] = heat_color[1] = circle_color;
		}
	};
};