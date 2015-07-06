#include "../setting/track.h"
#include "../setting/circle.h"
#include "../setting/arc.h"
#include "../setting/band.h"
#include "../setting/enum.h"

#include <sstream>
#include <string>
#include <vector>
#include <math.h>
using std::vector;
using std::endl;
namespace circos
{

	int background_radius;
	color backgroud_color;

	void draw(ostream& output, const circle& on_circle, const vector<Track>& tracks)
	{
		for (auto i : tracks)
		{
			output << i;
		}
	}
	void draw(ostream& output, const circle& on_cicle, const vector<value_onband> statistics)
	{
		stat_type type = statistics[0].draw_type;
		switch (type)
		{
		case stat_type::linechart:


		}
	}
}