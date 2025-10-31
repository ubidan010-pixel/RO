#ifndef __SOUNDINPUTDEVICEMANAGER_WIN32_H__
#define __SOUNDINPUTDEVICEMANAGER_WIN32_H__

#include <mmsystem.h>
#include <vector>
//#include <audiodefs.h>
#include "adapters/Adapter_WWISE/AudioSDK/types.h"
#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"

namespace	AudioSDK
{
	class	SoundRecorder_win32;

	class	SoundInputDeviceManager_win32
	{
			static SoundInputDeviceManager_win32	*ms_singleton;

	//
	// -------------------------------------------------------------------------------------------------------------------
	//    common prototype
	// -------------------------------------------------------------------------------------------------------------------
	//


	public:
			inline static SoundInputDeviceManager_win32 *s_getSingleton()	{ return ms_singleton; }
			static void									s_createSingleton() { ms_singleton = WWISE_NEW(SoundInputDeviceManager_win32) (); }
			static void									s_destroySingleton(){ if(ms_singleton){ WWISE_DELETE(ms_singleton); } }

			void										enumerateDevices();
			u32											getDefaultDeviceID() const;
			void										getDefaultFormat(AudioStructs::WaveFormatExtensible * _formatEx, u32 _deviceID);
			void										getCompatibleFormat(AudioStructs::WaveFormatExtensible * _formatEx, u32 _deviceID, u32 _channels, u32 _sampleRate);
			SoundRecorder_win32							*createRecorder();
			void										destroyRecorder(SoundRecorder_win32 *_recorder);


	private:
			SoundInputDeviceManager_win32();
			SoundInputDeviceManager_win32(const SoundInputDeviceManager_win32 &);				// Prevent copy-construction
			SoundInputDeviceManager_win32 &operator =(const SoundInputDeviceManager_win32 &);	// Prevent assignment
			virtual									~SoundInputDeviceManager_win32();

			struct InputDevice
			{
				UINT		m_deviceID;
				WAVEINCAPS	m_capacities;
				u32			m_maxChannels;
				bool		m_deviceInputChannelCountIsKnown;
				u32			m_defaultSampleRate;
			};

			std::vector<InputDevice>		m_devices;

			static const i32				c_defaultSampleRateSearchOrderSize = 13;
			static const u32				c_defaultSampleRateSearchOrder[c_defaultSampleRateSearchOrderSize];
			

			static void						s_detectDefaultSampleRate(InputDevice &_deviceInfo, bool (*_waveFormatExQueryFunction) (i32, AudioStructs::WaveFormatEx *), i32 _maxChannels);
			static bool						s_queryFormatSupported(const InputDevice &_deviceInfo, bool (*_waveFormatExQueryFunction) (i32, AudioStructs::WaveFormatEx *), i32 _channels, u32 _sampleRate);
			static void						s_initializeWaveFormatExtensible(AudioStructs::WaveFormatExtensible * _waveFormatEx,
																			 i32 numChannels,
																			 i32 bytesPerSample,
																			 i32 waveFormatTag,
																			 u32 sampleRate,
																			 DWORD channelMask = 0);
			static void						s_initializeWaveFormatEx(AudioStructs::WaveFormatEx * waveFormatEx, i32 numChannels, i32 bytesPerSample, i32 waveFormatTag, u32 sampleRate);
			static bool						s_queryInputWaveFormatEx(i32 deviceId, AudioStructs::WaveFormatEx * waveFormatEx);
	};
}
#endif // __SOUNDINPUTDEVICEMANAGER_WIN32_H__
