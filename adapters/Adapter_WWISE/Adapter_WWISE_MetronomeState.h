#ifndef __ADAPTER_WWISE_METRONOMESTATE_H__
#define __ADAPTER_WWISE_METRONOMESTATE_H__

#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#include "adapters/Adapter_WWISE/AudioSDK/Lifo.h"
#include "adapters/Adapter_WWISE/Configure/WWISE_Configure.h"

struct AkMusicSyncCallbackInfo;

namespace ITF
{
	class Adapter_WWISE::	MetronomeState
	{
        MetronomeState&operator=(const MetronomeState&);

			struct MusicalSegmentSignature
			{
				u32 m_beatsPerBar;			///< beats per bar
				u32 m_segmentDuration_ms;	///< segment duration in ms
				u32 m_barDuration_ms;		///< bar duration in ms

				MusicalSegmentSignature();
				inline bool operator	!=(const MusicalSegmentSignature &_r) const;
			};

            struct MusicCallbackEvent
            {
                i32         m_eventFlag;
                StringID    m_cueName;

                MusicCallbackEvent();
                MusicCallbackEvent(i32 _eventFlag, const char*_cueName);
            };

	public:
			struct MusicCallbackInfo
			{
				f32 m_beatDuration;			///< Beat Duration in seconds.
				f32 m_barDuration;			///< Bar Duration in seconds.
				f32 m_gridDuration;			///< Grid duration in seconds.
				f32 m_gridOffset;			///< Grid offset in seconds.

				MusicCallbackInfo();
                MusicCallbackInfo(f32 _beatDuration, f32 _barDuration, f32 _gridDuration, f32 _gridOffset) ;
				inline bool operator	!=(const MusicCallbackInfo &_r) const;
			};

            f32         m_musicDuration_msec = 0.f;
	public:
			explicit	MetronomeState(MetronomeType _metronomeID);
			void		attach(SoundHandle _playingID);
			void		detach(SoundHandle _playingID);
			void		update();
			bool		musicCallBack(const AkMusicSyncCallbackInfo *_akMusicSyncCallbackInfo);
            bool durationCallBack(const AkDurationCallbackInfo* _akInfo, SoundHandle _soundHandle);
			SoundHandle playOnNext(const AudioPlayRequest&_internalPlayRequest);
			bbool		sendEventOnNext(const AudioSendEventRequest &_internalSendEventRequest);

	private:
		void sendEvent(const AudioSendEventRequest &_sendEventRequest) const;

	private:
			const MetronomeType						c_metronomeID;
			SoundHandle							    m_playingID;
			MusicalSegmentSignature					m_currentSegment;
            AudioSDK::Lifo<MusicCallbackInfo, 32>   m_lastCallbackInfo;
			u32										m_measureSinceStart;
			u32										m_measureInPart;
            bool                                    m_isPause;
            static const u32                        c_syncPlayStackMaxSize = Wwise::Configure::METRONOME_SYNC_PLAY_STACK_SIZE;
			AudioPlayRequest                        m_syncPlayStack[Wwise::Configure::METRONOME_SYNC_PLAY_STACK_SIZE];
			AudioSendEventRequest					m_syncSendEventStack[Wwise::Configure::METRONOME_SYNC_PLAY_STACK_SIZE];
			u32                                     m_syncPlayStackSize;
			u32                                     m_syncSendEventStackSize;
            AudioSDK::RingBuffer<MusicCallbackEvent, Wwise::Configure::METRONOME_RINGBUFFER_SIZE> m_callbackEventRingBuffer;
	};
}
#endif // ITF_SUPPORT_WWISE
#endif // __ADAPTER_WWISE_METRONOMESTATE_H__
