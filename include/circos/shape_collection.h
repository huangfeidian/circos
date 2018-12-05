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
	template<typename img>
	void draw_collections(img& output_img, const shape_collection& in_shape_collection)
	{
		//for(const auto& one_circle: in_shape_collection.circles)
		//{
		//	output_img << one_circle;
		//}
		for(const auto& one_line : in_shape_collection.lines)
		{
			output_img << one_line;
		}
		for(const auto& one_rectangle: in_shape_collection.rectangles)
		{
			output_img << one_rectangle;
		}
		for(const auto& one_bezier: in_shape_collection.beziers)
		{
			output_img << one_bezier;
		}
		for(const auto& one_arc: in_shape_collection.arcs)
		{
			output_img << one_arc;
		}
		for(const auto& one_track: in_shape_collection.tracks)
		{
			output_img << one_track;
		}
		for(const auto& one_ring: in_shape_collection.rings)
		{
			output_img << one_ring;
		}
	}
}