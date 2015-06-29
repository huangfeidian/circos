#include "colors.h"
#include "stroke.h"
namespace circos
{
	struct arc_setting
	{
		color arc_color;
		int breadth;//arc的宽度
		float beisier_center;//该arc的贝塞尔中心点
		float angel_begin;//arc的偏移角度
		float angel_end;
		float opacity;
		arc_setting() :arc_color(), breadth(0), angel_begin(0), angel_end(0), opacity(1)
		{

		}
	};


}