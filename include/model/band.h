#pragma once
#include "../color.h"

using namespace std;
namespace circos
{
#define PI 3.14159
	struct strand
	{
		Color strand_color;
		double begin;
		double end;
		double angle_begin;
		double angle_end;
		double opacity;

		strand() :strand_color(), begin(0), end(0), opacity(1)
		{

		}
		float position_to_angle(int position)
		{
			return angle_begin + ((position - begin)*1.0 / (end - begin))*(angle_end - angle_begin);
		}
		
	};
	

}