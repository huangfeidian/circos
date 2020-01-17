#pragma once
#include <unordered_map>
#include <vector>
#include "./shapes/arc.h"
#include "./shapes/bezier.h"
#include "./shapes/circle.h"
#include "./shapes/line_text.h"
#include "./shapes/line.h"
#include "./shapes/rectangle.h"
#include "./shapes/tile.h"
#include "./shapes/ribbon.h"
#include "./shapes/annulus.h"
#include "./shapes/region.h"

namespace spiritsaway::circos
{
	class shape_collection
	{
	public:
		std::vector<Arc> arcs;
		std::vector<Bezier> beziers;
		std::vector<Circle> circles;
		std::vector<Line> lines;
		std::vector<Rectangle> rectangles;
		std::vector<Tile> tiles;
		std::vector<Ribbon> ribbons;
		std::vector<LineText> line_texts;
		std::vector<std::vector<Point>> paths;
		std::vector<Annulus> annuluses;
		std::vector<Region> regions;

	};
	template<typename img>
	void draw_collections(img& output_img, const shape_collection& in_shape_collection)
	{

		for(const auto& one_annulus: in_shape_collection.annuluses)
		{
			output_img << one_annulus;
		}
		for(const auto& one_circle: in_shape_collection.circles)
		{
			output_img << one_circle;
		}
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
		for(const auto& one_ribbon: in_shape_collection.ribbons)
		{
			output_img << one_ribbon;
		}
		for(const auto& one_tile: in_shape_collection.tiles)
		{
			output_img << one_tile;
		}
		for (const auto& one_line_text : in_shape_collection.line_texts)
		{
			output_img << one_line_text;
		}
		for (const auto& one_region: in_shape_collection.regions)
		{
			output_img << one_region;
		}
	}
}