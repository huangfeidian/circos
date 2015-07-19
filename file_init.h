
#include <fstream>
#include <unordered_set>
#include <array>
#include "setting/onband.h"
#include "setting/track.h"
#include "setting/arc.h"
#include "setting/circle.h"
#include "setting/enum.h"
#pragma once
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
	struct things_of_circle
	{
		circle current_circle;
		vector<onbands_of_bands> all_bands;
		vector<besiel_link> all_links;
		vector<track> all_tracks;
	};

	struct SvgGraph
	{
		color background_color;
		const static unordered_set<string> head_set;
		vector<circle> all_circles;
		vector<things_of_circle> all_thing_to_draw;
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
		void draw_head(ostream& in_stream);
		void draw_end(ostream& in_stream);
		void draw_body(ostream& in_stream);
		friend ostream& operator<<(ostream& in_stream, const SvgGraph& in_graph);
	};
}