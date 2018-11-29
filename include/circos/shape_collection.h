#pragma once
#include <unordered_map>
#include <vector>
#include "./shapes/arc.h"
#include "./shapes/bezier.h"
#include "./shapes/circle.h"
#include "./shapes/line_text.h"
#include "./shapes/line.h"
#include "./shapes/rectangle.h"
#include "./shapes/ring.h"
#include "./shapes/track.h"

namespace circos
{
	class shape_collection
	{
	public:
		std::vector<Arc> arcs;
		std::vector<Bezier> beziers;
		std::vector<Circle> circles;
		std::vector<Line> lines;
		std::vector<Rectangle> rectangles;
		std::vector<Ring> rings;
		std::vector<Track> tracks;
		std::vector<LineText> line_texts;
		std::vector<std::vector<Point>> paths;
	};
}