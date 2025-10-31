#ifndef _ITF_MUSICINSTANCE_H_
#define _ITF_MUSICINSTANCE_H_


#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_


#ifndef _ITF_MUSICTREE_H_
#include "engine/blendTree/MusicTree/MusicTree.h"
#endif //_ITF_MUSICTREE_H_



#ifndef _ITF_MUSICSYNCPLAY_H_
#include "engine/sound/Music/MusicSyncPlay.h"
#endif // _ITF_MUSICSYNCPLAY_H_


#include "engine/sound/Music/MusicPart.h"



#if defined ( ITF_WII )

#ifndef _ITF_ADPCMDATAPROVIDER_WII_H_
#include "engine/sound/raki/ADPCMDataProvider_Wii.h"
#endif // _ITF_ADPCMDATAPROVIDER_WII_H_

#elif defined ( ITF_X360 )

#ifndef _ITF_XMA2DATAPROVIDER_X360_H_
#include "engine/sound/raki/XMA2DataProvider_X360.h"
#endif // _ITF_XMA2DATAPROVIDER_X360_H_

#elif defined ( ITF_PS3 )

#ifndef _ITF_MP3DATAPROVIDER_H_
#include "engine/sound/raki/MP3DataProvider_PS3.h"
#endif // _ITF_MP3DATAPROVIDER_H_

#else

#ifndef _ITF_PCMDATAPROVIDER_H_
#include "engine/sound/raki/PCMDataProvider.h"
#endif // _ITF_PCMDATAPROVIDER_H_

#ifdef ITF_SUPPORT_RAKI
#ifndef RAKI_STREAMBUFFERINFO_H
#include "RakiEngine/Sounds/StreamBufferInfo.h"
#endif // RAKI_STREAMBUFFERINFO_H
#ifndef CIRCULARSTATELIST_H
#include "RakiEngine/System/Containers/CircularStateList.h"
#endif // CIRCULARSTATELIST_H
#else
#include "engine/sound/common/SoundCircularStateList.h"
#endif

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

#include "engine/sound/common/DummyDataProvider.h"

#endif


namespace raki
{
    class ADPCMStreamSoundNative;
    class PCMStreamSoundNative;
    class MP3StreamSoundNative;
    class XMA2StreamSoundNative;
    class VoiceBase;
    class RoutingGroup;
}



namespace ITF
{

