#include "setting/normalise.h"
#include <fstream>
#include <unordered_set>
#include <array>
using std::array;
using std::unordered_set;
using std::ifstream;
using std::ofstream;
namespace circos
{
	struct SvgGraph
	{
		const static unordered_set<string> head_set;
		vector<circle> all_circles;
		vector<band> all_bands;
		vector<fill_onband> all_fill_onbands;
		vector<tick_onband> all_tick_onbands;
		vector<value_onband> all_value_onbands;
		vector<label_onband> all_label_onbands;
		vector<besiel_link> all_links;
		vector<track> all_tracks;
		void read_settings(ifstream& setting_file);
		void read_links(const string& file_path);
		void read_circles(const string& file_path);
		void read_tracks(const string& file_path);
		void read_bands(const string& file_path);
		void read_tick_onbands(const string& file_path);
		void read_value_onbands(const string& file_path);
		void read_label_onbands(const string& file_path);
		void read_fill_onbands(const string& file_path);
	};
}