#pragma once
#include <circos_description.h>
#include <fstream>

namespace circos
{
	class PngGraph
	{
		const string& output_file_name;
		ofstream output_stream;
		PngGraph(const string& file_name)
			:output_file_name(file_name)
		{

		}
		void draw_circos(const CircosDescription& circos_description);
		void draw_line(const )
	};
}