    class Music;
    class MusicSimple;
    class MusicPart;

 
    class MusicInstance
#ifdef ITF_SUPPORT_RAKI
        : public raki::StreamBufferInfo::StreamUpdateClient
#endif
    {
        friend class MusicInstanceSimple;
        friend class MusicInstanceContainer;


    public:

        MusicInstance( const u32 _nbChannels );
        virtual ~MusicInstance();

        void                                init( Music * _music );
        void                                clear();

        const bbool                         canReinit();
        void                                reinit();

        virtual void                        update( f32 _dt );

        virtual void                        play( const StringID _node, f32 _fadeTime = 0.0f, f32 _volume = .0f, const bbool _loop = btrue, const bbool _stopAndPlay = bfalse );

        virtual void                        stopVoice();    // stop the voice either by closing the sound handle, or by closing the stream
        void                                stop( f32 _fadeTime = 0.0f );         // stops with fade if fade exists, or directly calls stopVoice 

        virtual bbool                       isPlaying() const;

        virtual void                        pause();
        virtual void                        resume();

        // temp 
        void                                setLinearVolume( const f32 _linearVolume );  
        f32                                 getLinearVolume();
        void                                mute(bbool _mute);
        void                                resetMuteFlag() { m_muted = bfalse; }
        bbool                               isMuted() {return m_muted;}

        virtual const MusicPart *           getNextPart();

        const u32                           getMetronomeType() const { return m_metronomeType; }

        // Inputs
        template <typename T>
        void                                setInput(StringID _inputName, T _value );

        StringID                            debugGetMusicName();

        const u32                           getNbChannels();

        ITF_INLINE bbool                    registerForPlayAtNextDivision( const u32 _handle, const u32 _playOnNextSubdivision, const f32 _volume ) { return m_musicSyncPlay.registerForPlayAtNextDivision( _handle, _playOnNextSubdivision, _volume ); }
#ifdef ITF_SUPPORT_RAKI
        ITF_INLINE bbool                    registerForPlayAtNextDivision( raki::VoiceBase * _voice, const u32 _playOnNextSubdivision, const f32 _volume ) { return m_musicSyncPlay.registerForPlayAtNextDivision( _voice, _playOnNextSubdivision, _volume ); }
#else
        ITF_INLINE bbool                    registerForPlayAtNextDivision(SoundInstance* _voice, const u32 _playOnNextSubdivision, const f32 _volume) { return m_musicSyncPlay.registerForPlayAtNextDivision(_voice, _playOnNextSubdivision, _volume); }
        const u64                           getRemainingTimeSec();
#endif
        void                                notifyEnd( const MusicPart* _musicPart);
        void                                updateVolume();

        void                                setRoutingGroup( raki::RoutingGroup * _routingGroup );

        ITF_INLINE void                     setPauseSensitive(const bbool _pauseSensitive);
        ITF_INLINE bbool                    isPauseSensitive() const;
        f32                                 getPosInSec();

    protected:

        void                                create();
        void                                destroy();

        virtual bbool                       isMusic();

        bbool                               isStopping();
        void                                setNewFade( f32 _destVolume, f32 _fadeTime );

        const Music*                        m_music;

#ifdef ITF_SUPPORT_RAKI

#if defined ( ITF_WII )
        ADPCMDataProvider                   m_dataProvider;
        raki::ADPCMStreamSoundNative*       m_sound;
#elif defined ( ITF_X360 )
        XMA2DataProvider                    m_dataProvider;
        raki::XMA2StreamSoundNative*        m_sound;
#elif defined ( ITF_PS3 )
        MP3DataProvider                     m_dataProvider;
        raki::MP3StreamSoundNative*         m_sound;
#else
        PCMDataProvider                     m_dataProvider;
        raki::PCMStreamSoundNative*         m_sound;
#endif
#else
       DummyDataProvider                    m_dataProvider;

#endif

        enum State
        {
            stopped, fading, playing 
        };
        State                               m_state;

        f32                                 m_fadeStartVolume;
        f32                                 m_fadeEndVolume;
        f32                                 m_fadeTotalTime;
        f32                                 m_fadeCurrentTime;

        f32                                 m_currentVolume;

        StringID                            m_lastPlayedNode;
        StringID                            m_nextNodeToPlay;
        f32                                 m_nextFadeTime;
        f32                                 m_nextVolume;
        bbool                               m_bVolumeChanged;

        u64                                 m_lastSamplePosition;

        bbool                               m_initialized;
        bbool                               m_waitingToReinit;
        SoundHandle                         m_playingHandle = SoundHandle::getInvalidHandle();
        ObjectRef                           m_wwiseObjectref = ITF_INVALID_OBJREF;

        struct SynchronizationData
        {
            SynchronizationData() : m_samplePosition( 0 ), m_musicPart( NULL ), m_measurementTime( 0.f ) {}
            SynchronizationData( const u32 _samplePosition, const MusicPart* _musicPart = NULL )
                : m_samplePosition( _samplePosition )
                , m_musicPart( _musicPart )
#ifdef ITF_SUPPORT_RAKI
                , m_measurementTime(raki::Time::getTime())
#else
                , m_measurementTime((f32)SYSTEM_ADAPTER->getTime())
#endif
            {
#if defined(ITF_SUPPORT_WWISE)
                if(m_musicPart)
                m_soundDurationSec = m_musicPart->getWaveDuration();
#endif
            }
            SynchronizationData(const u32 _samplePosition, const f32 _samplePositionsec,const f32 durationsec, const MusicPart* _musicPart = NULL)
                : m_timePositionSec(_samplePositionsec),
                  m_samplePosition(_samplePosition)
                , m_soundDurationSec(durationsec)
                , m_musicPart(_musicPart)
                , m_measurementTime((f32)SYSTEM_ADAPTER->getTime())
            {
            
            }
            ITF_INLINE bool operator != ( const SynchronizationData & _r ) const 
            {
                if ( m_samplePosition != _r.m_samplePosition ) return true;
                if (F32_ALMOST_EQUALS(m_timePositionSec, _r.m_timePositionSec, MTH_EPSILON) == bfalse)
                    return true;
                if ( m_musicPart != _r.m_musicPart ) return true;
                return false;
            }
            bool isEndNotification() const { return (m_timePositionSec == -1.f) && (m_soundDurationSec == -1.f); }
            void setAsEndNotification(const MusicPart* _musicPart)
            {
                m_timePositionSec = -1.f;
                m_soundDurationSec = -1.f;
                m_musicPart = _musicPart;
            }

            u32                     m_samplePosition;
            const MusicPart*        m_musicPart;
            f32                     m_measurementTime;
            f32                     m_timePositionSec = 0;
            f32                     m_soundDurationSec = 0;


        };


    private:

#ifdef ITF_SUPPORT_RAKI
        virtual void                        updateStreamPosition( const void * _clientData, const u64 _samplePosition );
        raki::CircularStateList<SynchronizationData> m_synchronizationDataArray;
#else
        sound::CircularStateList<SynchronizationData> m_synchronizationDataArray;
        
#endif
        void                                          notifySoundEnded(const void* _clientData);
        void                                          updateFromAudioCallback(const void* _clientData, f32 _timePositionSec, f32 _soundDurationSec);

        SynchronizationData                 m_synchronizationData;

        MusicTree                           m_musicTree;

        // Inputs
        InputContainer                      m_inputList;

        MusicSyncPlay                       m_musicSyncPlay;

        u32                                 m_metronomeType;

        u32                                 m_measureSinceStart;
        u32                                 m_measureInPart;

        bbool                               m_muted;
        bbool                               m_pauseSensitive;
    };

