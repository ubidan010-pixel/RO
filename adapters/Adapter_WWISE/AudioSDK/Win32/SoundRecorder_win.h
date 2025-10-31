#ifndef __SOUNDRECORDER_WIN32_H__
#define __SOUNDRECORDER_WIN32_H__

#include <Mmsystem.h>
#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"

#define AUDIO_INPUT_NUM_BUFFERS 8
#define AUDIO_INPUT_BUFFER_SIZE 10240

namespace	AudioSDK
{
	class	SoundRecorder_win32
	{


	public:
			SoundRecorder_win32();
			~		SoundRecorder_win32();

			bool	startRecording(const char *_filePath, u32 _deviceID, u32 _channel, u32 _sampleRate);
			bool	stopRecording();
            void    setCaptureListener(AudioStructs::WaveDataListener * _val) { m_captureListener = _val; }

	private:
			static void CALLBACK	s_captureCallback(HWAVEIN _hwi, UINT _msg, DWORD_PTR _instance, DWORD_PTR _param1, DWORD_PTR _param2);
			inline u32 getFullBufferSize() const
			{
				return AUDIO_INPUT_BUFFER_SIZE * sizeof(i16);
			}

			FILE								*m_fileHandle;
			u32        							m_dataSizeSeek;
			size_t   							m_dataSize;
			size_t  							m_headerSize;
			HWAVEIN								m_waveInHandle;
			WAVEHDR								m_waveHeader[AUDIO_INPUT_NUM_BUFFERS];
			AudioStructs::WaveFormatExtensible	m_waveFormatEx;
			bool								m_successfullyInitOnceAndMustSleepOnExit;
            AudioStructs::WaveDataListener        *m_captureListener;
	};
}
#endif // __SOUNDRECORDER_WIN32_H__
