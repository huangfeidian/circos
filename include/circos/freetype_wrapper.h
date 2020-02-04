#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <unordered_map>
#include <vector>
#include <string_view>
#include <cstdint>

namespace spiritsaway::circos
{
    class freetype_wrapper
    {
	public:
		using point_grey_t = std::vector<std::pair<std::pair<int32_t, int32_t>, std::uint8_t>>;
    private:

		std::vector<unsigned char> invalid_font;
        const std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>& font_info;//所有字体相关文件的存储路径映射
        std::unordered_map<std::string_view, std::vector<unsigned char>> font_cache;//字体文件读入内存
        std::uint8_t scale = 1;
        bool rotate_first = true;
		FT_Library ft_library;
        
        const std::vector<unsigned char>& get_font_mem(std::string_view font_name);

		point_grey_t read_glyph(const FT_GlyphSlot& slot);
		point_grey_t get_text_bitmap(double rad, std::vector<std::uint32_t> text, int font_size, const std::vector<uint8_t>& font_file);
	public:
        freetype_wrapper(const  std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>& in_font_info);
		point_grey_t draw_text_along_line(std::pair<int,int> from, std::pair<int,int> to, std::vector<std::uint32_t> text, std::uint8_t font_size, std::string_view font_name);
	};
}