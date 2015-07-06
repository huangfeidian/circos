#include "band.h"
namespace circos
{
	struct onband
	{
		color band_color;
		int begin;
		int end;
		float angle_begin;
		float angle_end;
		float opacity;
		int on_this_band;
		onband() :band_color(), begin(0), end(0), opacity(1)
		{

		}
	};
	template<typename T>
	void draw_onbands(ostream& output, const circle& on_circle, const T& onbands);
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
			auto band_position = band_map_index.find(band_id);
			if (band_position == band_map_index.end())
			{
				std::cout << "error while lookup band label " << band_id;
				exit(1);
			}
			on_this_band = band_position->second;
			int begin;
			input_buf >> begin;
			int end;
			input_buf >> end;
			string colorful;
			input_buf >> colorful;
			band_color = color(colorful);
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
	template<>
	void draw_onbands<fill_onband>(ostream& output, const circle& on_circle, const fill_onband& onbands)
	{
		//<path d="M300,200 h-150 a150,150 0 1,0 150,-150 z" fill = "red" stroke = "blue" stroke - width = "5" / >

		SvgPoint p_1, p_2, p_3, p_4;
		int band_begin, band_end;
		band_begin = 0;
		int sweep_flag = 1;
		int large_arc = 0;
		CircularArc arc_to_draw;

		//对于这个band 我们需要先向上，然后顺时针，然后向下，然后逆时针
		p_1 = SvgPoint(on_circle.inner_radius, onbands.angle_begin);
		p_2 = SvgPoint(on_circle.outer_radius, onbands.angle_begin);
		p_3 = SvgPoint(on_circle.outer_radius, onbands.angle_end);
		p_4 = SvgPoint(on_circle.inner_radius, onbands.angle_end);
		if (onbands.angle_end - onbands.angle_begin > PI / 2)//这个圆弧超过了半个圆，需要考虑large_arc的问题
		{
			large_arc = 1;
		}
		output << "<path d=\"";
		//output << " M " << p_1.x << "," << p_1.y;
		output << " M " << p_2;
		output << CircularArc(on_circle.outer_radius, onbands.angle_begin, onbands.angle_end, 1);
		output << "L " << p_4;
		output << CircularArc(on_circle.inner_radius, onbands.angle_end, onbands.angle_begin, 0);
		output << "z\" ";
		output << "fill=\"" << onbands.band_color << "\" ";
		output << "opacity=\"" << onbands.opacity << "\"";
		output << "/>" << endl;

	}
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
			auto band_position = band_map_index.find(band_id);
			if (band_position == band_map_index.end())
			{
				std::cout << "error while lookup band label " << band_id;
				exit(1);
			}
			on_this_band = band_position->second;
			input_buf >> begin;
			input_buf >> end;
			input_buf >> font_size;
			input_buf >> align;
			input_buf >> text;
			input_buf >> band_color;
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
	template<>
	void draw_onbands<label_onband>(ostream& output, const circle& on_circle, const label_onband& onbands)
	{
		//<path d="M300,200 h-150 a150,150 0 1,0 150,-150 z" fill = "red" stroke = "blue" stroke - width = "5" / >
		//<text font-family="Verdana" font-size="42.5" fill="blue" >
		//<textPath xlink : href = "#MyPath" startOffset = "80%">
		//We go up, then we go down, then up again
		//< / textPath>
		//< / text>
		SvgPoint p_1, p_2;
		int band_begin, band_end;
		band_begin = 0;
		int sweep_flag = 1;
		int large_arc = 0;

		switch (onbands.align)
		{
		case 0://径向
			int text_length = onbands.font_size*onbands.text.length() * 2;//预防多字节编码。。。
			int out_radius = on_circle.outer_radius + text_length;
			int angle = (onbands.angle_end + onbands.angle_end) / 2;
			SvgLine line_to_draw = SvgLine(SvgPoint(on_circle.outer_radius, angle), SvgPoint(out_radius, angle), color(), 0);
			int path_id = line_to_draw.convert_to_path();
			output << "<text font-family=\"monospace\" font-size=\"" << onbands.font_size << "\" ";
			output << "fill=\"" << onbands.band_color << "\" >\n";
			output << "textPath xlink:href=\"#textPath" << path_id << "\" startoffset=\"" << 0 << "%\" >\n";
			output << onbands.text << "\n";
			output << "</textPath>\n";
			output << "</text>\n";
		case 1://左对齐
			CircularArc arc_to_draw = CircularArc(on_circle.outer_radius, onbands.angle_begin, onbands.angle_end, 1);
			int path_id = arc_to_draw.convert_to_path();
			output << "<text font-family=\"monospace\" font-size=\"" << onbands.font_size << "\" ";
			output << "fill=\"" << onbands.band_color << "\" >\n";
			output << "textPath xlink:href=\"#textPath" << path_id << "\" startoffset=\"" << 0 << "%\" >\n";
			output << onbands.text << "\n";
			output << "</textPath>\n";
			output << "</text>\n";
			break;
		case 2://居中对齐
			CircularArc arc_to_draw = CircularArc(on_circle.outer_radius, onbands.angle_begin, onbands.angle_end, 1);
			int path_id = arc_to_draw.convert_to_path();
			output << "<text font-family=\"monospace\" font-size=\"" << onbands.font_size << "\" ";
			output << "fill=\"" << onbands.band_color << "\" >\n";
			output << "textPath xlink:href=\"#textPath" << path_id << "\" startoffset=\"" << 50 << "%\" >\n";
			output << onbands.text << "\n";
			output << "</textPath>\n";
			output << "</text>\n";
			break;
		default:
			std::cout << "unknown text draw type\n";
			break;
		}
	}
	struct tick_onband :public onband
	{
		int width;//tick 宽度
		int height;//tick 高度
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
			auto band_position = band_map_index.find(band_id);
			if (band_position == band_map_index.end())
			{
				std::cout << "error while lookup band label " << band_id;
				exit(1);
			}
			on_this_band = band_position->second;
			input_buf >> begin;
			input_buf >> end;
			input_buf >> width;
			input_buf >> height;
			string colorful;
			input_buf >> colorful;
			band_color = color(colorful);
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
	template<>
	void draw_onbands<tick_onband>(ostream& output, const circle& on_circle, const tick_onband& onbands)
	{
		SvgPoint from = SvgPoint(on_circle.outer_radius, onbands.angle_begin);
		SvgPoint to = SvgPoint(on_circle.outer_radius + onbands.height, onbands.angle_begin);
		SvgLine tick_to_draw = SvgLine(from, to, onbands.band_color, onbands.width);
		output << tick_to_draw;
	}
	struct value_onband :public onband
	{

