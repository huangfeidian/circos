#include "file_init.h"
using circos::SvgGraph;
int main()
{
	ifstream setting_file("pi_circos.txt");
	SvgGraph current_graph;
	current_graph.read_settings(setting_file);
	current_graph.reorder();
	current_graph.all_normalise();
	ofstream output_file("pi_circle.svg");
	current_graph.draw_head(output_file);
	current_graph.draw_body(output_file);
	current_graph.draw_end(output_file);
}