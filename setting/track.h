#include "colors.h"
#include <vector>
#include "point.h"
#include <ostream>
namespace circos
{
	struct Track
	{
		bool cross;
		int on_radius;
		int control_radius;
		float begin_from_angle;
		float end_from_angle;
		float begin_to_angle;
		float end_to_angle;
		color track_color;
		float opacity;
		Track() :cross(false), on_radius(0), control_radius(0), begin_from_angle(0), end_from_angle(0), begin_to_angle(0), end_to_angle(0)
		{

		}
		friend ostream& operator<<(ostream& in_stream, Track in_track)
		{
			SvgPoint begin_from;
			SvgPoint end_from;
			SvgPoint begin_to;
			SvgPoint end_to;
			SvgPoint control_point;
			begin_from = SvgPoint(in_track.on_radius, in_track.begin_from_angle);
			end_from = SvgPoint(in_track.on_radius, in_track.end_from_angle);
			begin_to = SvgPoint(in_track.on_radius, in_track.begin_to_angle);
			end_to = SvgPoint(in_track.on_radius, in_track.end_to_angle);
			control_point = SvgPoint(in_track.control_radius, (in_track.begin_from_angle + in_track.end_to_angle) / 2);
			if (in_track.cross)
			{

				in_stream << "path d=\" M" << begin_from;
				in_stream << "Q " << control_point << begin_to;
				in_stream << CircularArc(end_to, in_track.on_radius, 0, 1);
				in_stream << "Q " << control_point << end_from;
				in_stream << CircularArc(begin_from, in_track.on_radius, 0, 0);
				in_stream << "fill= \"" << in_track.track_color << "\" ";
				in_stream << "opacity=\"" << in_track.opacity << "\"";
				in_stream << "/>" << endl;
			}
			else
			{
				in_stream << "path d=\" M" << begin_from;
				in_stream << CircularArc(end_from, in_track.on_radius, 0, 1);
				in_stream << "Q " << control_point << begin_to;
				in_stream << CircularArc(end_to, in_track.on_radius, 0, 1);
				in_stream << "Q " << control_point << begin_from;
				in_stream << "fill= \"" << in_track.track_color << "\" ";
				in_stream << "opacity=\"" << in_track.opacity << "\"";
				in_stream << "/>" << endl;
			}
			return in_stream;

		}
	};
}