#include "colors.h"
#include "stroke.h"
namespace circos
{
	struct arc_setting
	{
		color arc_color;
		int breadth;//arc�Ŀ��
		float beisier_center;//��arc�ı��������ĵ�
		float begin_angle;//arc�Ŀ�ʼ�Ƕ�
		float end_angle;//arc�Ľ����Ƕ�
		float opacity;
		arc_setting() :arc_color(), breadth(0), begin_angle(0), end_angle(0), opacity(1)
		{

		}
		
	};


}