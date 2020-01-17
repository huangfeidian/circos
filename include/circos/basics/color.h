#pragma once

namespace spiritsaway::circos
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
		Color(const Color& c_a, const Color& c_b, double opacity)
		{
			r = static_cast<std::uint8_t>(c_a.r*(1 - opacity) + c_b.r*opacity);
			g = static_cast<std::uint8_t>(c_a.g*(1-opacity)+c_b.g*opacity);
			b = static_cast<std::uint8_t>(c_a.b*(1-opacity)+c_b.b*opacity);
		}

		void blend(const Color& other, float opacity)
		{
			r = static_cast<std::uint8_t>(other.r*opacity + r*(1 - opacity));
			g = static_cast<std::uint8_t>(other.g*opacity + g*(1 - opacity));
			b = static_cast<std::uint8_t>(other.b*opacity + b*(1 - opacity));
		}
		void set_gradient(const Color& begin_Color, const Color& end_Color, float percentage)
		{
			r = static_cast<std::uint8_t>(begin_Color.r + (end_Color.r - begin_Color.r)*percentage);
			g = static_cast<std::uint8_t>(begin_Color.g + (end_Color.g - begin_Color.g)*percentage);
			b = static_cast<std::uint8_t>(begin_Color.b + (end_Color.b - begin_Color.b)*percentage);
		}
	};
};