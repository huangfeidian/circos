#include <vector>
#include "../shapes/color"

namespace circos::model
{
    struct point_link
    {
        std::string_view link_id;
        std::string_view from_band_id;
        int from_pos_idx;
        std::string_view to_band_id;
        int to_pos_idx;
        double control_radius_percent; //negative for direct link
        Color fill_color;
        double opacity;

    }
    struct range_link
    {
        std::string_view link_id;
        std::string_view from_band_id;
        int from_pos_begin_idx;
        int from_pos_end_idx;
        std::string_view to_band_id;
        int to_pos_begin_idx;
        int to_pos_end_idx;
        bool is_cross;
        Color fill_color;
        double opacity;
    }
}