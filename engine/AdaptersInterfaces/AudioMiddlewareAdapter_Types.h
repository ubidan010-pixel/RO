#ifndef __AUDIOMIDDLEWAREADAPTER_TYPES_H__
#define __AUDIOMIDDLEWAREADAPTER_TYPES_H__

#ifndef __AUDIOMIDDLEWAREADAPTER_SELECTOR_H__
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Selector.h"
#endif

namespace	ITF
{
   


    class	SoundHandle
    {


    public:
        static const u32	c_invalidValue = 0;//AK_INVALID_PLAYING_ID is zero in wwise

        explicit SoundHandle(u32 _handle) :
            m_handle(_handle)
        {
        }

        SoundHandle() :
            m_handle(c_invalidValue)
        {
        }

        inline bbool isValid() const
        {
            return m_handle != c_invalidValue;
        }

        inline void invalidate() { m_handle = c_invalidValue; }
        inline u32 getValueAsU32() const
        {
            return m_handle;
        }

        static SoundHandle	getInvalidHandle() { return SoundHandle(c_invalidValue); }
        inline bbool operator==(ITF::SoundHandle const& _handle2) const
        {
            return getValueAsU32() == _handle2.getValueAsU32();
        }

        inline bbool operator!=(ITF::SoundHandle const& _handle2) const
        {
            return getValueAsU32() != _handle2.getValueAsU32();
        }




    private:
        u32 m_handle;
    };






}

//$4
// ***********************************************************************************************************************
//    ITF_SUPPORT_RAKI
// ***********************************************************************************************************************
//

#if defined(ITF_SUPPORT_RAKI)
#include "engine/sound/SoundInstance.h"
namespace	ITF
{
	typedef StringID	SoundBusID;
	typedef StringID	SoundEffectID;
	typedef StringID	SoundEventID;
	typedef StringID	SoundRtpcID;
	typedef StringID	SoundBankID;
}
#define ITF_INVALID_SOUND_BUS_ID		ITF::StringID::Invalid
#define ITF_INVALID_SOUND_EFFECT_ID		ITF::StringID::Invalid
#define ITF_INVALID_SOUND_EVENT_ID		ITF::StringID::Invalid
#define ITF_INVALID_SOUND_RTPC_ID		ITF::StringID::Invalid
#define ITF_INVALID_SOUND_PLAYING_ID	ITF::SoundHandle::getInvalidHandle()
#define ITF_INVALID_SOUND_BANK_ID		ITF::StringID::Invalid


//$4
// ***********************************************************************************************************************
//    ITF_SUPPORT_WWISE
// ***********************************************************************************************************************
//

#elif defined(ITF_SUPPORT_WWISE)
#include "engine/sound/SoundInstance.h"
namespace	ITF
{


	typedef u32 SoundBusID;
	typedef u32 SoundEffectID;
	typedef u32 SoundEventID;
	typedef u32 SoundRtpcID;
	typedef u32 SoundBankID;

    constexpr u32 AK_MAX_AUX_PER_OBJ = 4;

    enum class ListenerID : u64
    {
        eDefaultListener = 2, //start at 2 as global gao is 1
        eCenterSpeakerListener,
        eListenerCount
    };
}	// namespace ITF
#define ITF_INVALID_SOUND_BUS_ID		0
#define ITF_INVALID_SOUND_EFFECT_ID		0
#define ITF_INVALID_SOUND_EVENT_ID		0
#define ITF_INVALID_SOUND_RTPC_ID		0
#define ITF_INVALID_SOUND_PLAYING_ID	ITF::SoundHandle::getInvalidHandle()
#define ITF_INVALID_SOUND_BANK_ID		0

#define SOUND_BUS_MUSIC   ITF_GET_STRINGID_CRC(MUSIC, 863906348)
#define SOUND_BUS_MASTER   ITF_GET_STRINGID_CRC(master, 2884859710)




//$4
// ***********************************************************************************************************************
// ***********************************************************************************************************************
//

#else
#error "No Audio Engine defined"
#endif

//$4
// ***********************************************************************************************************************
//    Common
// ***********************************************************************************************************************
//

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Enums.h"
    
namespace	ITF
{
	class Event;
        class MusicPart;

	struct AudioSendEventRequest
	{
	private:
		Event*          m_onPlayEvent;
	public:
		MetronomeType   m_metronomeID;
		AudioSyncPlayAt m_playAt;
		StringID        m_playAtCue;
		ObjectRef       m_sentToObjectRef;

		AudioSendEventRequest();
		AudioSendEventRequest(MetronomeType _metronomeID, AudioSyncPlayAt _playAt, StringID _playAtCue);
		AudioSendEventRequest(const AudioSendEventRequest&_ref) ;
		virtual ~AudioSendEventRequest();

		const AudioSendEventRequest &operator	=(const AudioSendEventRequest &);

		inline Event*   getOnPlayEvent() const { return m_onPlayEvent; }
		void            setOnPlayEvent(Event* val);
	};

	struct AudioPlayRequest : public AudioSendEventRequest
    {
    public:
        SoundEventID    m_soundEventID;
        ObjectRef       m_objectRef;
        bool            m_stopOnObjecDestroy;
	bool			m_sourcePlayPosition;
        const MusicPart* m_musicPart = nullptr;


    
        explicit AudioPlayRequest(SoundEventID _soundEventID= ITF_INVALID_SOUND_EVENT_ID) ;
		AudioPlayRequest(SoundEventID _soundEventID, ObjectRef _objectRef);
        AudioPlayRequest(SoundEventID _soundEventID, ObjectRef _objectRef, bool _stopOnObjecDestroy, MetronomeType _metronomeID, AudioSyncPlayAt _playAt, StringID _playAtCue, bool _sourcePlayPosition = false );
        AudioPlayRequest(const AudioPlayRequest&_ref) ;
        virtual ~AudioPlayRequest();

		const AudioPlayRequest &operator	=(const AudioPlayRequest &);
    };
    
}     
#endif // __AUDIOMIDDLEWAREADAPTER_TYPES_H__
