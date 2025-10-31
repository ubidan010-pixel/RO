#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
//#include <audiodefs.h>
#include <mmsystem.h>
#include "adapters/Adapter_WWISE/AudioSDK/Win32/SoundInputDeviceManager_win.h"
#include "adapters/Adapter_WWISE/AudioSDK/Win32/SoundRecorder_win.h"
#include "adapters/Adapter_WWISE/AudioSDK/memory.h"

namespace	AudioSDK
{
	SoundInputDeviceManager_win32 *SoundInputDeviceManager_win32::	ms_singleton = NULL;

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	SoundInputDeviceManager_win32::SoundInputDeviceManager_win32()
	{
		enumerateDevices();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	SoundInputDeviceManager_win32::~SoundInputDeviceManager_win32()
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool SoundInputDeviceManager_win32::s_queryInputWaveFormatEx(i32 _deviceId, AudioStructs::WaveFormatEx *_waveFormatEx)
	{
		switch(waveInOpen(NULL, _deviceId, (LPCWAVEFORMATEX) _waveFormatEx, 0, 0, WAVE_FORMAT_QUERY))
		{
		case MMSYSERR_NOERROR:
			return true;

		case MMSYSERR_ALLOCATED:	// Specified resource is already allocated.
			return false;

		case MMSYSERR_NODRIVER:		// No device driver is present.
			return false;

		case MMSYSERR_NOMEM:		// Unable to allocate or lock memory.
			return false;

		case WAVERR_BADFORMAT:		// Attempted to open with an unsupported waveform-audio format.
			return false;

		case MMSYSERR_BADDEVICEID:	// Specified device identifier is out of range.

		// falls through
		default:
			return false;
		}
	}

	

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	void SoundInputDeviceManager_win32::s_initializeWaveFormatExtensible(AudioStructs::WaveFormatExtensible *waveFormatEx, i32 numChannels, i32 bytesPerSample, i32 waveFormatTag, u32 sampleRate, DWORD channelMask)
	{
		unsigned long	bytesPerFrame = numChannels * bytesPerSample;

		waveFormatEx->Format.formatTag = AudioStructs::FORMAT_EXTENSIBLE;
		waveFormatEx->Format.channels = (WORD) numChannels;
		waveFormatEx->Format.samplesPerSec = (DWORD) sampleRate;
		waveFormatEx->Format.avgBytesPerSec = waveFormatEx->Format.samplesPerSec * bytesPerFrame;
		waveFormatEx->Format.blockAlign = (WORD) bytesPerFrame;
		waveFormatEx->Format.bitsPerSample = (u16)bytesPerSample * 8;
		waveFormatEx->Format.cbSize = 22;
		waveFormatEx->Samples.validBitsPerSample = waveFormatEx->Format.bitsPerSample;
		waveFormatEx->channelMask = channelMask;

		AudioStructs::Guid	guid = AudioStructs::c_ksDataFormatSubtypeGuidBase;
		guid.Data1 = (USHORT) waveFormatTag;

		waveFormatEx->SubFormat = guid;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void SoundInputDeviceManager_win32::s_initializeWaveFormatEx(AudioStructs::WaveFormatEx *waveFormatEx, i32 numChannels, i32 bytesPerSample, i32 waveFormatTag, u32 sampleRate)
	{
		unsigned long	bytesPerFrame = numChannels * bytesPerSample;

		waveFormatEx->formatTag =(u16) waveFormatTag;
		waveFormatEx->channels = (WORD) numChannels;
		waveFormatEx->samplesPerSec = (DWORD) sampleRate;
		waveFormatEx->avgBytesPerSec = waveFormatEx->samplesPerSec * bytesPerFrame;
		waveFormatEx->blockAlign = (WORD) bytesPerFrame;
		waveFormatEx->bitsPerSample = (u16)bytesPerSample * 8;
		waveFormatEx->cbSize = 0;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool SoundInputDeviceManager_win32::s_queryFormatSupported(const InputDevice &_deviceInfo, bool (*_waveFormatExQueryFunction) (i32, AudioStructs::WaveFormatEx *), i32 _channels, u32 _sampleRate)
	{
		i32 waveFormatTag = AudioStructs::FORMAT_PCM;

		// attempt bypass querying the device for linear formats
		if(_sampleRate == 11025 && ((_channels == 1 && (_deviceInfo.m_capacities.dwFormats & WAVE_FORMAT_1M16)) || (_channels == 2 && (_deviceInfo.m_capacities.dwFormats & WAVE_FORMAT_1S16))))
		{
			return true;
		}

		if(_sampleRate == 22050 && ((_channels == 1 && (_deviceInfo.m_capacities.dwFormats & WAVE_FORMAT_2M16)) || (_channels == 2 && (_deviceInfo.m_capacities.dwFormats & WAVE_FORMAT_2S16))))
		{
			return true;
		}

		if(_sampleRate == 44100 && ((_channels == 1 && (_deviceInfo.m_capacities.dwFormats & WAVE_FORMAT_4M16)) || (_channels == 2 && (_deviceInfo.m_capacities.dwFormats & WAVE_FORMAT_4S16))))
		{
			return true;
		}

		//
		// first, attempt to query the device using WAVEFORMATEXTENSIBLE, if this fails we
		// fall back to WAVEFORMATEX
		//
		AudioStructs::WaveFormatExtensible	waveFormat;
		s_initializeWaveFormatExtensible(&waveFormat, _channels, sizeof(i16), waveFormatTag, _sampleRate);

		if(_waveFormatExQueryFunction(_deviceInfo.m_deviceID, &waveFormat.Format)) return true;

		s_initializeWaveFormatEx(&waveFormat.Format, _channels, sizeof(i16), waveFormatTag, _sampleRate);

		return _waveFormatExQueryFunction(_deviceInfo.m_deviceID, &waveFormat.Format);
	}

	const u32 SoundInputDeviceManager_win32::	c_defaultSampleRateSearchOrder[SoundInputDeviceManager_win32::c_defaultSampleRateSearchOrderSize] =
	{
		48000,
		44100,
		32000,
		24000,
		22050,
		88200,
		96000,
		192000,
		16000,
		12000,
		11025,
		9600,
		8000
	};

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	void SoundInputDeviceManager_win32::s_detectDefaultSampleRate(InputDevice &_deviceInfo, bool (*_waveFormatExQueryFunction) (i32, AudioStructs::WaveFormatEx *), i32 _maxChannels)
	{
		_deviceInfo.m_defaultSampleRate = 0;

		for(i32 i = 0; i < c_defaultSampleRateSearchOrderSize; ++i)
		{
			u32 sampleRate = c_defaultSampleRateSearchOrder[i];
			if(s_queryFormatSupported(_deviceInfo, _waveFormatExQueryFunction, _maxChannels, sampleRate))
			{
				_deviceInfo.m_defaultSampleRate = sampleRate;
				break;
			}
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void SoundInputDeviceManager_win32::enumerateDevices()
	{
		InputDevice inputDevice;
		const i32	inputDeviceCount = (i32) waveInGetNumDevs();

		m_devices.clear();
		m_devices.reserve(inputDeviceCount);

		for(i32 i = -1; i < inputDeviceCount; ++i)
		{
			inputDevice.m_deviceID = (UINT) ((i == -1) ? WAVE_MAPPER : i);

			const MMRESULT	mmresult = waveInGetDevCaps(inputDevice.m_deviceID, &inputDevice.m_capacities, sizeof(WAVEINCAPS));

			if(mmresult != MMSYSERR_NOERROR)
			{
				continue;
			}

			if(inputDevice.m_capacities.wChannels == 0xFFFF || inputDevice.m_capacities.wChannels < 1 || inputDevice.m_capacities.wChannels > 255)
			{
				//
				// For Windows versions using WDM (possibly Windows 98 ME and later) the kernel
				// mixer sits between the application and the driver. As a result, wave*GetDevCaps
				// often kernel mixer channel counts, which are unlimited.
				//
				inputDevice.m_maxChannels = 2;
				inputDevice.m_deviceInputChannelCountIsKnown = false;
			}
			else
			{
				inputDevice.m_maxChannels = inputDevice.m_capacities.wChannels;
				inputDevice.m_deviceInputChannelCountIsKnown = true;
			}

			s_detectDefaultSampleRate(inputDevice, s_queryInputWaveFormatEx, inputDevice.m_maxChannels);

			m_devices.push_back(inputDevice);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	u32 SoundInputDeviceManager_win32::getDefaultDeviceID() const
	{
		return WAVE_MAPPER;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void SoundInputDeviceManager_win32::getDefaultFormat(AudioStructs::WaveFormatExtensible *_formatEx, u32 _deviceID)
	{
		for(std::vector<InputDevice>::iterator it = m_devices.begin(), itEnd = m_devices.end(); it != itEnd; ++it)
		{
			if(it->m_deviceID == _deviceID)
			{
				s_initializeWaveFormatEx(&_formatEx->Format, it->m_maxChannels, sizeof(i16), AudioStructs::FORMAT_PCM, it->m_defaultSampleRate);
				break;
			}
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void SoundInputDeviceManager_win32::getCompatibleFormat(AudioStructs::WaveFormatExtensible *_formatEx, u32 _deviceID, u32 _channels, u32 _sampleRate)
	{
		for(std::vector<InputDevice>::iterator it = m_devices.begin(), itEnd = m_devices.end(); it != itEnd; ++it)
		{
			if(it->m_deviceID == _deviceID)
			{
				// limit channel count
				if(_channels > it->m_maxChannels) _channels = it->m_maxChannels;

				// test sample rate + WaveFormatExtensible
				s_initializeWaveFormatExtensible(_formatEx, _channels, sizeof(i16), AudioStructs::FORMAT_PCM, _sampleRate);
				if(s_queryInputWaveFormatEx(it->m_deviceID, &_formatEx->Format))
				{
					break;
				}

				// test sample rate + WaveFormatEx
				s_initializeWaveFormatEx(&_formatEx->Format, _channels, sizeof(i16), AudioStructs::FORMAT_PCM, _sampleRate);
				if(s_queryInputWaveFormatEx(it->m_deviceID, &_formatEx->Format))
				{
					break;
				}

				// else try other one WaveFormatExtensible
				for(i32 i = 0; i < c_defaultSampleRateSearchOrderSize; ++i)
				{
					const u32	sampleRate = c_defaultSampleRateSearchOrder[i];
					if(sampleRate > (2 * _sampleRate)) continue;

					s_initializeWaveFormatExtensible(_formatEx, _channels, sizeof(i16), AudioStructs::FORMAT_PCM, sampleRate);
					if(s_queryInputWaveFormatEx(it->m_deviceID, &_formatEx->Format)) break;

					s_initializeWaveFormatEx(&_formatEx->Format, _channels, sizeof(i16), AudioStructs::FORMAT_PCM, sampleRate);
					if(s_queryInputWaveFormatEx(it->m_deviceID, &_formatEx->Format)) break;
				}

				// else use default sample rate
				s_initializeWaveFormatEx(&_formatEx->Format, _channels, sizeof(i16), AudioStructs::FORMAT_PCM, it->m_defaultSampleRate);
				break;
			}
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	SoundRecorder_win32 *SoundInputDeviceManager_win32::createRecorder()
	{
		return WWISE_NEW(SoundRecorder_win32);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void SoundInputDeviceManager_win32::destroyRecorder(SoundRecorder_win32 *_recorder)
	{
		WWISE_DELETE(_recorder);
	}
}
#endif
