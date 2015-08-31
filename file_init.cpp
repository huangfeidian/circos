#include "file_init.h"
#include "setting/normalise.h"
#include "draw/draw_onband.h"
#include "draw/draw_band.h"
#pragma once
namespace circos
{
	void strip_utf8header(istream& utf8_txt_stream)
	{
		char temp;
		utf8_txt_stream >> temp >> temp >> temp;
	}
	int path_index=0;
	double background_radius=5000;
	map<string, int> circle_map_index{};
	map<string, int> band_map_index{};
	vector<pair<double, double>> circle_radius{};
	const unordered_set<string> SvgGraph::head_set = {
		"circle", "link", "track", "band", "linechart_onband","histogram_onband","heatmap_onband", "tick_onband", "label_onband", "fill_onband" };
	void SvgGraph::read_circles(const string& file_path)
	{
		ifstream circle_file(file_path);
		strip_utf8header(circle_file);
		string single_line;
		int index = 0;
		while (std::getline(circle_file, single_line))
		{
			all_circles.emplace_back(single_line);
			circle_map_index.insert(std::make_pair(all_circles[index].circle_label, index));
			circle_radius.push_back(std::make_pair(all_circles[index].inner_radius, all_circles[index].outer_radius));
			index++;
		}
	}
	void SvgGraph::read_bands(const string& file_path)
	{
		ifstream band_file(file_path);
		strip_utf8header(band_file);
		string single_line;
		int index = 0;
		while (std::getline(band_file,single_line))
		{
			band temp_band(single_line);
			all_bands.emplace_back(temp_band);
			band_map_index.insert(std::make_pair(all_bands[index].band_label, index));
			index++;
		}
	}
	void SvgGraph::read_tick_onbands(const string& file_path)
	{
		ifstream tick_onband_file(file_path);
		strip_utf8header(tick_onband_file);
		string single_line;
		while (std::getline( tick_onband_file , single_line))
		{
			all_tick_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_heatmap_onbands(const string& file_path)
	{
		ifstream heatmap_onband_file(file_path);
		strip_utf8header(heatmap_onband_file);
		string single_line;
		while (std::getline(heatmap_onband_file,single_line))
		{
			all_heatmap_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_linechart_onbands(const string& file_path)
	{
		ifstream linechart_onband_file(file_path);
		strip_utf8header(linechart_onband_file);
		string single_line;
		while (std::getline(linechart_onband_file, single_line))
		{
			all_linechart_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_histogram_onbands(const string& file_path)
	{
		ifstream histogram_onband_file(file_path);
		strip_utf8header(histogram_onband_file);
		string single_line;
		while (std::getline(histogram_onband_file,single_line))
		{
			all_histogram_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_label_onbands(const string& file_path)
	{
		ifstream label_onband_file(file_path);
		strip_utf8header(label_onband_file);
		string single_line;
		while (std::getline(label_onband_file,single_line))
		{
			all_label_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_fill_onbands(const string& file_path)
	{
		ifstream file_onband_file(file_path);
		strip_utf8header(file_onband_file);
		string single_line;
		while (std::getline(file_onband_file,single_line))
		{
			all_fill_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_links(const string& file_path)
	{
		ifstream link_file(file_path);
		strip_utf8header(link_file);
		string single_line;
		while (std::getline(link_file ,single_line))
		{
			all_links.emplace_back(single_line);
		}
	}
	void SvgGraph::read_tracks(const string& file_path)
	{
		ifstream track_file(file_path);
		strip_utf8header(track_file);
		string single_line;
		while (std::getline(track_file , single_line))
		{
			all_tracks.emplace_back(single_line);
		}
	}
	void SvgGraph::read_settings(ifstream& setting_file)
	{
		strip_utf8header(setting_file);
		string temp_line;
		string head;
		string value;
		//background_color=color 
		//background_radius=int
		std::getline(setting_file, temp_line);
		head = temp_line.substr(0, temp_line.find("="));
		if (head.compare("background_color") != 0)
		{
			std::cout << "unexpected string " << head << " while waiting for color" << std::endl;
			exit(1);
		}
		background_color = color(temp_line.substr(temp_line.find("=") + 1));
		std::getline(setting_file, temp_line);
		head = temp_line.substr(0, temp_line.find("="));
		if (head.compare("background_radius") != 0)
		{
			std::cout << "unexpected string " << head << " while waiting for radius" << std::endl;
			exit(1);
		}
		background_radius = stoi(temp_line.substr(temp_line.find("=") + 1));
		while (std::getline(setting_file, temp_line))
		{
			
			//setting_file >> temp_line;
			std::cout<<temp_line<<std::endl;
			//Name(string) "file="filepath(string)
			auto delimiter = temp_line.find(" ");
			if (delimiter == temp_line.npos)
			{
				std::cout << "can't find blanksbace in " << temp_line << std::endl;
				exit(1);
			}
			string head = temp_line.substr(0, delimiter);
			delimiter = temp_line.find("=", delimiter);
			if (delimiter == temp_line.npos)
			{
				std::cout << "can't find = in " << temp_line << std::endl;
				exit(1);
			}
			string file_path = temp_line.substr(delimiter + 1);
			if (head_set.find(head) == head_set.end())
			{
				std::cout << "unknownd head " << head << std::endl;
				exit(1);
			}
			if (head.compare("circle") == 0)
			{
				read_circles(file_path);
			}
			if (head.compare("band") == 0)
			{
				read_bands(file_path);
			}
			if (head.compare("tick_onband") == 0)
			{
				read_tick_onbands(file_path);
			}
			if (head.compare("fill_onband") == 0)
			{
				read_fill_onbands(file_path);
			}
			if (head.compare("label_onband") == 0)
			{
				read_label_onbands(file_path);
			}
			if (head.compare("linechart_onband") == 0)
			{
				read_linechart_onbands(file_path);
			}
			if (head.compare("heatmap_onband") == 0)
			{
				read_heatmap_onbands(file_path);
			}
			if (head.compare("histogram_onband") == 0)
			{
				read_histogram_onbands(file_path);
			}
			if (head.compare("link") == 0)
			{
				read_links(file_path);
			}
			if (head.compare("track") == 0)
			{
				read_tracks(file_path);
			}
		}
	}
	void SvgGraph::reorder()
	{
		int circle_number = all_circles.size();
		int band_number = all_bands.size();
		vector<onbands_of_bands> onband_band_map(band_number);
		vector<things_of_circle> things_circle_map(circle_number);
		for (auto i : all_fill_onbands)
		{
			onband_band_map[i.on_this_band].all_fill_onbands.push_back(i);
		}
		for (auto i : all_tick_onbands)
		{
			onband_band_map[i.on_this_band].all_tick_onbands.push_back(i);
		}
		for (auto i:all_label_onbands)
		{
			onband_band_map[i.on_this_band].all_label_onbands.push_back(i);
		}
		for (auto i : all_heatmap_onbands)
		{
			i.draw_type = stat_type::heatmap;
			onband_band_map[i.on_this_band].all_heatmap_onbands.push_back(i);
		}
		for (auto i : all_histogram_onbands)
		{
			i.draw_type = stat_type::heatmap;
			onband_band_map[i.on_this_band].all_histogram_onbands.push_back(i);
		}
		for (auto i : all_linechart_onbands)
		{
			i.draw_type = stat_type::heatmap;
			onband_band_map[i.on_this_band].all_linechart_onbands.push_back(i);
		}
		for (int i = 0; i < band_number; i++)
		{
			onband_band_map[i].current_band = all_bands[i];
			onband_band_map[i].revert_index = i;
			things_circle_map[all_bands[i].on_circle].all_bands.push_back(onband_band_map[i]);
		}
		for (auto i : all_links)
		{
			things_circle_map[all_bands[i.from_band_index].on_circle].all_links.push_back(i);
		}
		for (auto i : all_tracks)
		{
			things_circle_map[all_bands[i.from_band_index].on_circle].all_tracks.push_back(i);
		}
		for (int i = 0; i < circle_number; i++)
		{
			things_circle_map[i].current_circle = all_circles[i];
		}
		all_thing_to_draw.swap(things_circle_map);
	}
	void SvgGraph::all_normalise()
	{
		for (auto& i : all_thing_to_draw)
		{
			vector<band> all_bands_on_circle;
			for (auto j : i.all_bands)
			{
				all_bands_on_circle.push_back(j.current_band);
			}
			normalise(i.current_circle, all_bands_on_circle);

			for (int j = 0; j < all_bands_on_circle.size(); j++)
			{
				all_bands[i.all_bands[j].revert_index] = all_bands_on_circle[j];
				i.all_bands[j].current_band = all_bands_on_circle[j];
				normalise_onbands(i.all_bands[j].current_band, i.all_bands[j].all_fill_onbands);
				normalise_onbands(i.all_bands[j].current_band, i.all_bands[j].all_heatmap_onbands);
				normalise_onbands(i.all_bands[j].current_band, i.all_bands[j].all_tick_onbands);
				normalise_onbands(i.all_bands[j].current_band, i.all_bands[j].all_label_onbands);
				normalise_onbands(i.all_bands[j].current_band, i.all_bands[j].all_linechart_onbands);
				normalise_onbands(i.all_bands[j].current_band, i.all_bands[j].all_histogram_onbands);
			}
			normalise(all_bands, i.all_links);
			normalise(all_bands, i.all_tracks);
		}
		
	}
	void SvgGraph::draw_head(ostream& in_stream)
	{
		in_stream << R"(<?xml version="1.0" encoding="utf-8" standalone="no"?>)" << std::endl;
		in_stream << R"(<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">)" << std::endl;
		in_stream << "<svg width=\"" << background_radius*2 << "px\" ";
		in_stream << "height=\"" << background_radius*2 << "px\" ";
		in_stream << R"(version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">)" << std::endl;
		in_stream << "<rect x=\"0\" y=\"0\" width=\"" << 2 * background_radius << "\" height=\"" << 2 * background_radius << "\" ";
		in_stream << "fill=\"" << background_color << "\"/>" << std::endl;
	}
	void SvgGraph::draw_end(ostream& in_stream)
	{
		in_stream << "</svg>" << std::endl;
	}
	void SvgGraph::draw_body(ostream& in_stream)
	{
		for (auto i : all_thing_to_draw)
		{
			in_stream << i.current_circle;//draw circle
			vector<band> all_bands_on_circle;
			for (auto j : i.all_bands)
			{
				all_bands_on_circle.push_back(j.current_band);
			}
			draw_band(in_stream, i.current_circle, all_bands_on_circle);//draw bands
			for (auto j : i.all_bands)//draw onbands
			{
				draw_onbands(in_stream, i.current_circle, j.all_fill_onbands);
				draw_onbands(in_stream, i.current_circle, j.all_heatmap_onbands);
				draw_onbands(in_stream, i.current_circle, j.all_histogram_onbands);
				draw_onbands(in_stream, i.current_circle, j.all_label_onbands);
				draw_onbands(in_stream, i.current_circle, j.all_tick_onbands);
				draw_onbands(in_stream, i.current_circle, j.all_linechart_onbands);
			}
			for (auto j : i.all_links)//draw links
			{
				in_stream << j;
			}
			for (auto j : i.all_tracks)//draw tracks
			{
				in_stream << j;
			}
		}
	}

};