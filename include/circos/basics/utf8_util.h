#pragma once
#include <string_view>
#include <vector>

namespace spiritsaway::string_util
{
    struct utf8_util
    {
       static std::vector<uint32_t> utf8_to_uint(std::string_view text) 
        {
            unsigned char u, v, w, x, y, z;
            std::vector<uint32_t> utf8_result;
            int num_chars = 0;
            uint32_t num_bytes = text.length();
            std::uint32_t iii = 0;
            while (iii < num_bytes)
            {
                uint32_t cur_utf8_char = 0;
                z = text[iii];
                if (z <= 127)
                {
                    cur_utf8_char = z;
                }
                else if (z >= 192 && z <= 223 && iii + 1 < num_bytes)
                {
                    iii++;
                    y = text[iii];
                    cur_utf8_char = (z - 192) * 64 + (y - 128);
                }
                else if (z >= 224 && z <= 239 && iii + 2 < num_bytes)
                {
                    iii++; y = text[iii];
                    iii++; x = text[iii];
                    cur_utf8_char = (z - 224) * 4096 + (y - 128) * 64 + (x - 128);
                }
                else if ((240 <= z) && (z <= 247) && iii + 3 < num_bytes)
                {
                    iii++; y = text[iii];
                    iii++; x = text[iii];
                    iii++; w = text[iii];
                    cur_utf8_char = (z - 240) * 262144 + (y - 128) * 4096 + (x - 128) * 64 + (w - 128);
                }
                else if ((248 <= z) && (z <= 251) && iii + 4 < num_bytes)
                {
                    iii++; y = text[iii];
                    iii++; x = text[iii];
                    iii++; w = text[iii];
                    iii++; v = text[iii];
                    cur_utf8_char = (z - 248) * 16777216 + (y - 128) * 262144 + (x - 128) * 4096 + (w - 128) * 64 + (v - 128);
                }
                else if ((252 == z) || (z == 253) && iii + 5 < num_bytes)
                {
                    iii++; y = text[iii];
                    iii++; x = text[iii];
                    iii++; w = text[iii];
                    iii++; v = text[iii];
                    iii++; u = text[iii];
                    cur_utf8_char = (z - 252) * 1073741824 + (y - 128) * 16777216 + (x - 128) * 262144 + (w - 128) * 4096 + (v - 128) * 64 + (u - 128);
                }
                else if (z >= 254)
                {
                    return utf8_result;
                }
                utf8_result.push_back(cur_utf8_char);
                iii++;
            }
            return utf8_result;
        }
	   static std::vector<std::string_view> u8_split(std::string_view text)
	   {
		   unsigned char u, v, w, x, y, z;
		   std::vector<std::string_view> utf8_result;
		   int num_chars = 0;
		   uint32_t num_bytes = text.length();
		   std::uint32_t iii = 0;
		   while (iii < num_bytes)
		   {
			   uint32_t cur_utf8_char = 0;
			   z = text[iii];
			   if (z <= 127)
			   {
				   cur_utf8_char = z;
				   utf8_result.push_back(std::string_view(&text[iii], 1));
			   }
			   else if (z >= 192 && z <= 223 && iii + 1 < num_bytes)
			   {
				   utf8_result.push_back(std::string_view(&text[iii], 2));

				   iii++;
				   y = text[iii];
				   cur_utf8_char = (z - 192) * 64 + (y - 128);
			   }
			   else if (z >= 224 && z <= 239 && iii + 2 < num_bytes)
			   {
				   utf8_result.push_back(std::string_view(&text[iii], 3));

				   iii++; y = text[iii];
				   iii++; x = text[iii];
				   cur_utf8_char = (z - 224) * 4096 + (y - 128) * 64 + (x - 128);
			   }
			   else if ((240 <= z) && (z <= 247) && iii + 3 < num_bytes)
			   {
				   utf8_result.push_back(std::string_view(&text[iii], 4));

				   iii++; y = text[iii];
				   iii++; x = text[iii];
				   iii++; w = text[iii];
				   cur_utf8_char = (z - 240) * 262144 + (y - 128) * 4096 + (x - 128) * 64 + (w - 128);
			   }
			   else if ((248 <= z) && (z <= 251) && iii + 4 < num_bytes)
			   {
				   utf8_result.push_back(std::string_view(&text[iii], 5));

				   iii++; y = text[iii];
				   iii++; x = text[iii];
				   iii++; w = text[iii];
				   iii++; v = text[iii];
				   cur_utf8_char = (z - 248) * 16777216 + (y - 128) * 262144 + (x - 128) * 4096 + (w - 128) * 64 + (v - 128);
			   }
			   else if ((252 == z) || (z == 253) && iii + 5 < num_bytes)
			   {
				   utf8_result.push_back(std::string_view(&text[iii], 6));

				   iii++; y = text[iii];
				   iii++; x = text[iii];
				   iii++; w = text[iii];
				   iii++; v = text[iii];
				   iii++; u = text[iii];
				   cur_utf8_char = (z - 252) * 1073741824 + (y - 128) * 16777216 + (x - 128) * 262144 + (w - 128) * 4096 + (v - 128) * 64 + (u - 128);
			   }
			   else if (z >= 254)
			   {
				   return utf8_result;
			   }
			   iii++;
		   }
		   return utf8_result;
	   }
	};
}