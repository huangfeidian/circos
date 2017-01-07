#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include "../shapes/line.h"
#include "../shapes/circle.h"
#include "../shapes/arc.h"
#include "../shapes/bezier.h"
#include "../shapes/ring.h"
#include "../shapes/track.h"
#include "../shapes/rectangle.h"

using std::vector;
using std::endl;
#define PI 3.1415926
#define EPS 0.0001
namespace circos
{

	class SvgGraph
	{
		std::ostringstream output;
		const std::string file_name;
		int path_index = 0;
		const int background_radius;
		const Color background_color;
	public:
		SvgGraph(string in_file_name, int in_background_radius, Color in_background_color)
			: file_name(in_file_name)
			, background_radius(in_background_radius)
			,background_color(in_background_color)
		{
			output << R"(<?xml version="1.0" encoding="utf-8" standalone="no"?>)" << std::endl;
			output << R"(<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">)" << std::endl;
			output << "<svg width=\"" << background_radius * 2 << "px\" ";
			output << "height=\"" << background_radius * 2 << "px\" ";
			output << R"(version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">)" << std::endl;
			output << "<rect x=\"0\" y=\"0\" width=\"" << 2 * background_radius << "\" height=\"" << 2 * background_radius << "\" ";
			output << "fill=\"" << background_color << "\"/>" << std::endl;
		}
		SvgGraph& operator<<( const string& input_str)
		{
			output<<input_str;
			return *this;
		}
		SvgGraph& operator<<( int value)
		{
			output << value;
			return *this;
		}
		SvgGraph& operator<<( double value)
		{
			output<<value;
			return *this;
		}
		SvgGraph& operator<<(Color color)
		{
			output<< color;
			return *this;
		}
		SvgGraph& operator<<(const Point& point)
		{
			output<< point.x << "," << point.y << " ";
			return *this;
		}
		SvgGraph& operator<<(const Line& line)
		{
			auto& graph = *this;
			graph << "<line ";
			graph << "x1=\"" << line.from.x << "\" y1=\" "<< line.from.y << "\" ";
			graph << "x2=\"" << line.to.x << "\" y2=\"" << line.to.y << "\" ";
			graph << "stroke=\"" << line.color << "\" ";
			graph << "stroke-width=\"" << line.width << "\" ";
			graph << "opacity=\"" << line.opacity << "\" ";
			graph << "/>\n";
			return graph;
		}
		void add_to_path(const Arc& arc)
		{
			(*this) << " A " << arc.radius << "," << arc.radius << " ";
			(*this) << 0 << " " << arc.large_flag << "," << arc.sweep_flag << " ";
			(*this) << arc.to_point;
		}
		SvgGraph& operator<<( const Arc& arc)
		{
			auto& graph = *this;
			graph << "<path id=\"textPath" << path_index << "\" ";
			graph << "d= \" M " << arc.from_point.x << " " << arc.from_point.y << " ";
			graph.add_to_path(arc);
			if (arc.fill_flag)
			{
				graph << "L " << arc.center.x << " " << arc.center.y << " Z\" ";
				graph << "fill=\"" << arc.color << "\" ";
			}
			else
			{
				graph << "\" fill=\"none\" ";
			}
			graph << "stroke=\"" << arc.color << "\" " << "stroke-width=\"" << arc.stroke << "\" />";
			path_index++;
			return graph;
		}

		SvgGraph& operator<<(const Bezier& bezier)
		{
			auto& graph = *this;
			graph << "<path d=\" ";
			graph << "M " << bezier.begin_point;
			graph << "Q " << bezier.control_point;
			graph << bezier.end_point << "\" ";
			graph << "fill=\"none\"" << " ";
			graph << "stroke=\"" << bezier.color << "\" ";
			graph << "stroke-width=\"" << bezier.stroke_width << "\" ";
			graph << "opacity=\"" << bezier.opacity << "\" ";
			graph << "/>\n";
			return graph;
		}
		void add_to_path(const Bezier& bezier)
		{
			(*this)<<"Q "<<bezier.control_point<<bezier.end_point;
		}
		SvgGraph& operator<<(const Circle& circle)
		{
			auto& graph = *this;
			graph << "<circle cx=\"" << circle.center.x << "\" cy=\"" << circle.center.y << "\" r=\"" << circle.radius<<"\" ";
			graph << "stroke=\"" << circle.color << "\" stroke-width=\"" << 1 <<"\" ";
			if(circle.filled)
			{
				graph << "fill=\""<<circle.color<<"\" ";
			}
			else
			{
				graph << "fill=\"none\" ";
			}
			graph << "opacity=\"" << circle.opacity << "\"/>\n";
			return graph;
		}
		SvgGraph& operator<<(const Rectangle& rect)
		{
			auto& graph = *this;
			Line edge(rect.left, rect.right,rect.color);
			double len = edge.len();
			double theta = 180*std::asin(-(rect.left.y - rect.right.y) / len)/PI;
			graph<< "<rect x=\""<<rect.left.x<<"\" y=\""<<rect.left.y<<"\" ";
			graph<<"width=\""<<len<<"\" height = \""<<rect.height<<"\" ";
			if (rect.fill)
			{
				graph << "style=\"fill:" << rect.color << ";opacity:" << rect.opacity << "\" ";
			}
			
			graph<<"transform = \"rotate("<<theta<<" "<<rect.left.x<<" "<<rect.left.y<<")\"";
			graph<<"/>\n";
			return graph;
		}
		SvgGraph& operator<<(const Track& track)
		{
			auto& graph = *this;
			graph<<"<path d=\" M" << track.arc_1.from_point;
			graph.add_to_path(track.arc_1);
			graph.add_to_path(track.bezier_1);
			graph.add_to_path(track.arc_2);
			graph.add_to_path(track.bezier_2);
			graph<<"\" ";
			graph << "stroke=\"" << track.color << "\" stroke-width=\"" << track.arc_1.stroke << "\" ";
			if(track.fill)
			{
				graph<<"fill=\""<<track.color<<"\" ";
			}
			else
			{
				graph << "fill=\"none\" ";
			}
			graph << "opacity=\"" << track.opacity << "\"";
			graph<<"/>\n";
			return graph;
		}
		~SvgGraph()
		{
			output << "</svg>" << endl;
			std::ofstream file_output(file_name);
			file_output << output.str() << endl;
			file_output.close();
		}
	};
}