#include "circle.h"
#include "onband.h"
#include "track.h"
namespace circos
{
	void normalize(const vector<band>& all_bands, vector<BesielLink>& all_links)
	{
		for (auto& i : all_links)
		{
			band from_band = all_bands[i.from_band_index];
			band to_band = all_bands[i.to_band_index];
			i.from_angle = from_band.position_to_angle(i.from_position);
			i.to_angle = to_band.position_to_angle(i.to_position);

		}
	}
	void normalize(const vector<band>& all_bands, vector<Track>& all_tracks)
	{
		for (auto& i : all_tracks)
		{
			band from_band = all_bands[i.from_band_index];
			band to_band = all_bands[i.to_band_index];
			i.begin_from_angle = from_band.position_to_angle(i.begin_from_position);
			i.end_from_angle = from_band.position_to_angle(i.end_from_position);
			i.begin_to_angle = to_band.position_to_angle(i.begin_to_position);
			i.end_to_angle = to_band.position_to_angle(i.end_to_position);
		}
	}
	void normalise(const circle& on_circle, vector<band> bands)
	{
		int band_number = bands.size();
		int band_size = 0;

		for (auto i : bands)
		{
			band_size += i.end - i.begin;
		}
		float pixel_per_unit = (2 * PI*on_circle.inner_radius - band_number*on_circle.gap) / band_size;
		float angle_begin = 0;
		for (auto& i : bands)
		{
			i.angle_begin = angle_begin;
			i.angle_end = (i.end - i.begin)*pixel_per_unit + i.angle_begin;
			angle_begin = i.angle_end + (on_circle.gap) *pixel_per_unit;
		}
	}
	template<typename T> enable_if<is_base_of<onband, T>::value, void>
		normalize_onbands(const band& in_band, vector<T>& onbands)//这里只针对onband类型
		{
			float angle_offset;
			float end_angle;
			for (auto& i : onbands)
			{
				angle_offset = (i.begin - in_band.begin) *in_band.angle_per_unit;
				i.angle_begin = angle_offset + in_band.angle_begin;
				angle_offset = ((i.end - in_band.begin)) *in_band.angle_per_unit;
				i.angle_end = angle_offset + in_band.angle_begin;
			}
		}
}