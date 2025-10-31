#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/AudioSDK/VuMeter.h"
#include <math.h>
#include "core/math/MathTools.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"

//$4
// ***********************************************************************************************************************
// ***********************************************************************************************************************
//

namespace	AudioSDK
{
	namespace
{
	enum SpeakerLayout
	{
		MS_FRONT_LEFT			= 0,
		MS_FRONT_RIGHT,
		MS_FRONT_CENTER,
		MS_LOW_FREQUENCY,
		MS_BACK_LEFT,
		MS_BACK_RIGHT,
		MS_FRONT_LEFT_OF_CENTER,
		MS_FRONT_RIGHT_OF_CENTER,
		MS_INVALID
	};

	static const i32	c_channelLookup[VuMeter::MODE_COUNT][VuMeter::CHANNEL_MAX] =
	{
		// MODE_MONO
		{ MS_FRONT_LEFT, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID },

		// MODE_STEREO,
		{ MS_FRONT_LEFT, MS_FRONT_RIGHT, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID },

		// MODE_2_1,
		{ MS_FRONT_LEFT, MS_FRONT_RIGHT, MS_LOW_FREQUENCY, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID },

		// MODE_4_0,
		{ MS_FRONT_LEFT, MS_FRONT_RIGHT, MS_BACK_LEFT, MS_BACK_RIGHT, MS_INVALID, MS_INVALID, MS_INVALID, MS_INVALID },

		// MODE_5_1,
		{ MS_FRONT_LEFT, MS_FRONT_RIGHT, MS_FRONT_CENTER, MS_LOW_FREQUENCY, MS_BACK_LEFT, MS_BACK_RIGHT, MS_INVALID, MS_INVALID },

		// MODE_7_1,
		{ MS_FRONT_LEFT, MS_FRONT_RIGHT, MS_FRONT_CENTER, MS_LOW_FREQUENCY, MS_BACK_LEFT, MS_BACK_RIGHT, MS_FRONT_LEFT_OF_CENTER, MS_FRONT_RIGHT_OF_CENTER }
	};

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	f32 ClipZeroToOne(f32 z)
	{
		if(z > 1.0)
			return 1.0;
		else if(z < 0.0)
			return 0.0;
		else
			return z;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	f32 ToDB(f32 v, f32 range)
	{
		double	db;

		if(v > 0)
			db = 20 * logf(fabsf(v));
		else
			db = -999;
		return ClipZeroToOne((f32) ((db + range) / range));
	}
}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	template<> inline f32 VuMeter::convertSample<f32> (f32 _sample) const
	{
		return _sample;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	template<> inline f32 VuMeter::convertSample<i16> (i16 _sample) const
	{
		return((f32) _sample) / 32767.f;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	template<typename T> void VuMeter::doProcess(T *__restrict _samples, i32 _frames, i32 _channels)
	{
		MesureMsg	msg;

		// clear message
		msg.frameCount = _frames;
		for(i32 j = 0; j < m_barCount; j++)
		{
			msg.peak[j] = 0;
			msg.rms[j] = 0;
			msg.clipping[j] = false;
			msg.headPeakCount[j] = 0;
			msg.tailPeakCount[j] = 0;
		}

		// number of useful channel
		const i32	effectiveChannelCount = std::min(_channels, m_barCount);
		f32			sample, absSample;

		for(i32 frame = 0; frame < _frames; frame++)
		{
			for(i32 ch = 0; ch < effectiveChannelCount; ch++)
			{
				const i32	trueCh = c_channelLookup[m_mode][ch];
				AUDIO_ASSERT(trueCh != MS_INVALID, "bad channel routing");

				sample = convertSample(_samples[trueCh]);

				absSample = fabsf(sample); 
				msg.rms[ch] += sample * sample;
				msg.peak[ch] = std::max(msg.peak[ch], absSample);

				if(absSample >= 1.f)
				{
					if(msg.headPeakCount[ch] == frame) msg.headPeakCount[ch]++;

					msg.tailPeakCount[ch]++;
					if(msg.tailPeakCount[ch] > m_numPeakSamplesToClip) msg.clipping[ch] = true;
				}
				else
					msg.tailPeakCount[ch] = 0;
			}

			_samples += _channels;
		}

		for(i32 j = 0; j < m_barCount; j++)
		{
			msg.rms[j] = (f32) ITF::f32_Sqrt(msg.rms[j] / (f32) _frames);
			msg.peak_dB[j] = ToDB(msg.peak[j], (f32) m_dBRange);
			msg.rms_dB[j] = ToDB(msg.rms[j], (f32) m_dBRange);
		}

		post(msg);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void VuMeter::doProcess_float32(f32 *__restrict _samples, i32 _frames, i32 _channels)
	{
		doProcess<f32> (_samples, _frames, _channels);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void VuMeter::doProcess_pcm16(i16 *__restrict _samples, i32 _frames, i32 _channels)
	{
		doProcess<i16> (_samples, _frames, _channels);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void VuMeter::compilData()
	{
		MesureMsg	msg;
		f32			deltaT;
		f32			decayAmount;
        const f32   blend = (1.f-m_rmsBlend);

		while(m_queue.get(msg))
		{
			deltaT = (f32) msg.frameCount / m_samplingRate;
			m_dT += deltaT;

			for(i32 j = 0; j < m_barCount; j++)
			{
				decayAmount = m_decayRate * deltaT / m_dBRange;
				m_bar[j].m_peak = std::max(msg.peak[j], m_bar[j].m_peak - decayAmount);

				// This smooths out the RMS signal
				m_bar[j].m_rms = m_bar[j].m_rms * blend + msg.rms[j] * m_rmsBlend;

				if(m_dT - m_bar[j].m_peakHoldTime > m_peakHoldDuration || m_bar[j].m_peak > m_bar[j].m_peakHold)
				{
					m_bar[j].m_peakHold = m_bar[j].m_peak;
					m_bar[j].m_peakHoldTime = m_dT;
				}

				if(m_bar[j].m_peak > m_bar[j].m_peakPeakHold) m_bar[j].m_peakPeakHold = m_bar[j].m_peak;
				if(msg.clipping[j] || m_bar[j].m_tailPeakCount + msg.headPeakCount[j] >= m_numPeakSamplesToClip) m_bar[j].m_clipping = true;

				m_bar[j].m_tailPeakCount = msg.tailPeakCount[j];
			}
		}	// while
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	VuMeter::VuMeter() :
		m_channel(2),
		m_barCount(2),
		m_numPeakSamplesToClip(3),
		m_dBRange(60),
		m_mode(MODE_STEREO),
		m_queue(),
		m_dT(0.f),
		m_decayRate(60.f),
		m_peakHoldDuration(3.f),
		m_samplingRate(48000.f),
        m_rmsBlend(0.8f)
	{
		memset(m_bar, 0, sizeof(m_bar));
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void VuMeter::reset()
	{
		memset(m_bar, 0, sizeof(m_bar));
		m_queue.reset();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void VuMeter::onStart(const AudioStructs::WaveFormatExtensible &_format)
	{
		reset();
		setMode(MODE_OFF);

		if(_format.Format.formatTag != AudioStructs::FORMAT_PCM)
		{
			AUDIO_WARNING("Vumeter doesnt support this format \"%d\" (only PCM 16 data) !", _format.Format.formatTag);
			return;
		}

		if(_format.Format.bitsPerSample != 16)
		{
			AUDIO_WARNING("Vumeter doesnt support bps \"%d\" (only PCM 16 data) !", _format.Format.bitsPerSample);
			return;
		}

		setSamplingRate (static_cast<float>(_format.Format.samplesPerSec));

		switch(_format.Format.channels)
		{
		case 1:		setMode(VuMeter::MODE_MONO); setBarCount(1); break;
		case 2:		setMode(VuMeter::MODE_STEREO); setBarCount(2); break;
		case 3:		setMode(VuMeter::MODE_2_1); setBarCount(3); break;
		case 4:		setMode(VuMeter::MODE_4_0); setBarCount(4); break;
		case 6:		setMode(VuMeter::MODE_5_1); setBarCount(6); break;
		case 8:		setMode(VuMeter::MODE_7_1); setBarCount(8); break;
		default:	AUDIO_WARNING("Vumeter doesnt support this channel count \"%d\" !", _format.Format.channels); return;
		}

		m_channel = _format.Format.channels;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void VuMeter::onStop()
	{
		reset();
		setMode(MODE_OFF);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void VuMeter::onBuffer(const void *_data, u32 _bytesRecorded)
	{
		if(m_mode != MODE_OFF)
		{
			const int	frames = _bytesRecorded / (m_channel * sizeof(short));
			doProcess_pcm16((short *)_data, frames, m_channel);
		}
	}
}
#endif
