#include <iostream>
#include <circos/drawer/svg_drawer.h>

namespace circos
{
	SvgGraph::SvgGraph(const std::unordered_map<std::string, std::pair<std::string, std::string>>& in_font_info,
		string in_file_name, int in_background_radius, Color in_background_color)
		: font_info(in_font_info)
		, file_name(in_file_name)
		, background_radius(in_background_radius)
		, background_color(in_background_color)
	{
		output << R"(<?xml version="1.0" encoding="utf-8" standalone="no"?>)" << std::endl;
		output << R"(<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">)" << std::endl;
		output << "<svg width=\"" << background_radius * 2 << "px\" ";
		output << "height=\"" << background_radius * 2 << "px\" ";
		output << R"(version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">)" << std::endl;
		output << "<rect x=\"0\" y=\"0\" width=\"" << 2 * background_radius << "\" height=\"" << 2 * background_radius << "\" ";
		*this << "fill=\"" << background_color << "\"/>";
		output << std::endl;
	}

	SvgGraph& SvgGraph::operator<<(const string& input_str)
	{
		output << input_str;
		return *this;
	}
	SvgGraph& SvgGraph::operator<<(int value)
	{
		output << value;
		return *this;
	}
	SvgGraph& SvgGraph::operator<<(double value)
	{
		output << value;
		return *this;
	}
	SvgGraph& SvgGraph::operator<<(Color color)
	{
		output << "rgb("<<int(color.r)<<","<<int(color.g)<<","<<int(color.b)<<")";
		return *this;
	}
	SvgGraph& SvgGraph::operator<<(const Point& point)
	{
		output << point.x << "," << point.y << " ";
		return *this;
	}
	void SvgGraph::add_to_path(const Line& line)
	{
		(*this) << " L" << line.to << " ";
	}
	SvgGraph& SvgGraph::operator<<(const Line& line)
	{
		auto& graph = *this;
		graph << "<line ";
		graph << "x1=\"" << line.from.x << "\" y1=\" " << line.from.y << "\" ";
		graph << "x2=\"" << line.to.x << "\" y2=\"" << line.to.y << "\" ";
		graph << "stroke=\"" << line.color << "\" ";
		graph << "stroke-width=\"" << line.width << "\" ";
		graph << "opacity=\"" << line.opacity << "\" ";
		graph << "/>\n";
		return graph;
	}
	const std::string& SvgGraph::get_font_name(const std::string& input_name)const
	{
		auto iter = font_info.find(input_name);
		if (iter == font_info.end())
		{
			std::cerr << "unknown font " << input_name << std::endl;
			exit(1);
		}
		return iter->second.second;
	}

	void SvgGraph::add_to_path(const Arc& arc)
	{
		(*this) << " A " << arc.radius << "," << arc.radius << " ";
		(*this) << 0 << " " << arc.large_flag << "," << arc.sweep_flag << " ";
		(*this) << arc.to_point;
	}
	SvgGraph& SvgGraph::operator<<( const Arc& arc)
	{
		auto& graph = *this;
		graph << "<path id=\"textPath" << path_index << "\" ";
		graph << "d= \" M " << arc.from_point<<" ";
		graph.add_to_path(arc);
		if (arc.fill_flag)
		{
			graph << "L " << arc.center<< " Z\" ";
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

	SvgGraph& SvgGraph::operator<<(const Bezier& bezier)
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
	void SvgGraph::add_to_path(const Bezier& bezier)
	{
		(*this)<<"Q "<<bezier.control_point<<bezier.end_point;
	}
	SvgGraph& SvgGraph::operator<<(const Circle& circle)
	{
		auto& graph = *this;
		graph << "<circle cx=\"" << circle.center.x << "\" cy=\"" << circle.center.y << "\" r=\"" << circle.radius<<"\" ";
		graph << "stroke=\"" << circle.color << "\" stroke-width=\"" << circle.stroke_width <<"\" ";
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
	SvgGraph& SvgGraph::operator<<(const Rectangle& rect)
	{
		auto& graph = *this;
		Line edge(rect.left, rect.right,rect.color);
		double len = edge.len();
		double theta = 180*std::asin(-(rect.left.y - rect.right.y) / len)/pi();
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
	SvgGraph& SvgGraph::operator<<(const Tile& tile)
	{
		auto& graph = *this;
		Arc arc_1(tile.inner_radius, tile.begin_angle, tile.end_angle, tile.center, tile.color);
		Arc arc_2(tile.outer_radius, tile.end_angle, tile.begin_angle, tile.center, tile.color);
		Line line_1(Point::radius_point(tile.inner_radius, tile.end_angle,tile.center), Point::radius_point(tile.outer_radius, tile.end_angle,tile.center), tile.color);
		Line line_2(Point::radius_point(tile.outer_radius, tile.begin_angle, tile.center), Point::radius_point(tile.inner_radius, tile.begin_angle, tile.center), tile.color);
		graph << "<path d=\" M" << arc_1.from_point;
		graph.add_to_path(arc_1);
		graph.add_to_path(line_1);
		graph.add_to_path(arc_2);
		graph.add_to_path(line_2);
		graph << "\" ";
		graph << "stroke=\"" << tile.color << "\" stroke-width=\"" << tile.stroke << "\" ";
		if (tile.fill)
		{
			graph << "fill=\"" << tile.color << "\"";
		}
		else
		{
			graph << "fill=\"none\" ";
		}
		graph << " opacity=\"" << tile.opacity << "\"";
		graph << "/>\n";
		return graph;
	}
	SvgGraph& SvgGraph::operator<<(const Ribbon& ribbon)
	{
		auto& graph = *this;
		graph<<"<path d=\" M" << ribbon.arc_1.from_point;
		graph.add_to_path(ribbon.arc_1);
		graph.add_to_path(ribbon.bezier_1);
		graph.add_to_path(ribbon.arc_2);
		graph.add_to_path(ribbon.bezier_2);
		graph<<"\" ";
		graph << "stroke=\"" << ribbon.color << "\" stroke-width=\"" << ribbon.arc_1.stroke << "\" ";
		if(ribbon.fill)
		{
			graph<<"fill=\""<<ribbon.color<<"\" ";
		}
		else
		{
			graph << "fill=\"none\" ";
		}
		graph << "opacity=\"" << ribbon.opacity << "\"";
		graph<<"/>\n";
		return graph;
	}
	SvgGraph& SvgGraph::operator<<(const LineText& line_text)
	{
		const Line& base_line = line_text.on_line;
		auto& graph = *this;
		double angle = atan2(base_line.to.y - base_line.from.y, base_line.to.x - base_line.from.x)*180/pi();
		graph << "<text x=\"" << base_line.from.x << "\" y=\"" << base_line.from.y << "\" ";
		//graph << "font-family=\"" << line_text.font_name << "\" " << "font-size=\"" << line_text.font_size << "\" ";
		graph << "font-family=\"" << get_font_name(line_text.font_name) << "\" " << "font-size=\"" << line_text.font_size << "\" ";
		graph << "fill=\"" << line_text.color << "\" ";
		graph << "opacity=\"" << line_text.opacity << "\" ";
		graph << "transform=\"rotate(" << angle << " " << base_line.from.x << " " << base_line.from.y << ")\"";
		graph <<">\n" << line_text.utf8_text << "\n</text>\n";
		return graph;
	}
	SvgGraph& SvgGraph::operator<<(const Annulus& annulus)
	{
		if(!annulus.filled)
		{
			// 简化为两个空心圆
			Circle circle_1(annulus.inner_radius, annulus.center, annulus.color, annulus.opacity, false);
			Circle circle_2(annulus.outer_radius, annulus.center, annulus.color, annulus.opacity, false);
			*this<< circle_1<< circle_2;
		}
		else
		{
			// 简化为一个空心圆
			Circle circle_1((annulus.inner_radius + annulus.outer_radius) / 2, annulus.center, annulus.color, annulus.opacity, false, (annulus.outer_radius - annulus.inner_radius));
			*this<< circle_1;
		}
		return *this;
	}
	SvgGraph::~SvgGraph()
	{
		output << "</svg>" << endl;
		std::ofstream file_output(file_name);
		file_output << output.str() << endl;
		file_output.close();
	}
}