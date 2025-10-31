#ifndef __SOUNDRECORDER_STUB_H__
#define __SOUNDRECORDER_STUB_H__

#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"

namespace	AudioSDK
{
	class	SoundRecorder_stub
	{


	public:
			SoundRecorder_stub()																			{ }
			~		SoundRecorder_stub()																	{ }
			bool	startRecording(const char *_filePath, u32 _deviceID, u32 _channel, u32 _sampleRate)		{ return true; }
			bool	stopRecording()																			{ return true; }
            void    setCaptureListener(AudioStructs::WaveDataListener * _val)                               {  }

	};
}
#endif // __SOUNDRECORDER_STUB_H__
