#pragma once
#include <fstream>
#include <unordered_set>
#include <array>
#include "shapes/onband.h"
#include "shapes/track.h"
#include "shapes/arc.h"
#include "shapes/circle.h"
#include "shapes/enum.h"
#include "shapes/bezier_link.h"

using std::array;
using std::unordered_set;
using std::ifstream;
using std::ofstream;
namespace circos
{
	struct onbands_of_bands
	{
		int revert_index;
		band current_band;
		vector<fill_onband> all_fill_onbands;
		vector<tick_onband> all_tick_onbands;
		vector<value_onband> all_linechart_onbands;
		vector<value_onband> all_heatmap_onbands;
		vector<value_onband> all_histogram_onbands;
		vector<label_onband> all_label_onbands;
	};
	struct circle_description
	{
		circle current_circle;
		vector<onbands_of_bands> all_bands;
		vector<besiel_link> all_links;
		vector<track> all_tracks;
	};

	struct CircosDescription
	{
		const static unordered_set<string> head_set;
		vector<circle> all_circles;
		vector<circle_description> all_thing_to_draw;
		vector<band> all_bands;
		vector<fill_onband> all_fill_onbands;
		vector<tick_onband> all_tick_onbands;
		vector<label_onband> all_label_onbands;
		vector<value_onband> all_linechart_onbands;
		vector<value_onband> all_heatmap_onbands;
		vector<value_onband> all_histogram_onbands;
		vector<besiel_link> all_links;
		vector<track> all_tracks;
		void read_settings(ifstream& setting_file);
		void read_links(const string& file_path);
		void read_circles(const string& file_path);
		void read_tracks(const string& file_path);
		void read_bands(const string& file_path);
		void read_tick_onbands(const string& file_path);
		void read_linechart_onbands(const string& file_path);
		void read_histogram_onbands(const string& file_path);
		void read_heatmap_onbands(const string& file_path);
		void read_label_onbands(const string& file_path);
		void read_fill_onbands(const string& file_path);
		void all_normalise();
		void reorder();

	};
}