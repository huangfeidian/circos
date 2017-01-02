Bezier(const string& input)
		{
			stringstream in_stream(input);
			//"Link" band_from(string) position_from(int) band_to(string) position_to(int) control_radius(int) stroke_width(int) color(color) optional[opacity(float)]
			string temp;
			in_stream >> temp;
			if (temp.compare("link") != 0)
			{
				std::cout << "unexpected beginning " << temp << "while parsing  link\n";
				exit(1);
			}
			in_stream >> temp;
			auto index_iterator = config.band_map_index.find(temp);
			if ((index_iterator) == config.band_map_index.end())
			{
				std::cout << "unknown band label " << temp << " encountered\n";
				exit(1);
			}
			from_band_index = index_iterator->second;
			in_stream >> from_position;
			in_stream >> temp;
			index_iterator = config.band_map_index.find(temp);
			if ((index_iterator) == config.band_map_index.end())
			{
				std::cout << "unknown band label " << temp << " encountered\n";
				exit(1);
			}
			to_band_index = index_iterator->second;
			in_stream >> to_position;
			in_stream >> control_radius;
			in_stream >> stroke_width;
			in_stream >> color;
			opacity = 1.0;
			string optional;
			in_stream >> optional;

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
				in_stream >> optional;
			}
		}