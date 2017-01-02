#include "colors.h"
#include <vector>
#include <sstream>
#include "../circos_config.h"
using std::vector;
using std::string;
using std::stringstream;
#pragma once
namespace circos
{
	struct circle
	{
		double inner_radius;
		double outer_radius;
		color circle_color;
		color heat_color[2];
		string circle_label;
		double opacity;
		double gap;
		vector<int> bands;
		circle() :inner_radius(0), outer_radius(0), circle_color(), opacity(1),  gap(20)
		{
			heat_color[0] = heat_color[1] = circle_color;
		}
		circle(const string& input)
		{
			// "circle" circle_label(string) inner_radius(int) outer_radius(int) gap(int) circle_color(color)
			stringstream input_buf(input);
			string head;
			input_buf >> head;
			if (head.compare("circle") != 0)
			{
				std::cout << "error while parse " << input << " to circle\n";
				std::cout << "the head is " << head << endl;
				exit(1);
			}
		
			input_buf >> circle_label;
			input_buf >> inner_radius;
			input_buf >> outer_radius;
			input_buf >> gap;
			input_buf >> circle_color;
			opacity = 1.0;
			string optional;
			input_buf >> optional;

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
					if (option_label.compare("heatcolor") == 0)
					{
						stringstream color_stream(option_value);
						color_stream >> heat_color[0];
						color_stream.get();
						color_stream >> heat_color[1];
					}
					else
					{
						std::cout << "unknown optional value " << optional << std::endl;
						exit(1);
					}
				}
				optional.clear();

				input_buf >> optional;
			}
		}
		friend istream& operator>>(istream& in_stream, circle& in_circle)
		{
			string temp;
			in_stream >> temp;
			in_circle = circle(temp);
			return in_stream;
		}
		friend ostream& operator<<(ostream& in_stream, const circle& in_circle)
		{
			//<circle cx = "50" cy = "50" r = "40" stroke = "black" stroke - width = "3" fill = "white" / >
			double cx, cy;
			cx = cy = circos::background_radius;
			double r;
			double stroke_breadth;
			r = (in_circle.inner_radius+in_circle.outer_radius)/2;
			stroke_breadth = (in_circle.outer_radius - in_circle.inner_radius);
			if (in_circle.circle_color.b >= 0)
			{
				in_stream << "<circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << r<<"\" ";
				in_stream << "stroke=\"" << in_circle.circle_color << "\" stroke-width=\"" << stroke_breadth<<"\" ";
				in_stream << "fill=\"none\" ";
				in_stream << "opacity=\"" << in_circle.opacity << "\"/>" << endl;
			}
			return in_stream;
		}
	};
};