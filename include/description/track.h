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
	struct track
	{
		bool cross;
		double on_radius;
		double control_radius;
		double  begin_from_angle;
		double  end_from_angle;
		double  begin_to_angle;
		double  end_to_angle;
		double begin_from_position;
		double end_from_position;
		double begin_to_position;
		double end_to_position;
		int from_band_index;
		int to_band_index;
		color track_color;
		double  opacity;
		track() :cross(false), on_radius(0), control_radius(0), begin_from_angle(0), end_from_angle(0), begin_to_angle(0), end_to_angle(0), opacity(0)
		{

		}
		track(const string& input)
		{
			//"Track" from_band(string) begin_from_position(int) end_from_position(int) end_band(string) begin_to_position(int) end_to_position(int) 
			//control_radius(int) cross(bool) track_color(Color) optional[opacity=float]
			stringstream in_stream(input);
			string temp;
			in_stream >> temp;
			if (temp.compare("track") != 0)
			{
				std::cout << "unexpected beginning " << temp << " while parse track\n";
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
			opacity = 1.0;
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
				optional.clear();
				in_stream >> optional;
			}
		}
		friend istream& operator>>(istream& in_stream, track& in_track)
		{
			string temp;
			in_stream >> temp;
			in_track = track(temp);
			return in_stream;
		}
		friend ostream& operator<<(ostream& in_stream, const track& in_track)
		{
			SvgPoint begin_from;
			SvgPoint end_from;
			SvgPoint begin_to;
			SvgPoint end_to;
			SvgPoint control_point;
			begin_from = SvgPoint(in_track.on_radius, in_track.begin_from_angle);
			end_from = SvgPoint(in_track.on_radius,in_track.end_from_angle);
			begin_to = SvgPoint(in_track.on_radius, in_track.begin_to_angle);
			end_to = SvgPoint(in_track.on_radius,in_track.end_to_angle);
			control_point = SvgPoint(in_track.control_radius, (in_track.begin_from_angle +in_track.end_to_angle) / 2);
			if (in_track.cross)
			{

				in_stream << "<path d=\" M" << begin_from;
				in_stream << besiel_link(in_track.on_radius, in_track.begin_from_angle, in_track.begin_to_angle, in_track.control_radius).add_to_path();
				in_stream << circular_arc(in_track.on_radius, in_track.begin_to_angle,in_track.end_to_angle, 1);
				in_stream << besiel_link(in_track.on_radius,in_track.end_to_angle,in_track.end_from_angle, in_track.control_radius).add_to_path();
				in_stream << circular_arc(in_track.on_radius, in_track.end_from_angle, in_track.begin_from_angle, 0) << "\" ";
				in_stream << "fill= \"" << in_track.track_color << "\" ";
				in_stream << "opacity=\"" << in_track.opacity << "\"";
				in_stream << "/>" << endl;
			}
			else
			{
				in_stream << "<path d=\" M" << begin_from;
				in_stream << besiel_link(in_track.on_radius, in_track.begin_from_angle, in_track.end_to_angle, in_track.control_radius).add_to_path();
				in_stream << circular_arc(in_track.on_radius, in_track.end_to_angle, in_track.begin_to_angle, 0);
				in_stream << besiel_link(in_track.on_radius, in_track.begin_to_angle, in_track.end_from_angle, in_track.control_radius).add_to_path();
				in_stream << circular_arc(in_track.on_radius, in_track.end_from_angle, in_track.begin_from_angle, 0) << "\" ";
				in_stream << "fill= \"" << in_track.track_color << "\" ";
				in_stream << "opacity=\"" << in_track.opacity << "\"";
				in_stream << "/>" << endl;
			}
			return in_stream;
		}
		
	};
	
}