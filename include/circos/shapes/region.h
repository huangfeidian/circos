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
	};
}
