#include "circle.h"
#include "onband.h"
#include "track.h"
#pragma once
namespace circos
{
	void normalise(const vector<band>& all_bands, vector<besiel_link>& all_links)
	{
		for (auto& i : all_links)
		{
			band from_band = all_bands[i.from_band_index];
			band to_band = all_bands[i.to_band_index];
			i.from_angle = from_band.position_to_angle(i.from_position);
			i.to_angle = to_band.position_to_angle(i.to_position);
			i.on_radius = circle_radius[from_band.on_circle].first;

		}
	}
	void normalise(const vector<band>& all_bands, vector<track>& all_tracks)
	{
		for (auto& i : all_tracks)
		{
			band from_band = all_bands[i.from_band_index];
			band to_band = all_bands[i.to_band_index];
			i.begin_from_angle = from_band.position_to_angle(i.begin_from_position);
			i.end_from_angle = from_band.position_to_angle(i.end_from_position);
			i.begin_to_angle = to_band.position_to_angle(i.begin_to_position);
			i.end_to_angle = to_band.position_to_angle(i.end_to_position);
			i.on_radius = circle_radius[from_band.on_circle].first;
		}
	}
	void normalise(const circle& on_circle, vector<band>& bands)
	{
		int band_number = bands.size();
		double band_size = 0;
		float gap_angle = on_circle.gap*1.0 / ( on_circle.inner_radius);
		for (auto i : bands)
		{
			band_size += i.end - i.begin;
		}
		float angle_per_unit = (2.0 / (2 * on_circle.inner_radius))*((2 * PI*on_circle.inner_radius - band_number*on_circle.gap) / band_size);
		float angle_begin = 0;
		for (auto& i : bands)
		{
			i.angle_begin = angle_begin;
			i.angle_end = (i.end - i.begin)*angle_per_unit + i.angle_begin;
			angle_begin = i.angle_end + gap_angle;
		}
	}
	template<typename T> 
	typename enable_if<is_base_of<onband, T>::value, void>::type
		normalise_onbands(const band& in_band, vector<T>& onbands)//这里只针对onband类型
		{
			float angle_offset;
			float end_angle;
			float angle_per_unit = (in_band.angle_end - in_band.angle_begin) / (in_band.end - in_band.begin);
			for (auto& i : onbands)
			{
				angle_offset = (i.begin - in_band.begin) *angle_per_unit;
				i.angle_begin = angle_offset + in_band.angle_begin;
				angle_offset = ((i.end - in_band.begin)) *angle_per_unit;
				i.angle_end = angle_offset + in_band.angle_begin;
			}
		}
}