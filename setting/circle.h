#include "colors.h"
using std::vector;
namespace circos
{
	struct circle
	{
		int inner_radius;
		int outer_radius;
		color circle_color;
		color heat_color[2];
		float opacity;
		bool label_on;
		int gap;
		circle() :inner_radius(0), outer_radius(0), circle_color(), opacity(1), label_on(false), gap(20)
		{
			heat_color[0] = heat_color[1] = circle_color;
		}
	};

	void draw(ostream& output, const vector<circle>& circles)
	{
		//<circle cx = "50" cy = "50" r = "40" stroke = "black" stroke - width = "3" fill = "red" / >
		int cx, cy;
		cx = cy = background_radius;
		int r;
		int stroke_breadth;
		for (auto i : circles)//我们以画边框的形式来画圆弧
		{
			r = i.inner_radius;
			stroke_breadth = i.outer_radius - i.inner_radius;
			if (i.circle_color.b > 0)
			{
				output << "circle cx =\"" << cx << "\" cy=\"" << cy << "\" r=" << r << " stroke=\"" << i.circle_color << "\" stroke-width=\"" << stroke_breadth << "\" opacity=\"" << 0 << "\"/>" << endl;
			}
		}
	}
};