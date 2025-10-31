#ifndef __AUDIOSTRUCTS_H__
#define __AUDIOSTRUCTS_H__

#include "adapters/Adapter_WWISE/AudioSDK/types.h"

namespace	AudioSDK
{
	struct AudioStructs
	{
		struct Guid						// 16 bytes
		{
   			u32 Data1;
			u16 Data2;
			u16 Data3;
			u8	Data4[8];
		};

        struct WaveFormat
        {
            u16 formatTag;
            u16 channels;
            u32 samplesPerSec;
            u32 avgBytesPerSec;
            u16 blockAlign;
        } ;

        struct PCMWaveFormat
        {
            WaveFormat  wf;
            u16         bitsPerSample;
        } ;

        struct WaveFormatEx				// 18 bytes
		{
			u16 formatTag;
			u16 channels;
			u32 samplesPerSec;
			u32 avgBytesPerSec;
			u16 blockAlign;
			u16 bitsPerSample;
			u16 cbSize;
		};

		struct WaveFormatExtensible		// 40 bytes
		{
			WaveFormatEx	Format;
			union
			{
				u16 validBitsPerSample; // bits of precision
				u16 samplesPerBlock;	// valid if wBitsPerSample==0
				u16 reserved;			// If neither applies, set to zero.
			} Samples;
			u32		channelMask;		// which channels are
			Guid	SubFormat;
		};

		enum WaveFormatTag
		{
			FORMAT_UNKNOWN		= 0x0000,	
			FORMAT_PCM			= 0x0001,	
			FORMAT_IEEE_FLOAT	= 0x0003,	
			FORMAT_EXTENSIBLE	= 0xFFFE	
		};

        static const Guid c_ksDataFormatSubtypeGuidBase;

        static WaveFormatTag s_extractWaveFormatTag(const Guid& _guid);

        struct WaveDataListener
        {
            virtual ~WaveDataListener() {}
            virtual void onStart(const WaveFormatExtensible&_format) = 0;
            virtual void onStop() = 0;
            virtual void onBuffer(const void *_data, u32 _bytesRecorded) = 0;
        };
	};

}
#endif // __AUDIOSTRUCTS_H__
