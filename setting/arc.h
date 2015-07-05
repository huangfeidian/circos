#include "colors.h"
#include "point.h"
#include "global_container.h"
#include <sstream>
using std::endl;
using std::stringstream;
namespace circos
{

#define PI 3.14159f
	struct CircularArc
	{
		SvgPoint from_point;
		SvgPoint to_point;
		int radius;
		int large_flag;
		int sweep_flag;
		CircularArc() :radius(0), large_flag(0), sweep_flag(0)
		{

		}
		CircularArc(int in_radius, float begin_angle, float end_angle,int in_sweep_flag=1) 
			:from_point(in_radius, begin_angle), to_point(in_radius, end_angle), sweep_flag(in_sweep_flag)
		{
			if (abs(end_angle - begin_angle )> PI / 2)
			{
				large_flag = 1;
			}
			else
			{
				large_flag = 0;
			}
		}
		
		friend ostream& operator<<(ostream& in_stream, CircularArc in_arc)
		{
			in_stream << " A " << in_arc.radius << "," << in_arc.radius << " ";
			in_stream << 0 << " " << in_arc.large_flag << "," << in_arc.sweep_flag << " ";
			in_stream << in_arc.to_point;
			return in_stream;
		}
		int to_path()
		{
			//<path id="textPath" d="M 250 500 A 250,250 0 1 1 250 500.0001"/>
			stringstream path_string;
			path_string << "<path id=\"textPath" << path_define.size() << "\" ";
			path_string << "d= \" M " << from_point.x << " " << from_point.y << " ";
			path_string << *this;
			path_string << "/>\n";
			path_define.push_back(path_string.str());
			return path_define.size() - 1;
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
		friend ostream& operator<<(ostream& in_stream, BesielLink in_link)
		{
			//<path d="M200,300 Q400,50 600,300 " fill = "none" stroke = "red" stroke - width = "5" / >
			SvgPoint from_point;
			SvgPoint to_point;
			SvgPoint control_point;
			from_point = SvgPoint(in_link.on_radius, in_link.begin_angle);
			to_point = SvgPoint(in_link.on_radius, in_link.end_angle);
			if (abs(in_link.end_angle - in_link.begin_angle) < PI / 2)
			{
				control_point = SvgPoint(in_link.control_radius, (in_link.begin_angle + in_link.end_angle) / 2);
			}
			else
			{
				control_point = SvgPoint(in_link.control_radius, (in_link.begin_angle + in_link.end_angle) / 2 - PI / 2);
			}
			in_stream << "<path d=\" ";
			in_stream << "M " << from_point;
			in_stream << "Q" << control_point;
			in_stream << to_point;
			in_stream << "fill= \"none\"" << " ";
			in_stream << "stroke=\"" << in_link.link_color << "\" ";
			in_stream << "stroke-width=\"" << in_link.stroke_width << "\" ";
			in_stream << "opacity=\"" << in_link.opacity << "\" ";
			in_stream << "/>" << endl;

		}
	};
	void draw(ostream& output, const circle& on_circle, const vector<BesielLink>& BesielLinks)
	{
		for (auto i : BesielLinks)
		{
			output << i;
		}
	}
}