#include "colors.h"
#include "global_container.h"
#include <string>
#include <type_traits>
#include <sstream>
#include <streambuf>
#pragma once
using namespace std;
namespace circos
{
#define PI 3.14159
	struct band
	{
		color band_color;
		double begin;
		double end;
		double angle_begin;
		double angle_end;
		double opacity;
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
			input_buf >> begin;
			input_buf >> end;
			string colorful;
			input_buf >> colorful;
			band_color = color(colorful);
			opacity = 1.0;
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
				optional.clear();
				input_buf >> optional;
			}
		}
		band(const band& other_band)
		{
			band_color = other_band.band_color;
			begin = other_band.begin;
			end = other_band.end;
			angle_begin = other_band.angle_begin;
			angle_end = other_band.angle_end;
			opacity = other_band.opacity;
			on_circle = other_band.on_circle;
			band_label = other_band.band_label;
		}
		band& operator=(const band& other_band)
		{
			band_color = other_band.band_color;
			begin = other_band.begin;
			end = other_band.end;
			angle_begin = other_band.angle_begin;
			angle_end = other_band.angle_end;
			opacity = other_band.opacity;
			on_circle = other_band.on_circle;
			band_label = other_band.band_label;
			return *this;
		}
		friend istream& operator>>(istream& in_stream, band& in_band)
		{
			string temp;
			std::getline(in_stream,temp);
			in_band = band(temp);
		}
		float position_to_angle(int position)
		{
			return angle_begin + ((position - begin)*1.0 / (end - begin))*(angle_end - angle_begin);
		}
		
	};
	

}