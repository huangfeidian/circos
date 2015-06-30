#include <string>
#include <iostream>
#include <fstream>
namespace circos
{
	using std::ostream;
	struct color
	{
		int r;
		int g;
		int b;
		color():r(-1),g(-1),b(-1)
		{

		}
		color(std::ifstream input)
		{
			
		}
		
	};
	ostream& operator<<(ostream& in_stream,const color& in_color)
	{
		in_stream << "rgb(" << in_color.r << "," << in_color.g << "," << in_color.b << ")";
		return in_stream;
	}
	struct gradient
	{
		color color_begin;
		color color_end;
	};

};