		stat_type draw_type;
		float value;
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
			auto band_position = band_map_index.find(band_id);
			if (band_position == band_map_index.end())
			{
				std::cout << "error while lookup band label " << band_id;
				exit(1);
			}
			on_this_band = band_position->second;
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
	template<>
	void draw_onbands<value_onband>(ostream& output, const circle& on_circle, const value_onband& onbands)
	{
		switch (onbands.draw_type)
		{
		case stat_type::heatmap:
			draw_heatmap(output, on_circle, onbands);
			break;
		case stat_type::histogram:
			draw_histogram(output, on_circle, onbands);
			break;
		case stat_type::linechart:
			std::cout << "line chart can't be drawed by a single point\n";
			break;
		default:
			std::cout << "unknown stat type\n";
			break;

		}
	}
	void draw_linechart(ostream& output, const circle& on_circle, const vector<value_onband>& in_statistics)
	{
		SvgPoint p[2];
		int stroke_width = (on_circle.outer_radius - on_circle.inner_radius) / 20;//we make the width to 1/20 of circle width
		int on_radius = in_statistics[0].value*(on_circle.outer_radius - on_circle.inner_radius) + on_circle.inner_radius;
		p[0] = SvgPoint(on_radius, in_statistics[0].angle_begin);
		on_radius = in_statistics[1].value*(on_circle.outer_radius - on_circle.inner_radius) + on_circle.inner_radius;
		p[1] = SvgPoint(on_radius, in_statistics[1].angle_begin);
		SvgLine line_to_draw = SvgLine(p[0], p[1], in_statistics[0].band_color, stroke_width);
		output << line_to_draw;
		for (int i = 2; i < in_statistics.size(); i++)
		{
			on_radius = in_statistics[i].value*(on_circle.outer_radius - on_circle.inner_radius) + on_circle.inner_radius;
			p[i % 2] = SvgPoint(on_radius, in_statistics[i].angle_begin);
			line_to_draw = SvgLine(p[i % 2], p[(i - 1) % 2], in_statistics[i - 1].band_color, stroke_width);

			output << line_to_draw;
		}

	}
	void draw_histogram(ostream& output, const circle& on_circle, const value_onband& in_statistics)
	{
		SvgPoint p;
		int width;
		int height;
		int angle;
		height = (in_statistics.angle_end - in_statistics.angle_begin)*on_circle.inner_radius;
		width = in_statistics.value*(on_circle.outer_radius - on_circle.inner_radius);
		angle = (270 + (in_statistics.angle_begin * 360 / PI));
		p = SvgPoint(on_circle.inner_radius, in_statistics.angle_begin);
		output << "<rect x=\"" << p.x << "\" " << "y=\"" << p.y << "\" ";
		output << "width=\"" << width << "\" ";
		output << "height=\"" << height << "\" ";
		output << "fill=\"" << in_statistics.band_color << "\"";
		output << "transform=\"rotate(" << angle << " " << p.x << " " << p.y << ")\"";
		output << "/>\n";

	}
	void draw_heatmap(ostream& output, const circle& on_circle, const value_onband& in_statistics)
	{
		SvgPoint p;
		int width;
		int height;
		int angle;
		color current_color;

		height = (in_statistics.angle_end - in_statistics.angle_begin)*on_circle.inner_radius;
		width = in_statistics.value*(on_circle.outer_radius - on_circle.inner_radius);
		angle = (270 + (in_statistics.angle_begin * 360 / PI));
		p = SvgPoint(on_circle.inner_radius, in_statistics.angle_begin);
		current_color.set_gradient(on_circle.heat_color[0], on_circle.heat_color[0], in_statistics.value);
		output << "<rect x=\"" << p.x << "\" " << "y=\"" << p.y << "\" ";
		output << "width=\"" << width << "\" ";
		output << "height=\"" << height << "\" ";
		output << "fill=\"" << current_color << "\"";
		output << "transform=\"rotate(" << angle << " " << p.x << " " << p.y << ")\"";
		output << "/>\n";

	}
	template<typename T>
	void draw_onbands(ostream& output, const circle& on_circle, const vector<T>& onbands_vector)
	{
		for (const auto i : onbands_vector)
		{
			output << i;
		}
	}
	template<>
	void draw_onbands<value_onband>(ostream& output, const circle& on_circle, const vector<value_onband>& onbands_vector)
	{
		switch (onbands_vector[0].draw_type)
		{
		case stat_type::heatmap:
		case stat_type::histogram:
			for (const auto i : onbands_vector)
			{
				draw_heatmap(output, on_circle, i);
			}
			break;
		case stat_type::linechart:
			draw_linechart(output, on_circle, onbands_vector);
			break;
		default:
			break;
		}
	}
}