#include "precompiled_engine.h"
#include "engine/sound/common/SoundVolume.h"

namespace ITF
{
	namespace sound
	{
		Volume* Volume::ms_instance = nullptr;

		void Volume::init()
		{
			ms_instance = newAlloc(mId_AudioEngine, Volume);
			ms_instance->initTab();
		}

		void Volume::term()
		{
			delete ms_instance;
			ms_instance = nullptr;
		}

		void Volume::initTab()
		{
			auto getDecibelsFromPsychoLinearIntenal = [](f32 _psychoLinear)
			{ return -0.56f * powf(0.2f + _psychoLinear, -3.f) + -28.675925925925925925925925925926f + 29.f * _psychoLinear; };

			m_decibelsFromPsychoLinearArray[0] = VOLUME_DB_MIN;
			m_decibelsFromPsychoLinearArray[VOLUME_NB_VALUES] = VOLUME_DB_MAX;
			for (u32 index = 1; index < VOLUME_NB_VALUES; ++index)
			{
				const f32 psychoLinear = (f32)index / (f32)VOLUME_NB_VALUES;

				m_decibelsFromPsychoLinearArray[index] = getDecibelsFromPsychoLinearIntenal(psychoLinear);
			}

			m_psychoLinearFromDecibelsArray[0] = VOLUME_LINEAR_MAX;
			m_psychoLinearFromDecibelsArray[VOLUME_NB_VALUES] = VOLUME_LINEAR_MIN;
			for (u32 index = 1; index < VOLUME_NB_VALUES; ++index)
			{
				f32 psychoLinearAbove = VOLUME_LINEAR_MAX;
				f32 psychoLinearBelow = VOLUME_LINEAR_MIN;

				const f32 decibels = VOLUME_DB_MIN * (f32)index / (f32)VOLUME_NB_VALUES;

				while (psychoLinearAbove - psychoLinearBelow > 0.000001f)
				{
					const f32 medium = (psychoLinearBelow + psychoLinearAbove) / 2.f;

					const f32 decibelsForMedium = getDecibelsFromPsychoLinearIntenal(medium);

					if (decibelsForMedium > decibels)
						psychoLinearAbove = medium;
					else
						psychoLinearBelow = medium;
				}

				m_psychoLinearFromDecibelsArray[index] = (psychoLinearBelow + psychoLinearAbove) / 2.f;
			}
		}

		f32 Volume::getDecibelsFromLinear(f32 _linear)
		{
			if (_linear <= 0.00001f)
				return VOLUME_DB_MIN;

			if (_linear >= VOLUME_LINEAR_MAX)
				return VOLUME_DB_MAX;

			return 20.f * log10f(_linear);
		}

		f32 Volume::getLinearFromDecibels(f32 _decibels)
		{
			if (_decibels <= VOLUME_DB_MIN)
				return 0.f;

			if (_decibels >= VOLUME_DB_MAX)
				return VOLUME_LINEAR_MAX;

			return powf(10.f, _decibels / 20.f);
		}

		f32 Volume::getDecibelsFromPsychoLinearInternal(f32 _psychoLinear) const
		{
			if (_psychoLinear <= VOLUME_LINEAR_MIN)
				return VOLUME_DB_MIN;

			if (_psychoLinear >= VOLUME_LINEAR_MAX)
				return VOLUME_DB_MAX;

			const int index = (int)((_psychoLinear) * (f32)VOLUME_NB_VALUES);
			const f32 alpha = (_psychoLinear) * (f32)VOLUME_NB_VALUES - (f32)index;

			f32 decibels = (1.f - alpha) * m_decibelsFromPsychoLinearArray[index] + alpha * m_decibelsFromPsychoLinearArray[index + 1];

			return decibels;
		}

		f32 Volume::getPsychoLinearFromDecibelsInternal(f32 _decibels) const
		{
			if (_decibels <= VOLUME_DB_MIN)
				return VOLUME_LINEAR_MIN;

			if (_decibels >= VOLUME_DB_MAX)
				return VOLUME_LINEAR_MAX;

			const int index = (int)((_decibels / VOLUME_DB_MIN) * (f32)VOLUME_NB_VALUES);
			const f32 alpha = (_decibels / VOLUME_DB_MIN) * (f32)VOLUME_NB_VALUES - (f32)index;

			f32 psychoLinear = (1.f - alpha) * m_psychoLinearFromDecibelsArray[index] + alpha * m_psychoLinearFromDecibelsArray[index + 1];

			return psychoLinear;
		}

		f32 Volume::getDecibelsFromPsychoLinear(f32 _psychoLinear)
		{
			return ms_instance->getDecibelsFromPsychoLinearInternal(_psychoLinear);
		}

		f32 Volume::getPsychoLinearFromDecibels(f32 _decibels)
		{
			return ms_instance->getPsychoLinearFromDecibelsInternal(_decibels);
		}
	} // namespace sound
} // namespace ITF