#include "circle_desc.h"
#include "link_desc.h"
#include "onband_desc.h"
#include <unordered_map>
#include <vector>

namespace circos::model{
	class data_desc
	{
	public:
		// 这些是原始的输入数据
		model_config config;
		std::unordered_map<std::string_view, circle_desc> circles;
		std::unordered_map<std::string_view, band_desc> bands;
		std::unordered_map<std::string_view, circle_tick> circle_ticks;
		std::unordered_map<std::string_view, point_link> point_links;
		std::unordered_map<std::string_view, range_link> range_links;
	};
	
}