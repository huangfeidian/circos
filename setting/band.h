#include "circle.h"
#include "enum.h"
#include "point.h"
#include "arc.h"
#include <string>
#include <type_traits>
#include <sstream>
#include <streambuf>
#pragma once
using std::is_base_of;
using std::enable_if;
using std::endl;
using std::stoi;
using std::stof;
namespace circos
{
#define PI 3.14159
	struct band
	{
		color band_color;
		int begin;
		int end;
		float angle_begin;
		float angle_end;
		float opacity;
		int on_circle;
		string band_label;
		band() :band_color(), begin(0), end(0), opacity(1)
		{

		}
		band(const string& input)
		{
			stringstream input_buf(input);
			string head;
			input_buf >> head;
			if (head.compare("band") != 0)
			{
				std::cout << "error while parse " << input << " to band\n";
				std::cout << "the head is " << head << endl;
				exit(1);
			}
			string circle_id;
			input_buf >> circle_id;
			auto circle_position = circle_map_index.find(circle_id);
			if ( circle_position== circle_map_index.end())
			{
				std::cout << "error while lookup circle label " << circle_id;
				exit(1);
			}
			on_circle = circle_position->second;
			input_buf >> band_label;
			int begin;
			input_buf >> begin;
			int end;
			input_buf >> end;
			string colorful;
			input_buf >> colorful;
			band_color = color(colorful);
			string optional;
			input_buf >> optional;
			
			while( optional.length()>0)
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
					std::cout << "unknown optional value " << optional<<std::endl;
					exit(1);
				}
				input_buf >> optional;
			}
		}
		friend istream& operator>>(istream& in_stream, band& in_band)
		{
			string temp;
			in_stream >> temp;
			in_band = band(temp);
		}
		float position_to_angle(int position)
		{
			return angle_begin + ((position - begin)*1.0 / (end - begin))*(angle_end - angle_begin);
		}
		
	};
	void draw_band(ostream& output, const circle& on_circle, const vector<band>& bands)
	{
		//<path d="M300,200 h-150 a150,150 0 1,0 150,-150 z" fill = "red" stroke = "blue" stroke - width = "5" / >
		SvgPoint p_1, p_2, p_3, p_4;
		int band_begin, band_end;
		band_begin = 0;
		int sweep_flag = 1;
		int large_arc = 0;
		circular_arc arc_to_draw;
		for (auto i : bands)
		{
			//对于这个band 我们需要先向上，然后顺时针，然后向下，然后逆时针
			float angle_begin = i.angle_begin;
			float angle_end = i.angle_end;
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
			output << " M " << p_2;
			output << circular_arc(on_circle.outer_radius,angle_begin,angle_end, 1);
			output << "L " << p_4;
			output << circular_arc( on_circle.inner_radius, angle_end,angle_begin, 0);
			output << "z\" ";
			output << "fill=\"" << i.band_color << "\" ";
			output << "opacity=\"" << i.opacity << "\"";
			output << "/>" << endl;
		}
	}

}