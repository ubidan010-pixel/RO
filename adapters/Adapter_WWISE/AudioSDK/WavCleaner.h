#ifndef __WAVCLEANER_H__
#define __WAVCLEANER_H__

#include "adapters/Adapter_WWISE/AudioSDK/types.h"
#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"

namespace	AudioSDK
{
	class	WavCleaner
	{


	public:
			WavCleaner();

			void		setLevelThreshold(f32 _dB, f32 _windowsSec);
			void		getLevelThreshold(f32 &_dB, f32 &_windowsSec);
			void		setFadeDuration(f32 _val);
			void		getFadeDuration(f32 &_val);
			void		setDefaultSettings();

			bool		run(const char *_outputWav, const char *_inputWav);

			inline void cleanOnlyIntro()	{ m_cleanIntro = true; m_cleanOutro = false; }
			inline void cleanOnlyOutro()	{ m_cleanIntro = false; m_cleanOutro = true; }
			inline void cleanAll()			{ m_cleanIntro = true; m_cleanOutro = true; }
			f32			getDeletedDurationAtTheBeginning() const;
			f32			getDeletedDurationAtTheEnd() const;
			f32			getDurationAfterCleaning() const;


	private:
			f32		m_threshold;
			f32		m_windowSecondsSize;
			f32		m_fadeSecondsDuration;
			bool	m_cleanIntro;
			bool	m_cleanOutro;
			u32		m_startSample;
			u32		m_endSample;
			u32		m_sampleRate;
			u32		m_sampleBeforeCleaning;

			bool	checkFormat(const AudioStructs::WaveFormatExtensible &);
	};
}
#endif // __WAVCLEANER_H__
