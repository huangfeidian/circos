#include "colors.h"
#include "stroke.h"
namespace circos
{
	struct arc_setting
	{
		color arc_color;
		int breadth;//arc�Ŀ��
		float beisier_center;//��arc�ı��������ĵ�
		float angel_begin;//arc��ƫ�ƽǶ�
		float angel_end;
		float opacity;
		arc_setting() :arc_color(), breadth(0), angel_begin(0), angel_end(0), opacity(1)
		{

		}
	};


}