#include <vector>
#include <string>
#include <map>
#include "shapes/colors.h"
#pragma once
using std::vector;
using std::string;
using std::map;
using std::pair;
namespace circos
{
    class CircosConfig
    {
	public:
        int path_index;
		double background_radius;
		map<string, int> circle_map_index;
		vector<pair<double, double>> circle_radius;
		map<string, int> band_map_index;
		color background_color;
	};
    static CircosConfig config;
}