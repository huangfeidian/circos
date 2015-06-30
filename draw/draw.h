#include "../setting/track.h"
#include "../setting/circle.h"
#include "../setting/arc.h"
#include "../setting/glyph.h"
#include "../setting/band.h"
#include "../setting/statistic.h"
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
namespace circos
{
#define PI 3.14159
	using std::vector;
	using std::endl;
	int background_radius;
	color backgroud_color;
	struct SvgPoint
	{
		int x;
		int y;
		SvgPoint():x(0),y(0)
		{

		}
		SvgPoint(int in_x, int in_y) :x(in_x), y(in_y)
		{

		}
	};
	SvgPoint polar_to_catersian(int radius, float angle)
	{
		int dx, dy;
		dx = radius*sin(angle);
		dy = -radius*cos(angle);
		return SvgPoint(dx+background_radius, dy+background_radius);

	}
	struct CircularArc
	{
		SvgPoint to_point;
		int radius;
		int large_flag;
		int sweep_flag;
		CircularArc() :radius(0), large_flag(0), sweep_flag(0)
		{

		}
		CircularArc(SvgPoint point, int in_radius,int in_large, int in_sweep)
			:to_point(point), radius(in_radius),large_flag(in_large), sweep_flag(in_sweep)
		{

		}
	};
	struct BesielLink
	{
		int on_radius;
		int control_radius;
		float begin_angle;
		float end_angle;
		int stroke_width;
		color link_color;
		float opacity;
		BesielLink() :on_radius(0), control_radius(0), begin_angle(0), end_angle(0), stroke_width(0), opacity(0)
		{

		}


	};
	ostream& operator<<(ostream& in_stream, SvgPoint in_point)
	{
		in_stream << in_point.x << "," << in_point.y << " ";
		return in_stream;
	}
	ostream& operator<<(ostream& in_stream, CircularArc in_arc)
	{
		in_stream <<  " A " << in_arc.radius << "," << in_arc.radius << " ";
		in_stream << 0 << " " << in_arc.large_flag << "," << in_arc.sweep_flag << " ";
		in_stream << in_arc.to_point;
		return in_stream;
	}
	ostream& operator<<(ostream& in_stream, BesielLink in_link)
	{
		//<path d="M200,300 Q400,50 600,300 " fill = "none" stroke = "red" stroke - width = "5" / >
		SvgPoint from_point;
		SvgPoint to_point;
		SvgPoint control_point;
		from_point = polar_to_catersian(in_link.on_radius, in_link.begin_angle);
		to_point = polar_to_catersian(in_link.on_radius, in_link.end_angle);
		if (abs(in_link.end_angle - in_link.begin_angle) < PI / 2)
		{
			control_point = polar_to_catersian(in_link.control_radius, (in_link.begin_angle + in_link.end_angle) / 2);
		}
		else
		{
			control_point = polar_to_catersian(in_link.control_radius, (in_link.begin_angle + in_link.end_angle) / 2-PI/2);
		}
		in_stream << "<path d=\" ";
		in_stream << "M " << from_point<<" ";
		in_stream << "Q" << control_point << " ";
		in_stream << to_point << " ";
		in_stream << "fill= \"none\"" << " ";
		in_stream << "stroke=\"" << in_link.link_color << "\" ";
		in_stream << "stroke-width=\"" << in_link.stroke_width << "\" ";
		in_stream << "opacity=\"" << in_link.opacity << "\" ";
		in_stream << "/>" << endl;

	}
	void draw(std::stringstream& output, const vector<circle_setting>& circles)
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
	void draw(std::stringstream& output, const circle_setting& on_circle, const vector<band_setting>& bands)
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
			p_1 = polar_to_catersian(on_circle.inner_radius, angle_begin);
			p_2 = polar_to_catersian(on_circle.outer_radius, angle_begin);
			p_3 = polar_to_catersian(on_circle.outer_radius, angle_end);
			p_4 = polar_to_catersian(on_circle.inner_radius, angle_end);
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
	void draw(std::stringstream& output, const circle_setting& on_circle, const vector<arc_setting>& arcs)
	{

	}
	void draw(std::stringstream& output, const circle_setting& on_circle, const vector<track_setting>& tracks)
	{

	}
	void draw(std::stringstream& output, const circle_setting& on_cicle, const vector<statistic_setting> statistics)
	{

	}
}