    template <typename T>
    ITF_INLINE void MusicInstance::setInput(StringID _inputName, T _value )
    {
        u32 numInputs = m_inputList.size();

        for ( u32 i = 0; i < numInputs; i++ )
        {
            Input& input = m_inputList[i];

            if ( input.getId() == _inputName )
            {
                input.setValue(_value);
                break;
            }
        }
    }

    ITF_INLINE void MusicInstance::setPauseSensitive(const bbool _pauseSensitive)
    {
        m_pauseSensitive = _pauseSensitive;
    }

    ITF_INLINE bbool MusicInstance::isPauseSensitive() const
    {
        return m_pauseSensitive;
    }

    ///////////////////////////////////////////////////////////////////////
    // MUSIC INSTANCE SIMPLE
    ///////////////////////////////////////////////////////////////////////
    //
    // This class is used to play and loop on only 1 music resource. There is no
    // Synchronization data updated as there is no metronome associated with

    class MusicInstanceSimple : public MusicInstance
    {
    public:

        MusicInstanceSimple( const u32 _nbChannels )
            : MusicInstance( _nbChannels )
            , m_isLooping( bfalse )
        {}

        virtual ~MusicInstanceSimple();

        void                                init( const MusicSimple * _music );

        virtual void                        play( const StringID _node, f32 _fadeTime = 0.0f, f32 _volume = .0f, const bbool _loop = btrue, const bbool _stopAndPlay = bfalse );

        virtual void                        update( f32 _dt );

        virtual const MusicPart *           getNextPart();

        ITF_INLINE void                     setIsLooping( bbool _isLooping ) { m_isLooping = _isLooping; }

    protected:

        virtual bbool                       isMusic();

    private:

        bbool                               m_isLooping;
    };

}

#endif //_ITF_MUSICINSTANCE_H_
