#include "colors.h"
#include <vector>
#include "point.h"
#include <ostream>
#include <sstream>
#include "arc.h"
#include "band.h"
using std::stringstream;
using std::endl;
#pragma once
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
		int begin_from_position;
		int end_from_position;
		int begin_to_position;
		int end_to_position;
		int from_band_index;
		int to_band_index;
		color track_color;
		float opacity;
		Track() :cross(false), on_radius(0), control_radius(0), begin_from_angle(0), end_from_angle(0), begin_to_angle(0), end_to_angle(0)
		{

		}
		Track(const string& input)
		{
			//"Track" from_band(string) begin_from_position(int) end_from_position(int) end_band(string) begin_to_position(int) end_to_position(int) 
			//control_radius(int) cross(bool) track_color(Color) optional[opacity=float]
			stringstream in_stream(input);
			string temp;
			in_stream >> temp;
			if (temp.compare("Track") != 0)
			{
				std::cout << "unexpected beginning " << temp << " while parse Track\n";
				exit(1);
			}
			in_stream >> temp;
			auto index_iterator = band_map_index.find(temp);
			if ((index_iterator) == band_map_index.end())
			{
				std::cout << "unknown band label " << temp << " encountered\n";
				exit(1);
			}
			from_band_index = index_iterator->second;
			in_stream >> begin_from_position>>end_from_position;
			in_stream >> temp;
			index_iterator = band_map_index.find(temp);
			if ((index_iterator) == band_map_index.end())
			{
				std::cout << "unknown band label " << temp << " encountered\n";
				exit(1);
			}
			to_band_index = index_iterator->second;
			in_stream >> begin_to_position >> end_to_position;
			in_stream >> control_radius >> cross>>track_color;
			string optional;
			in_stream >> optional;
			while (optional.length()>0)
			{
				auto delimiter = optional.find('=');
				string option_label = optional.substr(0, delimiter);
				string option_value = optional.substr(delimiter + 1);
				if (option_label.compare("opacity") == 0)
				{
					opacity = stof(option_value);
				}
				else
				{
					std::cout << "unknown optional value " << optional << std::endl;
					exit(1);
				}
				in_stream >> optional;
			}
		}
		friend istream& operator>>(istream& in_stream, Track& in_track)
		{
			string temp;
			in_stream >> temp;
			in_track = Track(temp);
			return in_stream;
		}
		friend ostream& operator<<(ostream& in_stream, const Track& in_track)
		{
			stringstream in_stream(input);
			SvgPoint begin_from;
			SvgPoint end_from;
			SvgPoint begin_to;
			SvgPoint end_to;
			SvgPoint control_point;
			begin_from = SvgPoint(in_track.on_radius, in_track.begin_from_angle);
			end_from = SvgPoint(in_track.on_radius,in_track.end_from_angle);
			begin_to = SvgPoint(in_track.on_radius, in_track.begin_to_angle);
			end_to = SvgPoint(in_track.on_radius,in_track.end_to_angle);
			control_point = SvgPoint(control_radius, (in_track.begin_from_angle +in_track.end_to_angle) / 2);
			if (cross)
			{

				in_stream << "path d=\" M" << begin_from;
				in_stream << BesielLink(in_track.on_radius, in_track.begin_from_angle, in_track.begin_to_angle, control_radius).add_to_path();
				in_stream << CircularArc(in_track.on_radius, in_track.begin_to_angle,in_track.end_to_angle, 1);
				in_stream << BesielLink(in_track.on_radius,in_track.end_to_angle,in_track.end_from_angle, control_radius).add_to_path();
				in_stream << CircularArc(in_track.on_radius,in_track.end_from_angle, in_track.begin_from_angle, 0);
				in_stream << "fill= \"" << track_color << "\" ";
				in_stream << "opacity=\"" << opacity << "\"";
				in_stream << "/>" << endl;
			}
			else
			{
				in_stream << "path d=\" M" << begin_from;
				in_stream << BesielLink(in_track.on_radius,in_track.end_from_angle, in_track.begin_to_angle, control_radius).add_to_path();
				in_stream << CircularArc(in_track.on_radius, in_track.begin_to_angle,in_track.end_to_angle, 1);
				in_stream << BesielLink(in_track.on_radius,in_track.end_to_angle, in_track.begin_from_angle, control_radius).add_to_path();
				in_stream << CircularArc(in_track.on_radius, in_track.begin_from_angle,in_track.end_from_angle, 1);
				in_stream << "fill= \"" << track_color << "\" ";
				in_stream << "opacity=\"" << opacity << "\"";
				in_stream << "/>" << endl;
			}
			return in_stream;
		}
		
	};
	
}