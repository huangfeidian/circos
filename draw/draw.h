#include "../setting/track.h"
#include "../setting/circle.h"
#include "../setting/arc.h"
#include "../setting/band.h"
#include "../setting/statistic.h"
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
	
	
	
	
	

	
	void draw_label_band(ostream& output, const circle& on_circle, const vector<band>& bands)
	{

	}
	void draw_tick_band(ostream& output, const circle& on_circle, const vector<band>& bands)
	{

	}
	void draw_fill_band(ostream& output, const circle& on_circle, const vector<band>& bands)
	{
		//<path d="M300,200 h-150 a150,150 0 1,0 150,-150 z" fill = "red" stroke = "blue" stroke - width = "5" / >
		int band_number = bands.size();
		int band_size = 0;
		for (auto i : bands)
		{
			band_size += i.end - i.begin;
		}
		float pixel_per_unit = (2 * PI*on_circle.inner_radius - band_number*on_circle.gap) / band_size;
		SvgPoint p_1, p_2, p_3, p_4;
		int band_begin, band_end;
		band_begin = 0;
		int sweep_flag = 1;
		int large_arc = 0;
		CircularArc arc_to_draw;
		for (auto i : bands)
		{
			//对于这个band 我们需要先向上，然后顺时针，然后向下，然后逆时针
			float angle_begin = band_begin / on_circle.inner_radius;
			float angle_end = angle_begin+((i.end-i.begin)*pixel_per_unit)/on_circle.inner_radius;
			p_1 = SvgPoint(on_circle.inner_radius, angle_begin);
			p_2 = SvgPoint(on_circle.outer_radius, angle_begin);
			p_3 = SvgPoint(on_circle.outer_radius, angle_end);
			p_4 = SvgPoint(on_circle.inner_radius, angle_end);
			if (angle_end - angle_begin > PI / 2)//这个圆弧超过了半个圆，需要考虑large_arc的问题
			{
				large_arc = 1;
			}
			output << "<path d=\"";
			//output << " M " << p_1.x << "," << p_1.y;
			output<<" M " << p_2;
			output << CircularArc(p_3, on_circle.outer_radius, large_arc, 1);
			output << "L " <<p_4;
			output << CircularArc(p_1, on_circle.inner_radius, large_arc, 0);
			output << "z\" ";
			output << "fill=\"" << i.band_color<< "\" ";
			output << "opacity=\"" << i.opacity << "\"";
			output << "/>" << endl;
			band_begin += on_circle.gap + (i.end - i.begin)*pixel_per_unit;

		}
	}
	void draw(ostream& output, const circle& on_circle, const vector<BesielLink>& BesielLinks)
	{
		for (auto i : BesielLinks)
		{
			output << i;
		}
	}
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