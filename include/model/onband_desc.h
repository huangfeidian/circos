#include <vector>
#include <string_view>
#include "../shapes/point.h"
#include "../shapes/color.h"
#include "../shapes/enum_types.h"

namespace circos::model
{
    struct fill_onband
    {
        std::string_view band_id;
        int on_band_begin;
        int on_band_end;
        Color fill_color;
    };

    struct text_onband:public fill_onband
    {
        std::string_view text;
        int font_size;
        std::string_view font_name;
        text_align_type align_type;
    };
    struct tick_onband: public fill_onband
    {
        int tick_gap;
        int width;
        int height;
    };
}