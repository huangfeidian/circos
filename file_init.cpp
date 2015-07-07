#include "file_init.h"
namespace circos
{
	const unordered_set<string> SvgGraph::head_set = {
		"circle", "link", "track", "band", "value_onband", "tick_onband", "label_onband", "fill_onband" };
	void SvgGraph::read_circles(const string& file_path)
	{
		ifstream circle_file(file_path);
		string single_line;
		int index = 0;
		while (circle_file >> single_line)
		{
			all_circles.emplace_back(single_line);
			circle_map_index.insert(std::make_pair(all_circles[index].circle_label, index));
		}
	}
	void SvgGraph::read_bands(const string& file_path)
	{
		ifstream circle_file(file_path);
		string single_line;
		int index = 0;
		while (circle_file >> single_line)
		{
			all_bands.emplace_back(single_line);
			band_map_index.insert(std::make_pair(all_bands[index].band_label, index));
		}
	}
	void SvgGraph::read_tick_onbands(const string& file_path)
	{
		ifstream circle_file(file_path);
		string single_line;
		while (circle_file >> single_line)
		{
			all_tick_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_value_onbands(const string& file_path)
	{
		ifstream circle_file(file_path);
		string single_line;
		while (circle_file >> single_line)
		{
			all_value_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_label_onbands(const string& file_path)
	{
		ifstream circle_file(file_path);
		string single_line;
		while (circle_file >> single_line)
		{
			all_label_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_fill_onbands(const string& file_path)
	{
		ifstream circle_file(file_path);
		string single_line;
		while (circle_file >> single_line)
		{
			all_fill_onbands.emplace_back(single_line);
		}
	}
	void SvgGraph::read_links(const string& file_path)
	{
		ifstream circle_file(file_path);
		string single_line;
		while (circle_file >> single_line)
		{
			all_links.emplace_back(single_line);
		}
	}
	void SvgGraph::read_tracks(const string& file_path)
	{
		ifstream circle_file(file_path);
		string single_line;
		while (circle_file >> single_line)
		{
			all_tracks.emplace_back(single_line);
		}
	}
	void SvgGraph::read_settings(ifstream& setting_file)
	{
		//Name(string) "file="filepath(string)
		string temp_line;
		while (setting_file >> temp_line)
		{
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
			if (head.compare("value_onband") == 0)
			{
				read_value_onbands(file_path);
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
};