#include "point.h"
#include "global_container.h"
#include <sstream>
#pragma once
using std::endl;
using std::stringstream;
namespace circos
{
#define EPS 0.04
#define PI 3.14159f
	struct circular_arc
	{
		SvgPoint from_point;
		SvgPoint to_point;
		double radius;
		int large_flag;
		int sweep_flag;
		circular_arc() :radius(0), large_flag(0), sweep_flag(0)
		{

		}
		circular_arc(double in_radius, double begin_angle, double end_angle,int in_sweep_flag=1) 
			: sweep_flag(in_sweep_flag), radius(in_radius)
		{
			from_point = SvgPoint(in_radius, begin_angle);
			to_point = SvgPoint(in_radius, end_angle);
			//if (begin_angle < end_angle)
			//{
			//	from_point = SvgPoint(in_radius, begin_angle);
			//	to_point = SvgPoint(in_radius, end_angle);
			//}
			//else
			//{
			//	from_point = SvgPoint(in_radius, end_angle);
			//	to_point = SvgPoint(in_radius, begin_angle);
			//}
			if (abs(end_angle - begin_angle )> PI )
			{
				large_flag = 1;
			}
			else
			{
				large_flag = 0;
			}
		}
		friend ostream& operator<<(ostream& in_stream, circular_arc in_arc)//add to path
		{
			in_stream << " A " << in_arc.radius << "," << in_arc.radius << " ";
			in_stream << 0 << " " << in_arc.large_flag << "," << in_arc.sweep_flag << " ";
			in_stream << in_arc.to_point;
			return in_stream;
		}
		int convert_to_path(ostream& in_stream)
		{
			//<path id="textPath" d="M 250 500 A 250,250 0 1 1 250 500.0001"/>
			in_stream << "<path id=\"textPath" << path_index << "\" ";
			in_stream << "d= \" M " << from_point.x << " " << from_point.y << " ";
			in_stream << *this;
			in_stream << "\" fill=\"none\"/>\n";
			path_index++;
			return path_index-1;
		}
	};
	
	struct besiel_link
	{
		double on_radius;
		double control_radius;
		double from_angle;
		double to_angle;
		double from_position;
		double to_position;
		int from_band_index;
		int to_band_index;
		double stroke_width;
		color link_color;
		double opacity;
		besiel_link() :on_radius(0), control_radius(0), from_angle(0), to_angle(0), stroke_width(0), opacity(0)
		{

		}
		besiel_link(double in_on_radius, double in_begin_angle, double in_end_angle, double in_control_radius)
			:on_radius(in_on_radius), from_angle(in_begin_angle), to_angle(in_end_angle), control_radius(in_control_radius)
		{

		}
		besiel_link(const string& input)
		{
			stringstream in_stream(input);
			//"Link" band_from(string) position_from(int) band_to(string) position_to(int) control_radius(int) stroke_width(int) link_color(color) optional[opacity(float)]
			string temp;
			in_stream >> temp;
			if (temp.compare("link") != 0)
			{
				std::cout << "unexpected beginning " << temp << "while parsing  link\n";
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
			in_stream >> from_position;
			in_stream >> temp;
			index_iterator = band_map_index.find(temp);
			if ((index_iterator) == band_map_index.end())
			{
				std::cout << "unknown band label " << temp << " encountered\n";
				exit(1);
			}
			to_band_index = index_iterator->second;
			in_stream >> to_position;
			in_stream >> control_radius;
			in_stream >> stroke_width;
			in_stream >> link_color;
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
		friend istream& operator>>(istream& in_stream, besiel_link& in_link)
		{
			string one_line;
			in_stream >> one_line;
			in_link = besiel_link(one_line);
			return in_stream;
			
		}
		friend ostream& operator<<(ostream& in_stream, const besiel_link& in_link)
		{
			//<path d="M200,300 Q400,50 600,300 " fill = "none" stroke = "red" stroke - width = "5" / >
			SvgPoint from_point;
			SvgPoint to_point;
			SvgPoint control_point;
			from_point = SvgPoint(in_link.on_radius, in_link.from_angle);
			to_point = SvgPoint(in_link.on_radius, in_link.to_angle);
			float radius_diff = abs(in_link.to_angle - in_link.from_angle);
			if ( radius_diff< PI -EPS)
			{
				control_point = SvgPoint(in_link.control_radius, (in_link.from_angle + in_link.to_angle) / 2);
			}
			else
			{
				control_point = SvgPoint(in_link.control_radius, (in_link.from_angle + in_link.to_angle) / 2 - PI);
			}
			
			in_stream << "<path d=\" ";
			in_stream << "M " << from_point;
			in_stream << "Q " << control_point;
			in_stream << to_point<<"\" ";
			in_stream << "fill= \"none\"" << " ";
			in_stream << "stroke=\"" << in_link.link_color << "\" ";
			in_stream << "stroke-width=\"" << in_link.stroke_width << "\" ";
			in_stream << "opacity=\"" << in_link.opacity << "\" ";
			in_stream << "/>" << endl;
			return in_stream;
		}
		string add_to_path()
		{
			stringstream in_stream;
			SvgPoint to_point;
			SvgPoint control_point;
			to_point = SvgPoint(on_radius, to_angle);
			if (abs(to_angle - from_angle) < PI )
			{
				control_point = SvgPoint(control_radius, (from_angle + to_angle) / 2);
			}
			else
			{
				control_point = SvgPoint(control_radius, (from_angle + to_angle) / 2 - PI);
			}
			in_stream << "Q" << control_point;
			in_stream << to_point;
			return in_stream.str();
		}
		
	};

}