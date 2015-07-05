#include <string>
#include <iostream>
#include <fstream>
using std::ostream;
namespace circos
{

	struct color
	{
		int r;
		int g;
		int b;
		color():r(-1),g(-1),b(-1)
		{

		}
		void set_gradient(color begin_color, color end_color, float percentage)
		{
			r = begin_color.r + (end_color.r - begin_color.r)*percentage;
			g = begin_color.g + (end_color.g - begin_color.g)*percentage;
			b = begin_color.b + (end_color.b - begin_color.b)*percentage;
		}
		
	};
	ostream& operator<<(ostream& in_stream,const color& in_color)
	{
		in_stream << "rgb(" << in_color.r << "," << in_color.g << "," << in_color.b << ")";
		return in_stream;
	}


};