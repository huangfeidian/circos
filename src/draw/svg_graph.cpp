#pragma once
#include <circos_description.h>
#include <fstream>
#include <description/circos_config.h>
namespace circos
{
	class SvgGraph
	{
		const string& output_file_name;
		ofstream output_stream;
		SvgGraph(const string& file_name)
			:output_file_name(file_name)
		{

		}
		void draw_circos(const CircosDescription& circos_description)
		{
			draw_head();
			draw_body(circos_description);
			draw_end();

		}
		void draw_head()
		{
			output_stream << R"(<?xml version="1.0" encoding="utf-8" standalone="no"?>)" << std::endl;
			output_stream << R"(<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">)" << std::endl;
			output_stream << "<svg width=\"" << background_radius * 2 << "px\" ";
			output_stream << "height=\"" << background_radius * 2 << "px\" ";
			output_stream << R"(version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">)" << std::endl;
			output_stream << "<rect x=\"0\" y=\"0\" width=\"" << 2 * background_radius << "\" height=\"" << 2 * background_radius << "\" ";
			output_stream << "fill=\"" << config.background_color << "\"/>" << std::endl;
		}
		void draw_band(const circle& on_circle, const vector<band>& all_bands)
		{

		}
		void draw_onbands(const circle& on_circle, const vector<fill_onband>& all_onbands)
		{

		}
		void draw_onbands(const circle& on_circle, const vector<label_onband>& all_onbands)
		{

		}
		void draw_onbands(const circle& on_circle, const vector<tick_onband>& all_onbands)
		{

		}
		void draw_onbands(const circle& on_circle, const vector<value_onband>& all_onbands)
		{

		}
		void draw_end()
		{
			output_stream << "</svg>" << std::endl;
		}
		void draw_body(const CircosDescription& circos_description)
		{
			for (auto i : circos_description.all_thing_to_draw)
			{
				output_stream << i.current_circle;//draw circle
				vector<band> all_bands_on_circle;
				for (auto j : i.all_bands)
				{
					all_bands_on_circle.push_back(j.current_band);
				}
				draw_band( i.current_circle, all_bands_on_circle);//draw bands
				for (auto j : i.all_bands)//draw onbands
				{
					draw_onbands( i.current_circle, j.all_fill_onbands);
					draw_onbands( i.current_circle, j.all_heatmap_onbands);
					draw_onbands( i.current_circle, j.all_histogram_onbands);
					draw_onbands( i.current_circle, j.all_label_onbands);
					draw_onbands( i.current_circle, j.all_tick_onbands);
					draw_onbands( i.current_circle, j.all_linechart_onbands);
				}
				for (auto j : i.all_links)//draw links
				{
					output_stream << j;
				}
				for (auto j : i.all_tracks)//draw tracks
				{
					output_stream << j;
				}
			}
		}
	};
}