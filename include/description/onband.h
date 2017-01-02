#include "band.h"
#include "enum.h"
#pragma once
namespace circos
{
	struct onband
	{
		color band_color;
		double begin;
		double end;
		double angle_begin;
		double angle_end;
		double opacity;
		int band_idx;
		onband() :band_color(), begin(0), end(0), opacity(1)
		{

		}
	};
	
	struct fill_onband :public onband
	{
		fill_onband() :onband()
		{

		}
		fill_onband(const string& input)
		{
			stringstream input_buf(input);
			string head;
			input_buf >> head;
			if (head.compare("onband") != 0)
			{
				std::cout << "error while parse " << input << " to onband\n";
				std::cout << "the head is " << head << endl;
				exit(1);
			}
			string band_id;
			input_buf >> band_id;
			auto band_position = config.band_map_index.find(band_id);
			if (band_position == config.band_map_index.end())
			{
				std::cout << "error while lookup band label " << band_id;
				exit(1);
			}
			band_idx = band_position->second;
			input_buf >> begin;
			input_buf >> end;
			string colorful;
			input_buf >> colorful;
			band_color = color(colorful);
			string optional;
			opacity = 1.0;
			input_buf >> optional;

			while (optional.length()>0)
			{
				auto delimiter = optional.find('=');
				string option_label = optional.substr(0, delimiter);
				string option_value = optional.substr(delimiter + 1);
				if (option_label.compare("opacity") == 0)
				{
					opacity = stof(option_value);
				}
				else
				{
					std::cout << "unknown optional value " << optional << std::endl;
					exit(1);
				}
				input_buf >> optional;
			}
		}
		friend istream& operator>>(istream& input, fill_onband& in_onband)
		{
			string temp;
			input >> temp;
			in_onband = fill_onband(temp);
			return input;
		}
	};
	
	struct label_onband :public onband
	{
		int align;//径向为0 ,左对齐=1 居中为2
		int font_size;//字体大小
		string text;//文字
		label_onband() :onband(), align(1), font_size(30), text("0")
		{

		}
		label_onband(const string& input)
		{
			stringstream input_buf(input);
			string head;
			input_buf >> head;
			if (head.compare("onband") != 0)
			{
				std::cout << "error while parse " << input << " to onband\n";
				std::cout << "the head is " << head << endl;
				exit(1);
			}
			string band_id;
			input_buf >> band_id;
			auto band_position = config.band_map_index.find(band_id);
			if (band_position == config.band_map_index.end())
			{
				std::cout << "error while lookup band label " << band_id;
				exit(1);
			}
			band_idx = band_position->second;
			input_buf >> begin;
			input_buf >> end;
			input_buf >> font_size;
			input_buf >> align;
			input_buf >> text;
			input_buf >> band_color;
			opacity = 1.0;
			string optional;
			input_buf >> optional;

			while (optional.length() > 0)
			{
				auto delimiter = optional.find('=');
				string option_label = optional.substr(0, delimiter);
				string option_value = optional.substr(delimiter + 1);
				if (option_label.compare("opacity") == 0)
				{
					opacity = stof(option_value);
				}
				else
				{
					std::cout << "unknown optional value " << optional << std::endl;
					exit(1);
				}
				optional.clear();
				input_buf >> optional;
			}
		}
		friend istream& operator>>(istream& input, label_onband& in_onband)
		{
			string temp;
			input >> temp;
			in_onband = label_onband(temp);
			return input;
		}
	};

	
	struct tick_onband :public onband
	{
		double width;//tick 宽度
		double height;//tick 高度
		tick_onband() :onband(), width(0), height(0)
		{

		}
		tick_onband(const string& input)
		{
			stringstream input_buf(input);
			string head;
			input_buf >> head;
			if (head.compare("onband") != 0)
			{
				std::cout << "error while parse " << input << " to onband\n";
				std::cout << "the head is " << head << endl;
				exit(1);
			}
			string band_id;
			input_buf >> band_id;
			auto band_position = config.band_map_index.find(band_id);
			if (band_position == config.band_map_index.end())
			{
				std::cout << "error while lookup band label " << band_id;
				exit(1);
			}
			band_idx = band_position->second;
			input_buf >> begin;
			input_buf >> end;
			input_buf >> width;
			input_buf >> height;
			string colorful;
			input_buf >> colorful;
			band_color = color(colorful);
			opacity = 1.0;
			string optional;
			input_buf >> optional;

			while (optional.length()>0)
			{
				auto delimiter = optional.find('=');
				string option_label = optional.substr(0, delimiter);
				string option_value = optional.substr(delimiter + 1);
				if (option_label.compare("opacity") == 0)
				{
					opacity = stof(option_value);
				}
				else
				{
					std::cout << "unknown optional value " << optional << std::endl;
					exit(1);
				}
				optional.clear();
				input_buf >> optional;
			}
		}
		friend istream& operator>>(istream& input, tick_onband& in_onband)
		{
			string temp;
			input >> temp;
			in_onband = tick_onband(temp);
			return input;
		}
	};
	
	struct value_onband :public onband
	{

		stat_type draw_type;
		double value;
		value_onband() :onband(), draw_type(stat_type::histogram), value(0)
		{

		}
		value_onband(const string& input)
		{
			stringstream input_buf(input);
			string head;
			input_buf >> head;
			if (head.compare("onband") != 0)
			{
				std::cout << "error while parse " << input << " to onband\n";
				std::cout << "the head is " << head << endl;
				exit(1);
			}
			string band_id;
			input_buf >> band_id;
			auto band_position = config.band_map_index.find(band_id);
			if (band_position == config.band_map_index.end())
			{
				std::cout << "error while lookup band label " << band_id;
				exit(1);
			}
			band_idx = band_position->second;
			input_buf >> begin;
			input_buf >> end;
			input_buf >> value;
			input_buf >> band_color;
			string optional;
			input_buf >> optional;
			while (optional.length()>0)
			{
				auto delimiter = optional.find('=');
				string option_label = optional.substr(0, delimiter);
				string option_value = optional.substr(delimiter + 1);
				if (option_label.compare("opacity") == 0)
				{
					opacity = stof(option_value);
				}
				else
				{
					std::cout << "unknown optional value " << optional << std::endl;
					exit(1);
				}
				optional.clear();
				input_buf >> optional;
			}
		}
		friend istream& operator>>(istream& input, value_onband& in_onband)
		{
			string temp;
			input >> temp;
			in_onband = value_onband(temp);
			return input;
		}
	};
	
}