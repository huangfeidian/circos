#include <circos/freetype_wrapper.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace spiritsaway::circos;
namespace
{
	struct pair_hash
	{
		std::size_t operator()(const pair<int, int>& data) const
		{
			return std::hash<int>()(data.first) ^ std::hash<int>()(data.second);
		}
	};

    vector<pair<pair<int32_t, int32_t>, uint8_t>> translate(const vector<pair<pair<int32_t, int32_t>, uint8_t>>& data, std::pair<int, int> from)
    {
        vector<pair<pair<int32_t, int32_t>, uint8_t>> result;
        result.reserve(data.size());


        for (auto one_data : data)
        {
            auto[diff_x, diff_y] = one_data.first;
            int new_x = diff_x + from.first;
            int new_y = diff_y + from.second;
            auto cur_pair = make_pair(new_x, new_y);
            result.emplace_back(cur_pair, one_data.second);
        }

        return result;
    }
    vector<pair<pair<int32_t, int32_t>, uint8_t>> rotate(const vector<pair<pair<int32_t, int32_t>, uint8_t>>& data, double rad)
    {
        vector<pair<pair<int32_t, int32_t>, uint8_t>> result;
        result.reserve(data.size());
        double cos_rad = cos(rad);
        double sin_rad = sin(rad);

        for (auto one_data : data)
        {
            auto[diff_x, diff_y] = one_data.first;
            int new_x = cos_rad * diff_x - sin_rad * diff_y;
            int new_y = cos_rad * diff_y + sin_rad * diff_x;
            auto cur_pair = make_pair(new_x, new_y);
            result.emplace_back(cur_pair, one_data.second);
        }

        return result;
    }
	vector<pair<pair<int32_t, int32_t>, uint8_t>> symmetric(const vector<pair<pair<int32_t, int32_t>, uint8_t>>& data, double rad)
	{
		vector<pair<pair<int32_t, int32_t>, uint8_t>> result;
		result.reserve(data.size());
		double cos_rad = cos(rad);
		double sin_rad = sin(rad);
		auto vec_1 = make_pair<int, int>(50 * cos_rad, 50 * sin_rad);
		for (auto one_data : data)
		{
			auto[diff_x, diff_y] = one_data.first;
			auto vec_2 = make_pair(diff_x, diff_y);
			float length = (vec_1.first*vec_2.first + vec_1.second*vec_2.second) / (1.0f*(vec_1.first*vec_1.first + vec_1.second*vec_1.second));
			std::pair<int, int> remain = static_cast<std::pair<int, int>>(std::make_pair(vec_2.first - length * vec_1.first, vec_2.second - length * vec_1.second));
			std::pair<int, int> vec_3 = std::make_pair(vec_2.first - 2 * remain.first, vec_2.second - 2 * remain.second);
			result.emplace_back(vec_3, one_data.second);
		}

		return result;
	}
    vector<pair<pair<int32_t, int32_t>, uint8_t>> down_sample(const vector<pair<pair<int32_t, int32_t>, uint8_t>>& data, std::uint8_t ratio)
    {
		if (ratio == 1)
		{
			return data;
		}
        unordered_map<pair<int, int>, uint16_t, pair_hash> temp_result;
		std::vector<std::tuple<int, int, int>> offset = {
			{0, 0, 8},
		{0,1, 1},
		{0, -1, 1},
		{1, 0, 1},
		{1, -1, 1},
		{1, 1, 1},
		 {-1, 0, 1},
		 {-1, 1, 1},
		 {-1, -1, 1},
		};
        for (auto one_data : data)
        {
			auto[cur_x, cur_y] = one_data.first;
			for (auto[diff_x, diff_y, weight] : offset)
			{
				
				auto new_x = (cur_x + diff_x) / ratio;
				auto new_y = (cur_y + diff_y) / ratio;
				if (new_x * ratio == (cur_x + diff_x) && new_y * ratio == (cur_y + diff_y))
				{
					auto cur_pair = make_pair(new_x, new_y);
					temp_result[cur_pair] = std::max(static_cast<std::uint16_t>(one_data.second), temp_result[cur_pair]);
					//temp_result[cur_pair] += one_data.second * one_data.second * weight;
				}
			}
			
        }
		int total_weight = 0;
		for (auto[diff_x, diff_y, weight] : offset)
		{
			total_weight += weight;
		}

   //     for (auto& one_data : temp_result)
   //     {
			//auto pre_value = one_data.second / total_weight;
   //         one_data.second = sqrt(pre_value);
   //     }
        vector<pair<pair<int32_t, int32_t>, uint8_t>> result;
        result.insert(result.end(), temp_result.begin(), temp_result.end());
        return result;
    }
}
const vector<unsigned char>& freetype_wrapper::get_font_mem(std::string_view font_name)
{
	auto font_iter = font_info.find(font_name);
	if (font_iter == font_info.end())
	{
		cerr << "invalid font " << font_name << endl;
		return invalid_font;
	}
	auto iter = font_cache.find(font_name);
	if (iter != font_cache.end())
	{
		return iter->second;
	}
	font_cache[font_name] = vector<unsigned char>();
	auto& result = font_cache[font_name];
	ifstream font_file(string(font_iter->second.first), ios::binary);
	result.insert(result.end(), std::istreambuf_iterator<char>(font_file), std::istreambuf_iterator<char>());
	return result;
}
freetype_wrapper::point_grey_t freetype_wrapper::read_glyph(const FT_GlyphSlot& slot)
{
	const FT_Bitmap* bitmap = &slot->bitmap;
	int left = slot->bitmap_left;
	int top = slot->bitmap_top;
	float temp;
	point_grey_t grey_points;
	for (uint32_t i = 1; i < bitmap->rows; i++)
	{
		for (uint32_t j = 1; j < bitmap->width; j++)
		{
			temp = static_cast<uint8_t>(bitmap->buffer[(i - 1)*bitmap->width + (j - 1)]);
			if (temp)
			{
				grey_points.push_back(make_pair(make_pair(left + j, top - i), temp));
			}
		}
	}
	return grey_points;
}
freetype_wrapper::point_grey_t freetype_wrapper::get_text_bitmap(double rad, std::vector<std::uint32_t> text, int font_size, const std::vector<uint8_t>& font_file)
{
	FT_Face face;
	point_grey_t result;
	FT_UInt glyph_index;
	FT_Error error;
	FT_Matrix matrix;
	FT_Vector pen;

	FT_Bool use_kerning;
	FT_UInt previous = 0;

	double cos_rad = cos(rad);
	double sin_rad = sin(rad);
	matrix.xx = static_cast<FT_Fixed>(cos_rad * 65536);
	matrix.xy = static_cast<FT_Fixed>(-sin_rad * 65536);
	matrix.yx = static_cast<FT_Fixed>(sin_rad * 65536);
	matrix.yy = static_cast<FT_Fixed>(cos_rad * 65536);
	pen.x = 0;
	pen.y = 0;
	error = FT_New_Memory_Face(ft_library, &font_file[0], font_file.size(), 0, &face);

	error = FT_Set_Pixel_Sizes(face, font_size, 0);

	FT_GlyphSlot slot = face->glyph;
	use_kerning = FT_HAS_KERNING(face);
	for (uint32_t i : text)
	{
		glyph_index = FT_Get_Char_Index(face, i);
		if (use_kerning&& previous&&glyph_index)
		{
			FT_Vector delta;
			FT_Get_Kerning(face, previous, glyph_index, ft_kerning_default, &delta);
			pen.x += static_cast<int>(delta.x*cos_rad);
			pen.y += static_cast<int>(delta.y*sin_rad);
		}
		FT_Set_Transform(face, &matrix, &pen);
		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

		error = FT_Render_Glyph(face->glyph, FT_Render_Mode::FT_RENDER_MODE_NORMAL);

		auto cur_glyph = read_glyph(face->glyph);
		result.insert(result.end(), cur_glyph.begin(), cur_glyph.end());
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
		previous = glyph_index;
	}
	FT_Done_Face(face);
	return result;
}
freetype_wrapper::freetype_wrapper(const  std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>& in_font_info)
	:font_info(in_font_info)
{
	auto error = FT_Init_FreeType(&ft_library);
	if (error)
	{
		std::cerr << "cant init ft_library" << endl;
	}
}
freetype_wrapper::point_grey_t freetype_wrapper::draw_text_along_line(std::pair<int, int> from, std::pair<int, int> to, std::vector<std::uint32_t> text, std::uint8_t font_size, std::string_view font_name)
{
	double rad = atan2(to.second - from.second, to.first - from.first);
	auto& font_file = get_font_mem(font_name);
	if (rotate_first)
	{
		auto temp_result = symmetric(get_text_bitmap(rad, text, font_size * scale, font_file), rad);
		return translate(down_sample(temp_result, scale), from);
	}
	else
	{
		auto temp_result = get_text_bitmap(0, text, font_size * scale, font_file);
		return translate(down_sample(symmetric(rotate(temp_result, rad), rad), scale), from);
	}
}

