#include <string>
#include <istream>
#include <fstream>
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
		color(std::ifstream input)
		{
			
		}
	};
	struct gradient
	{
		color color_begin;
		color color_end;
	};

};