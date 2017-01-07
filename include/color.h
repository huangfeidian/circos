#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>
#pragma once
using std::ostream;
using std::istream;
using std::string;
using std::stoi;
using std::endl;
namespace circos
{

	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		Color(uint8_t in_r = 255, uint8_t in_g = 255, uint8_t in_b = 255) :r(in_r), g(in_g), b(in_b)
		{

		}
		Color(const Color& a)
		: r(a.r)
		, g(a.g)
		, b(a.b)
		{

		}
		Color& operator=(const Color& other)
		{
			r= other.r;
			g= other.g;
			b= other.b;
			return (*this);
		}
		Color(Color a, Color b, double opacity)
		{
			r = a.r*(1-opacity)+b.r*opacity;
			g = a.g*(1-opacity)+b.g*opacity;
			b = a.b*(1-opacity)+b.b*opacity;
		}

		Color(const string& Colorful)
		{
			if (Colorful.compare(0, 3, "rgb", 3) != 0)
			{
				std::cout << "Color prefix " << Colorful << " is not accepted\n";
				exit(1);
			}
			auto first_of_rgb = Colorful.find('(') + 1;
			auto delimit = Colorful.find(',', first_of_rgb);
			string Color_value = Colorful.substr(first_of_rgb, delimit - first_of_rgb);
			r = stoi(Color_value);
			first_of_rgb = delimit + 1;
			delimit = Colorful.find(',', first_of_rgb);
			Color_value = Colorful.substr(first_of_rgb, delimit - first_of_rgb);
			g = stoi(Color_value);
			first_of_rgb = delimit + 1;
			delimit = Colorful.find(',', first_of_rgb);
			Color_value = Colorful.substr(first_of_rgb, delimit - first_of_rgb);
			b = stoi(Color_value);
		}
		void blend(const Color& other, float opacity)
		{
			r = other.r*opacity + r*(1 - opacity);
			g = other.g*opacity + g*(1 - opacity);
			b = other.b*opacity + b*(1 - opacity);
		}
		void set_gradient(const Color& begin_Color, const Color& end_Color, float percentage)
		{
			r = begin_Color.r + (end_Color.r - begin_Color.r)*percentage;
			g = begin_Color.g + (end_Color.g - begin_Color.g)*percentage;
			b = begin_Color.b + (end_Color.b - begin_Color.b)*percentage;
		}
		friend ostream& operator<<(ostream& in_stream, const Color& in_Color)
		{
			in_stream << "rgb(" << int(in_Color.r) << "," << int(in_Color.g) << "," << int(in_Color.b) << ")";
			return in_stream;
		}
		friend istream& operator>>(istream& in_stream, Color& in_Color)
		{
			string Colorful;
			in_stream >> Colorful;
			if (Colorful.compare(0, 3, "rgb", 3) != 0)
			{
				std::cout << "Color prefix " << Colorful<< " is not accepted\n";
				exit(1);
			}
			auto first_of_rgb = Colorful.find('(') + 1;
			auto delimit = Colorful.find(',', first_of_rgb);
			string Color_value = Colorful.substr(first_of_rgb, delimit - first_of_rgb);
			in_Color.r = stoi(Color_value);
			first_of_rgb = delimit + 1;
			delimit = Colorful.find(',', first_of_rgb);
			Color_value = Colorful.substr(first_of_rgb, delimit - first_of_rgb);
			in_Color.g = stoi(Color_value);
			first_of_rgb = delimit + 1;
			delimit = Colorful.find(',', first_of_rgb);
			Color_value = Colorful.substr(first_of_rgb, delimit - first_of_rgb);
			in_Color.b = stoi(Color_value);
			return in_stream;
		}
		
	};
};