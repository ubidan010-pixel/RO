#pragma once

#if defined(ITF_PASTA_STEAMBOT) && defined(ITF_SNOWDROP)
#include "MC_Core.h"
#endif

namespace ITF
{
	namespace sound
	{
		class Volume
		{
		public:
			static void init();
			static void term();

		public:
			// volumes are clamped between -100dB ... 0dB
			constexpr static f32 VOLUME_DB_MIN = -100.f;
			constexpr static f32 VOLUME_DB_MAX = 0.f;
			constexpr static f32 VOLUME_LINEAR_MIN = 0.f;
			constexpr static f32 VOLUME_LINEAR_MAX = 1.f;

			// classic
			static f32 getDecibelsFromLinear(f32 _linear);
			static f32 getLinearFromDecibels(f32 _decibels);

			// raki rules
			static f32 getDecibelsFromPsychoLinear(f32 _psychoLinear);
			static f32 getPsychoLinearFromDecibels(f32 _decibels);

		private:
			static Volume* ms_instance;
			constexpr static u32 VOLUME_NB_VALUES = 255;
			f32 m_decibelsFromPsychoLinearArray[VOLUME_NB_VALUES + 1];
			f32 m_psychoLinearFromDecibelsArray[VOLUME_NB_VALUES + 1];

			Volume() = default;
			void initTab();
			f32 getDecibelsFromPsychoLinearInternal(f32 _psychoLinear) const;
			f32 getPsychoLinearFromDecibelsInternal(f32 _decibels) const;
		};
	} // namespace sound
} // namespace ITF