#include <vector>
#include <string>
#include <map>
#include "colors.h"
#pragma once
using std::vector;
using std::string;
using std::map;
using std::pair;
namespace circos
{
	extern int path_index;
	extern double background_radius;
	extern map<string, int> circle_map_index;
	extern vector<pair<double, double>> circle_radius;
	extern map<string, int> band_map_index;
}