#include <string>
#include <iostream>
#include <fstream>
#pragma once
using std::ostream;
using std::istream;
using std::string;
using std::stoi;
using std::endl;
namespace circos
{

	struct color
	{
		int r;
		int g;
		int b;
		color():r(255),g(255),b(255)
		{

		}
		color(int in_r, int in_g, int in_b) :r(in_r), g(in_g), b(in_b)
		{

		}
		color(const string& colorful)
		{
			if (colorful.compare(0, 3, "rgb", 3) != 0)
			{
				std::cout << "color prefix " << colorful << " is not accepted\n";
				exit(1);
			}
			auto first_of_rgb = colorful.find('(') + 1;
			auto delimit = colorful.find(',', first_of_rgb);
			string color_value = colorful.substr(first_of_rgb, delimit - first_of_rgb);
			r = stoi(color_value);
			first_of_rgb = delimit + 1;
			delimit = colorful.find(',', first_of_rgb);
			color_value = colorful.substr(first_of_rgb, delimit - first_of_rgb);
			g = stoi(color_value);
			first_of_rgb = delimit + 1;
			delimit = colorful.find(',', first_of_rgb);
			color_value = colorful.substr(first_of_rgb, delimit - first_of_rgb);
			b = stoi(color_value);
		}
		color(const color& from_color)
		{
			r = from_color.r;
			g = from_color.g;
			b = from_color.b;
		}
		color(color&& from_color)
		{
			r = from_color.r;
			g = from_color.g;
			b = from_color.b;
		}
		color& operator=(const color& from_color)
		{
			if (&from_color != this)
			{
				r = from_color.r;
				g = from_color.g;
				b = from_color.b;
			}
			return *this;
		}
		color& operator=(color&& from_color)
		{
			if (&from_color != this)
			{
				r = from_color.r;
				g = from_color.g;
				b = from_color.b;
			}
			return *this;
		}
		void set_gradient(const color& begin_color, const color& end_color, float percentage)
		{
			r = begin_color.r + (end_color.r - begin_color.r)*percentage;
			g = begin_color.g + (end_color.g - begin_color.g)*percentage;
			b = begin_color.b + (end_color.b - begin_color.b)*percentage;
		}
		friend ostream& operator<<(ostream& in_stream, const color& in_color)
		{
			in_stream << "rgb(" << in_color.r << "," << in_color.g << "," << in_color.b << ")";
			return in_stream;
		}
		friend istream& operator>>(istream& in_stream, color& in_color)
		{
			string colorful;
			in_stream >> colorful;
			if (colorful.compare(0, 3, "rgb", 3) != 0)
			{
				std::cout << "color prefix " << colorful<< " is not accepted\n";
				exit(1);
			}
			auto first_of_rgb = colorful.find('(') + 1;
			auto delimit = colorful.find(',', first_of_rgb);
			string color_value = colorful.substr(first_of_rgb, delimit - first_of_rgb);
			in_color.r = stoi(color_value);
			first_of_rgb = delimit + 1;
			delimit = colorful.find(',', first_of_rgb);
			color_value = colorful.substr(first_of_rgb, delimit - first_of_rgb);
			in_color.g = stoi(color_value);
			first_of_rgb = delimit + 1;
			delimit = colorful.find(',', first_of_rgb);
			color_value = colorful.substr(first_of_rgb, delimit - first_of_rgb);
			in_color.b = stoi(color_value);
			return in_stream;
		}
		
	};
	


};