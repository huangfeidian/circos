#pragma once
#include <vector>
#include <variant>
#include "circle.h"
#include "line.h"
#include "arc.h"

namespace spiritsaway::circos
{
	class Region
	{
	public:
		std::vector<std::variant<Line, Arc>> boundaries;
		Point inner_point;
		float opacity;
		Color color;
		void add_boundary(Line _in_line)
		{
			boundaries.push_back(_in_line);
		}
		void add_boundary(Arc _in_arc)
		{
			boundaries.push_back(_in_arc);
		}
		void set_inner_point(Point _in_point)
		{
			inner_point = _in_point;
		}
		static Region make_triangle(Point a, Point b, Point c, Color boundary_color)
		{
			Region result;
			result.add_boundary(Line(a, b, boundary_color));
			result.add_boundary(Line(b, c, boundary_color));
			result.add_boundary(Line(c, a, boundary_color));
			result.set_inner_point((a + b + c) / 3);
			return result;
		}
		static Region make_fan(std::uint16_t radius_inner, std::uint16_t radius_outer, fixed_angle begin_rad, fixed_angle end_rad, Point center, Color boundary_color)
		{
			Region result;
			auto temp_arc1 = Arc(radius_inner, begin_rad, (free_angle(end_rad) - begin_rad),center, false, boundary_color, false, 1.0f);
			auto temp_arc2 = Arc(radius_outer, begin_rad, (free_angle(end_rad) - begin_rad),center, true, boundary_color, false, 1.0f);
			auto line_1 = Line(Point::radius_point(radius_inner, end_rad, center), Point::radius_point(radius_outer, end_rad, center), boundary_color);
			auto line_2 = Line(Point::radius_point(radius_outer, begin_rad, center), Point::radius_point(radius_inner, begin_rad, center), boundary_color);
			result.add_boundary(temp_arc1);
			result.add_boundary(line_1);
			result.add_boundary(temp_arc2);
			result.add_boundary(line_2);
			result.set_inner_point(Point::radius_point((radius_inner + radius_outer) / 2, fixed_angle::middle(begin_rad, end_rad), center));
			return result;
		}
	};

}
