#include <iostream>
#include <vector>
#include <unordered_map>
#include <circos/basics/color.h>
#include <fstream>
#include <numeric>
#include <string>

using namespace std;
using namespace circos;
int main()
{
	ifstream pi_file("../data/pi100000.txt");
	string pi_str((istreambuf_iterator<char>(pi_file)), istreambuf_iterator<char>());
	vector<uint8_t> pi_digits;
    vector<circos::Color> strand_color(10, Color());
	strand_color[0] = Color(15, 66, 90);
	strand_color[1] = Color(50, 163, 231);
	strand_color[2] = Color(77, 119, 25);
	strand_color[3] = Color(70, 207, 110);
	strand_color[4] = Color(105, 4, 4);
	strand_color[5] = Color(201, 15, 25);
	strand_color[6] = Color(148, 82, 0);
	strand_color[7] = Color(207, 105, 3);
	strand_color[8] = Color(63, 30, 78);
	strand_color[9] = Color(140, 81, 195);


    vector<string> strand_color_names;
    for(int i =0;i< 10;i++)
    {
        strand_color_names.push_back("c"+ to_string(i));
    }
        ofstream color_file("../data/pi_color.csv");
    for(int i = 0;i< 10; i++)
    {
        color_file<<strand_color_names[i]<<","<<int(strand_color[i].r)<<","<<int(strand_color[i].g)<<","<<int(strand_color[i].b)<<endl;
    }

    pi_digits.reserve(pi_str.size());
	int i = 0;
	for (; i < pi_str.size() - 1; i++)
	{
		if (pi_str[i] == '3' && pi_str[i + 1] == '.')
		{
			break;
		}
	}
	i += 1;
	for (; i < pi_str.size(); i++)
	{
		if (pi_str[i] >= '0' && pi_str[i] <= '9')
		{
			pi_digits.push_back(pi_str[i] - '0');
		}

	}
    i = 0;
	vector<int> chunk_size(10, 0); // 记录每个块的大小
	chunk_size[3] = 1;
	vector<vector<pair<int, pair<int, int>>>> connections(10, vector<pair<int, pair<int, int>>>()); // 记录每个块到其他块的链接
    int pre_char = 3;
    int link_count = 0;
    ofstream link_file("../data/pi_links.csv");
    link_file<<"link_id, from_tile_id, from_pos_idx, to_tile_id, to_pos_idx, control_radius_percent, color, ref_color, opacity"<<endl;
    float control_radius_percent = 0.3;
	float opacity = 0.3;
	for (i = 0; i < pi_digits.size(); i++)
	{
		auto cur_char = pi_digits[i];
		chunk_size[cur_char] += 1;
		if (cur_char != pre_char)
		{   
            link_count += 1;
            link_file<<"l"+to_string(link_count) <<","<<"tile"+to_string(pre_char)<<","<<chunk_size[pre_char] -1 << ","<<"tile"+to_string(cur_char)<<","<<chunk_size[cur_char] -1 <<","<<control_radius_percent<<", ,"<<"c"+to_string(pre_char)<<","<<opacity<<endl;
		}
		pre_char = cur_char;
	}

    ofstream tile_file("../data/pi_tile.csv");
    tile_file<<"tile_id, circle_id, width, sequence, is_fill, opacity, color, ref_color"<<endl;
    for(int i = 0;i <10; i++)
    {
        tile_file<<"tile"+to_string(i)<<",c0,"<<chunk_size[i]<<","<<i<<","<<1<<","<<1<<", ,"<<"c"+to_string(i)<<endl;
    }

}