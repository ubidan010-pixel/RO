#include "precompiled_Adapter_WWISE.h"
#ifdef tododisap
#include "adapters/Adapter_WWISE/AudioSDK/WavCleaner.h"
#include "adapters/Adapter_WWISE/AudioSDK/WavFile.h"

#include "adapters/Adapter_WWISE/AudioSDK/error.h"


namespace	AudioSDK
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	WavCleaner::WavCleaner() :  m_sampleBeforeCleaning(0)
	{
		setDefaultSettings();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void inline killDenormal(f32 &_val)
	{
		static const f32	antiDenormal = 1e-18f;
		_val += antiDenormal;
		_val -= antiDenormal;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool WavCleaner::run(const char *_outputWav, const char *_inputWav)
	{
		WavReader	input;

		m_startSample = 0;
		m_endSample = 0;
		m_sampleRate = 0;
		m_sampleBeforeCleaning = 0;

		if(input.open(_inputWav))
		{
			const AudioStructs::WaveFormatExtensible & wfx = input.getWaveFormatEx();

			if(checkFormat(wfx))
			{
				//////////////////////////////////////////////////////////////////////////
				// *  compute settings, in sample unit
				//
				const u32	windowSampleSize = u32(f32(wfx.Format.samplesPerSec) * m_windowSecondsSize);
				const f32	minRMS = m_threshold * m_threshold * windowSampleSize;
				const u32	fadeSampleDuration = u32(f32(wfx.Format.samplesPerSec) * m_fadeSecondsDuration);

				//////////////////////////////////////////////////////////////////////////
				// *  parse data to find beginning and ending
				u32			startSample = 0, endSample = 0, currentSample = 0;
				f32			*window = new f32[windowSampleSize];
				memset(window, 0, sizeof(f32) * windowSampleSize);

				u32			currIdx = 0;
				f32			rmsWindow = 0.;

				u32			dataBytes = input.gotoChunk(WavFile::c_tagdata);

				const u32	allocatedSize = std::min(windowSampleSize * (u32) sizeof(i16) * wfx.Format.channels, dataBytes);
				u8			*allocatedBuffer = new u8[allocatedSize];

				u32			bufferSize = allocatedSize;
				i16			*buffer = reinterpret_cast < i16 * > (allocatedBuffer);

				while(dataBytes && input.read(buffer, bufferSize))
				{
					dataBytes -= bufferSize;

					while(bufferSize)
					{
						f32 frameSum = 0.f;

						for(u32 channel = 0; channel < wfx.Format.channels; ++channel)
						{
							const f32	sample = static_cast<f32>(*buffer++) / 32767.f;
							frameSum += (sample * sample);
						}

						rmsWindow -= window[currIdx];
						rmsWindow = std::max(0.f, rmsWindow);

						window[currIdx] = frameSum;
						rmsWindow += window[currIdx];

						killDenormal(rmsWindow);

						if(rmsWindow >= minRMS)
						{
							if(startSample == 0) startSample = currentSample;

							endSample = currentSample;
						}

						bufferSize -= sizeof(i16) * wfx.Format.channels;
						currIdx = (currIdx + 1) % windowSampleSize;
						++currentSample;
					}

					bufferSize = std::min(allocatedSize, dataBytes);
					buffer = reinterpret_cast < i16 * > (allocatedBuffer);
				}

				m_sampleBeforeCleaning = currentSample;

				delete[] window;
				window = NULL;

				//////////////////////////////////////////////////////////////////////////
				// *  now we know that the wav start/end
				//
				if(startSample && endSample)
				{
					if(m_cleanIntro == false)
						startSample = 0;
					else if(m_cleanOutro == false)
						endSample = endSample - fadeSampleDuration;

					// adjust start/end with the fade duration
					if(startSample <= fadeSampleDuration)
						startSample = 0;
					else
						startSample = startSample - fadeSampleDuration;

					if(endSample <= fadeSampleDuration)
						endSample = 0;
					else
						endSample = endSample - fadeSampleDuration;

					WavWriter	output;
					if(output.open(_outputWav))
					{
						output.writeHeader(wfx);

						// goto data
						dataBytes = input.gotoChunk(WavFile::c_tagdata);

						// move to start sample
						input.setSeek(input.getSeek() + wfx.Format.blockAlign * startSample);
						dataBytes -= wfx.Format.blockAlign * startSample;
						bufferSize = std::min(allocatedSize, dataBytes);

						// prepare fade-in
						f32 fadeCoeff = 0.f;
						f32 fadeInc = 1.f / (f32) fadeSampleDuration;
						currentSample = startSample;

						m_startSample = startSample;
						m_sampleRate = wfx.Format.samplesPerSec;

						// copy
						buffer = reinterpret_cast < i16 * > (allocatedBuffer);
						while(dataBytes && input.read(buffer, bufferSize))
						{
							dataBytes -= bufferSize;

							const u32	sampleCount = bufferSize / wfx.Format.blockAlign;
							for(u32 sample = 0; sample < sampleCount; ++sample)
							{
								for(u32 channel = 0; channel < wfx.Format.channels; ++channel)
								{
									buffer[channel] = (i16) ((f32) buffer[channel] * fadeCoeff);
								}

								++currentSample;

								fadeCoeff += fadeInc;

								if(fadeCoeff >= 1.f)
									fadeCoeff = 1.f;
								else if(fadeCoeff <= 0.f)
									fadeCoeff = 0.f;

								if((fadeInc > 0.f) && (currentSample >= endSample)) fadeInc = -fadeInc;

								if(currentSample >= (endSample + fadeSampleDuration))
								{
									bufferSize = sample * wfx.Format.blockAlign;
									break;
								}

								++buffer;
							}

							output.writeData(allocatedBuffer, bufferSize);

							if(currentSample >= (endSample + fadeSampleDuration)) break;

							bufferSize = std::min(allocatedSize, dataBytes);
							buffer = reinterpret_cast < i16 * > (allocatedBuffer);
						}

						m_endSample = currentSample;

						output.close();
					}
				}
                else
                {
                    // copy the file
                    WavWriter	output;
                    if(output.open(_outputWav))
                    {
                        output.writeHeader(wfx);

                        // goto data
                        dataBytes = input.gotoChunk(WavFile::c_tagdata);
                        bufferSize = std::min(allocatedSize, dataBytes);

                        // copy
                        buffer = reinterpret_cast < i16 * > (allocatedBuffer);
                        while(dataBytes && input.read(buffer, bufferSize))
                        {
                            dataBytes -= bufferSize;
                            
                            output.writeData(allocatedBuffer, bufferSize);
                            
                            bufferSize = std::min(allocatedSize, dataBytes);
                            buffer = reinterpret_cast < i16 * > (allocatedBuffer);
                        }
                        output.close();
                    }
                }

				delete[] allocatedBuffer;
			}

			input.close();
			return true;
		}

		return false;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool WavCleaner::checkFormat(const AudioStructs::WaveFormatExtensible &_wfx)
	{
		if((_wfx.Format.formatTag == AudioStructs::FORMAT_PCM) && (_wfx.Format.bitsPerSample == 16)) return true;

		AUDIO_WARNING("WavCleaner : invalid WAV format. Only PCM 16 is supported");
		return false;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WavCleaner::setLevelThreshold(f32 _dB, f32 _windowsSec)
	{
		if((_dB < 0.f) && (-100.f < _dB))
		{
			m_threshold = powf(10.f, _dB * 0.05f);
		}
		else
		{
			AUDIO_WARNING("WavCleaner : bad \"Level threshold\" value (must be inside the range ]-100..0[ dB)");
		}

		if((0.f < _windowsSec) && (_windowsSec <= 10.f))
		{
			// db = 20 log A A = pow(10, dB/20)
			m_windowSecondsSize = _windowsSec;
		}
		else
		{
			AUDIO_WARNING("WavCleaner : bad \"window duration\" value (must be inside the range ]0..10] sec)");
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WavCleaner::getLevelThreshold(f32 &_dB, f32 &_windowsSec)
	{
		_dB = 20.f * log10f(m_threshold);
		_windowsSec = m_windowSecondsSize;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WavCleaner::setFadeDuration(f32 _val)
	{
		if((0.f < _val) && (_val <= 10.f))
		{
			m_fadeSecondsDuration = _val;
		}
		else
		{
			AUDIO_WARNING("WavCleaner : bad \"fade duration\" valeu (must be inside the range ]0..10] sec)");
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WavCleaner::getFadeDuration(f32 &_val)
	{
		_val = m_fadeSecondsDuration;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WavCleaner::setDefaultSettings()
	{
		m_windowSecondsSize = 0.01f;
		m_threshold = 0.025f;
		m_fadeSecondsDuration = 0.1f;
		m_cleanIntro = true;
		m_cleanOutro = true;

		m_startSample = 0;
		m_endSample = 0;
		m_sampleRate = 0;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	f32 WavCleaner::getDeletedDurationAtTheBeginning() const
	{
		if(m_sampleRate)
		{
			return static_cast<f32>(m_startSample + 1) / static_cast<f32>(m_sampleRate);
		}

		return 0.f;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	f32 WavCleaner::getDeletedDurationAtTheEnd() const
	{
		if(m_sampleRate)
		{
			return static_cast<f32>(m_sampleBeforeCleaning - m_endSample + 1) / static_cast<f32>(m_sampleRate);
		}

		return 0.f;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	f32 WavCleaner::getDurationAfterCleaning() const
	{
		if(m_sampleRate)
		{
			return static_cast<f32>(m_endSample - m_startSample + 1) / static_cast<f32>(m_sampleRate);
		}

		return 0.f;
	}
}
#endif
