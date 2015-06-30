#include "colors.h"
#include "stroke.h"
namespace circos
{
	struct arc_setting
	{
		color arc_color;
		int breadth;//arc的宽度
		float beisier_center;//该arc的贝塞尔中心点
		float begin_angle;//arc的开始角度
		float end_angle;//arc的结束角度
		float opacity;
		arc_setting() :arc_color(), breadth(0), begin_angle(0), end_angle(0), opacity(1)
		{

		}
		
	};


}