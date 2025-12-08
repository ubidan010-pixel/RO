#include "precompiled_engine.h"

#ifndef _ITF_MUSICINSTANCE_H_
#include "engine/sound/Music/MusicInstance.h"
#endif // _ITF_MUSICINSTANCE_H_

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_


#ifndef RAKI_ENGINEINITIALIZER_H
#include "RakiEngine/Initializer/EngineInitializer.h"
#endif // RAKI_ENGINEINITIALIZER_H

#ifndef RAKI_WAVEFILE_H
#include "RakiEngine/Data/WaveFile/WaveFile.h"
#endif // RAKI_WAVEFILE_H

#ifndef RAKI_FORMAT_H
#include "RakiEngine/Sounds/Format.h"
#endif // RAKI_FORMAT_H


#ifndef _ITF_MUSIC_H_
#include "engine/sound/Music/Music.h"
#endif // _ITF_MUSIC_H_

#ifndef _ITF_PCMDATAPROVIDER_H_
#include "engine/sound/raki/PCMDataProvider.h"
#endif //_ITF_PCMDATAPROVIDER_H_



#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#include "engine/sound/common/SoundVolume.h"



#if defined ( RAKI_PLATFORM_WIN32 ) || defined ( RAKI_PLATFORM_WIN64 )

#ifndef RAKI_PCMSTREAMSOUNDNATIVE_H
#include "RakiEngine/Sounds/_XAudio2/PCMStreamSoundNative.h"
#endif // RAKI_PCMSTREAMSOUNDNATIVE_H

#ifndef RAKI_AUDIOCALLBACKMANAGER_H
#include "RakiEngine/System/AudioCallbackManager/AudioCallbackManager.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_H

#elif defined ( RAKI_PLATFORM_XB360 )
/*
#ifndef RAKI_PCMSTREAMSOUNDNATIVE_H
#include "RakiEngine/Sounds/_XAudio2/PCMStreamSoundNative.h"
#endif // RAKI_PCMSTREAMSOUNDNATIVE_H
*/
#ifndef RAKI_AUDIOCALLBACKMANAGER_H
#include "RakiEngine/System/AudioCallbackManager/AudioCallbackManager.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_H

#ifndef _ITF_XMA2DATAPROVIDER_X360_H_
#include "engine/sound/raki/XMA2DataProvider_X360.h"
#endif //_ITF_XMA2DATAPROVIDER_X360_H_

#ifndef RAKI_XMA2STREAMSOUNDNATIVE_H
#include "RakiEngine/Sounds/_XAudio2/XMA2StreamSoundNative_X360.h"
#endif // RAKI_XMA2STREAMSOUNDNATIVE_H

#elif defined ( RAKI_PLATFORM_XBOX_SERIES )

#include "RakiEngine/Sounds/_XBoxSeries/PCMStreamSoundNative_xboxseries.h"
#include "RakiEngine/System/AudioCallbackManager/AudioCallbackManager.h"

#elif defined ( RAKI_PLATFORM_PS3 )

#ifndef RAKI_MP3STREAMSOUNDNATIVE_PS3_H
#include "RakiEngine/Sounds/_PS3/MP3StreamSoundNative_PS3.h"
#endif // RAKI_MP3STREAMSOUNDNATIVE_PS3_H

#ifndef RAKI_AUDIOCALLBACKMANAGER_H
#include "RakiEngine/System/AudioCallbackManager/AudioCallbackManager.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_H

#elif defined ( RAKI_PLATFORM_WII )

#ifndef RAKI_ADPCMSTREAMSOUNDNATIVE_WII_H
#include "RakiEngine/Sounds/_Wii/ADPCMStreamSoundNative_Wii.h"
#endif // RAKI_ADPCMSTREAMSOUNDNATIVE_WII_H

#ifndef RAKI_AUDIOCALLBACKMANAGER_WII_H
#include "RakiEngine/Specific/_Wii/AudioCallbackManager_Wii.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_WII_H

#elif defined ( RAKI_PLATFORM_PS5 )

#include "RakiEngine/Sounds/_PS5/PCMStreamSoundNative_PS5.h"
#include "RakiEngine/System/AudioCallbackManager/AudioCallbackManager.h"

#elif defined ( RAKI_PLATFORM_NINTENDO )

#include "RakiEngine/Sounds/_Nintendo/PCMStreamSoundNative_Nintendo.h"
#include "RakiEngine/System/AudioCallbackManager/AudioCallbackManager.h"

#endif

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

#if defined(ITF_PS5)
#define MAX_TIME_OFFSET ( 0.04f )
#else
#define MAX_TIME_OFFSET ( 0.02f )
#endif


//#define LOG_MUSICINSTANCE



#if defined ( ITF_WINDOWS ) || defined ( ITF_X360 ) || defined ( ITF_PS3 ) || defined ( ITF_WII ) || ( ( defined ( ITF_PS5 )||  defined ( ITF_NX )||  defined ( ITF_OUNCE )||  defined ( ITF_XBOX_SERIES )) && defined ( ITF_SUPPORT_WWISE ) )
#define USE_MUSICINSTANCE // this define must be present until we have music on all platforms...
#endif // defined ( ITF_WINDOWS ) || defined ( ITF_X360 ) || defined ( ITF_PS3 ) || defined ( ITF_WII )|| defined ( ITF_PS5 )


namespace ITF
{

///////////////////////////////////////////////////////////////////////
// MUSIC INSTANCE
///////////////////////////////////////////////////////////////////////

MusicInstance::MusicInstance( const u32 _nbChannels )
    :
     m_music( NULL )
#ifdef ITF_SUPPORT_RAKI
    , m_sound( NULL )
#endif
    , m_synchronizationDataArray( 16, SynchronizationData() ) 
    //, m_measureAdvance( 0.f )
    //, m_beatAdvance( 0.f )
    , m_metronomeType(U32_INVALID)
    , m_initialized( bfalse )
    , m_state( stopped )
    , m_waitingToReinit(bfalse)
    , m_fadeStartVolume(0.0f)
    , m_fadeEndVolume(0.0f)
    , m_fadeTotalTime(0.0f)
    , m_fadeCurrentTime(0.0f)
    , m_lastSamplePosition(0)
    , m_nextNodeToPlay(StringID::Invalid)
    , m_nextFadeTime(0.0f)
    , m_muted(bfalse)
    , m_measureSinceStart( 0 )
    , m_measureInPart( 0 )
    , m_currentVolume( 0.0f )
    , m_nextVolume(0.0f)
    , m_bVolumeChanged(bfalse)
#ifdef ITF_SUPPORT_RAKI
    , m_dataProvider( _nbChannels )
#endif
    , m_pauseSensitive(btrue)
{
#ifdef USE_MUSICINSTANCE

#if defined ( ITF_WII )
    m_sound = ( raki::ADPCMStreamSoundNative* ) newAlloc( mId_AudioEngine, raki::ADPCMStreamSoundNative )( _nbChannels );
    ITF_VERIFY(m_sound->allocateBuffers( raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_ADPCM, m_dataProvider.getNbChannels() ) ));
#elif defined ( ITF_X360 )
    m_sound = ( raki::XMA2StreamSoundNative* ) newAlloc( mId_AudioEngine, raki::XMA2StreamSoundNative );
    ITF_VERIFY(m_sound->allocateBuffers( raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_XMA2, m_dataProvider.getNbChannels() ) ));
#elif defined ( ITF_PS3 )
    m_sound = ( raki::MP3StreamSoundNative* ) newAlloc( mId_AudioEngine, raki::MP3StreamSoundNative );
    ITF_VERIFY(m_sound->allocateBuffers( raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_MPEGLAYER3, m_dataProvider.getNbChannels() ) ) );
#else
#ifdef ITF_SUPPORT_RAKI
    m_sound = ( raki::PCMStreamSoundNative* ) newAlloc( mId_AudioEngine, raki::PCMStreamSoundNative );
    ITF_VERIFY(m_sound->allocateBuffers( raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_PCM, m_dataProvider.getNbChannels() ) ));
#endif
#endif

#endif // USE_MUSICINSTANCE
}


MusicInstance::~MusicInstance()
{
    clear();
}


void MusicInstance::init( Music * _music )
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::init music 0x%x", this, _music);
#endif // LOG_MUSICINSTANCE

    m_music = _music;
    m_state = stopped;

    if ( !_music )
        return;

    m_metronomeType = _music->getMetronomeType();

    _music->setMusicInstance( this );

#ifdef USE_MUSICINSTANCE

    // JP : TEMP : until classes Music / MusicInstance / MusicManager are definitely implemented
    if ( _music->getInputList() != NULL )
    {
        const SafeArray<InputDesc>& inputList = *(_music->getInputList());
        m_inputList.resize( inputList.size() );

        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            Input* input = &m_inputList[i];
            const InputDesc& inputDesc = inputList[i];

            input->setId(inputDesc.getId());
            input->setType(inputDesc.getType());

            if ( inputDesc.getType() == InputType_F32 )
            {
                input->setValue(0.f);
            }
            else
            {
                input->setValue(static_cast<u32>(0));
            }
        }
    }
    // End TEMP

    MusicTreeInitData initData;
    initData.m_inputs = &m_inputList;
    m_musicTree.clear();
    m_musicTree.init( &initData, m_music->getMusicTreeTemplate() );

#endif // USE_MUSICINSTANCE
}


void MusicInstance::clear()
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::clear", this );
#endif // LOG_MUSICINSTANCE

#ifdef USE_MUSICINSTANCE
#ifdef ITF_SUPPORT_RAKI
    if ( m_sound )
        delete m_sound;

    m_sound = NULL;
#else
    if (AUDIO_ADAPTER && m_wwiseObjectref.isValid())
        AUDIO_ADAPTER->unregisterObjectRef(m_wwiseObjectref);
    m_wwiseObjectref.invalidate();
#endif
    m_playingHandle = SoundHandle::getInvalidHandle();

#endif // USE_MUSICINSTANCE
}


const bbool MusicInstance::canReinit()
{
    bbool canReinitAsync = btrue;

#ifdef ITF_SUPPORT_RAKI
#ifdef USE_MUSICINSTANCE
    canReinitAsync = m_dataProvider.canReinit();
#endif // USE_MUSICINSTANCE
#endif 


    return canReinitAsync;
}


void MusicInstance::reinit()
{
    ITF_ASSERT_MSG( canReinit(), "MusicInstance::reinit called while stream still active" );

#ifdef USE_MUSICINSTANCE

    m_dataProvider.reinit();
    m_synchronizationData.m_musicPart = NULL;
    m_synchronizationData.m_samplePosition = 0;
    m_synchronizationData.m_timePositionSec = 0;
    m_synchronizationData.m_soundDurationSec = 0;
    m_pendingStop.exchange( 0);

#ifndef ITF_SUPPORT_RAKI
    if (AUDIO_ADAPTER && m_wwiseObjectref.isValid())
        AUDIO_ADAPTER->unregisterObjectRef(m_wwiseObjectref);
    m_wwiseObjectref.invalidate();
#endif

    SynchronizationData reinitSynchroData;
    for(u32 i = 0; i < m_synchronizationDataArray.size(); ++i)
        m_synchronizationDataArray.write(reinitSynchroData);
#endif // USE_MUSICINSTANCE

    m_waitingToReinit = bfalse;
}

#ifdef ITF_SUPPORT_WWISE

const u64 MusicInstance::getRemainingTimeSec()
{
    if (!m_synchronizationData.m_musicPart)
        return 0;
    u64 partDurationSeconds = (u64)m_synchronizationData.m_musicPart->getWaveDuration();
    u64 posinsec = 0;
    if (m_playingHandle.isValid())
        posinsec = (u64) ( AUDIO_ADAPTER->getSourcePlayPosition(m_playingHandle) * 0.001);

    ITF_ASSERT(posinsec <= partDurationSeconds);

    return (posinsec - partDurationSeconds);
}
#endif

void MusicInstance::update(f32 _dt)
{
#ifdef USE_MUSICINSTANCE
#ifdef ITF_SUPPORT_RAKI
    const u64 samplePosition = m_sound ? m_sound->getSamplePosition() : 0;

    bool enableNewAsyncReadRequests = isPlaying() && !isStopping();
    m_dataProvider.update(enableNewAsyncReadRequests);

    if (m_sound)
    {

        m_sound->update();


        if (m_sound->isStopped() && (m_state != stopped))
        {
#ifdef LOG_MUSICINSTANCE
            RAKI_OUTPUT("0x%x MusicInstance::update calling stopVoice isStopped: %s - %d", this,
                m_sound->isStopped() ? "TRUE" : "false", m_state);
#endif // LOG_MUSICINSTANCE

            stopVoice();

        }

    }

#else
    if (!isPlaying() && !m_nextNodeToPlay.isValid())
    {
        return;
    }
    const bool hasPendingStop = m_pendingStop.exchange(0) != 0;
    if ( !AUDIO_ADAPTER->isPlaying(m_playingHandle)&& !hasPendingStop && (m_state != stopped))
    {
        #ifdef LOG_MUSICINSTANCE
                RAKI_OUTPUT("0x%x MusicInstance::update calling stopVoice isStopped:  %d", this, m_state);
        #endif // LOG_MUSICINSTANCE
        stopVoice();
        return;
    }


    bool enableNewAsyncReadRequests = isPlaying() && !isStopping();
    m_dataProvider.update(enableNewAsyncReadRequests);
    //getsample position
//     u64 remaimtime = 0;
//     if (m_playingHandle.isValid())
//         remaimtime = getRemainingTimeSec();

    u32 posinsec = 0;
     if (m_playingHandle.isValid())
     {
        posinsec = (u32)(AUDIO_ADAPTER->getSourcePlayPosition(m_playingHandle) * 0.001);
    }
    const u64 samplePosition = posinsec * DEFAULT_SAMPLING_RATE;

#endif
    // update synchronization data 

    if ( !m_music )
        return;

    const MetronomeType metronomeType = MetronomeType(m_music->getMusicTreeTemplate()->getMetronomeType());

    SynchronizationData currentSynchronizationData = m_synchronizationDataArray.read();


#ifdef ITF_SUPPORT_RAKI
    bool newData = true;
    if ( m_synchronizationData != currentSynchronizationData )
    {
        if ( currentSynchronizationData.m_musicPart )
        {
            if ( currentSynchronizationData.m_musicPart != m_synchronizationData.m_musicPart )
            {
                const u32 sizeInBars = currentSynchronizationData.m_musicPart->getSizeInBars();
                const u32 beatsPerBar = currentSynchronizationData.m_musicPart->getBeatsPerBar();

                f32 partDurationSeconds = (f32) currentSynchronizationData.m_musicPart->getSampleSize() / (f32) DEFAULT_SAMPLING_RATE;

               // RAKI_OUTPUT("METRONOME_MANAGER->setParameters  sizeinbar %d  beatsPerBar %d  partDurationSeconds %f", sizeInBars, beatsPerBar, partDurationSeconds);

                METRONOME_MANAGER->setParameters( sizeInBars, beatsPerBar, partDurationSeconds, metronomeType );

                if ( m_measureInPart )
                {
                    RAKI_OUTPUT("currentSynchronizationData.m_musicPart 0x%x != m_synchronizationData.m_musicPart 0x%x -> ++m_measureSinceStart %d", currentSynchronizationData.m_musicPart, m_synchronizationData.m_musicPart, ( m_measureSinceStart + 1 ) ); 
                    ++m_measureSinceStart;
                }

                m_measureInPart = 0;

                //LOG("MusicInstance::update m_measureSinceStart %d measureInPart %d", m_measureSinceStart, m_measureInPart);
            }
        }

        m_synchronizationData = currentSynchronizationData;
    }

#else
    bool newData = false;


    if (currentSynchronizationData.isEndNotification() == false)
    {
        if (m_synchronizationData != currentSynchronizationData)
        {
            if (currentSynchronizationData.m_musicPart && currentSynchronizationData.m_musicPart != m_synchronizationData.m_musicPart)
            {
                const u32 sizeInBars = currentSynchronizationData.m_musicPart->getSizeInBars();
                const u32 beatsPerBar = currentSynchronizationData.m_musicPart->getBeatsPerBar();

                const f32 partDurationSeconds = (f32)currentSynchronizationData.m_soundDurationSec;
                //f32 partDurationSeconds = (f32)currentSynchronizationData.m_musicPart->getSampleSize() / (f32)DEFAULT_SAMPLING_RATE;
             //   RAKI_OUTPUT("METRONOME_MANAGER->setParameters  sizeinbar %d  beatsPerBar %d  partDurationSeconds %f partDurationSeconds2 %f", sizeInBars, beatsPerBar, partDurationSeconds, (f32)currentSynchronizationData.m_soundDurationSec);

                METRONOME_MANAGER->setParameters(sizeInBars, beatsPerBar, partDurationSeconds, metronomeType);

                if (m_measureInPart)
                {
                    RAKI_OUTPUT("currentSynchronizationData.m_musicPart 0x%x != m_synchronizationData.m_musicPart 0x%x -> ++m_measureSinceStart %d", currentSynchronizationData.m_musicPart, m_synchronizationData.m_musicPart, (m_measureSinceStart + 1));

                    ++m_measureSinceStart;
                }

                m_measureInPart = 0;

              //  m_baseVolumeDb = m_nextVolumeDecibel;


            }

            m_synchronizationData = currentSynchronizationData;
            newData = true;
        }
    }
    else if (currentSynchronizationData.m_musicPart)
    {
        if (currentSynchronizationData.m_musicPart == m_synchronizationData.m_musicPart)
        {
            // detect the end of sound, try to chain with the next part

            if (const MusicPart* nextPart = getNextPart())
            {

//                 const f32 baseVolumeDb = m_baseVolumeDb;
//                 const f32 modifierVolumeDb = m_modifierVolumeDb;
//                 const f32 currentVolumeDb = m_currentVolumeDb;
//                 reinitMusicInstance();
//                 m_baseVolumeDb = baseVolumeDb;
//                 m_modifierVolumeDb = modifierVolumeDb;
//                 m_currentVolumeDb = currentVolumeDb;
                if(nextPart!= currentSynchronizationData.m_musicPart)
                   reinit();

                StringID wwiseId = nextPart->getWwiseEventGuid();

                if (!m_wwiseObjectref.isValid())
                { 

#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE) 
                    PathString_t name;
                    nextPart->getPath().getBasenameWithoutExtension(name);
                    m_wwiseObjectref = AUDIO_ADAPTER->createWwiseActor(StringConverter(name).getChar());
#else
                m_wwiseObjectref = AUDIO_ADAPTER->createWwiseActor();

#endif
                }

                AudioPlayRequest	playRequest
                (
                    AUDIO_ADAPTER->getIDFromGUID(wwiseId), 
                    m_wwiseObjectref
                );

                playRequest.m_sourcePlayPosition = btrue;
                playRequest.m_metronomeID = metronomeType;
                playRequest.m_musicPart = nextPart;
                m_playingHandle = AUDIO_ADAPTER->play(playRequest);
                if (m_muted)
                    AUDIO_ADAPTER->setVolume(m_wwiseObjectref, 0);
                else
                    AUDIO_ADAPTER->setVolume(m_wwiseObjectref, m_currentVolume);

                    


                return;
            }

        }
    }
#endif

     // calculate syncSamplePosition with time advancement but take into account odd situations with breakpoints...
    u32 syncSamplePosition = m_synchronizationData.m_samplePosition;
#ifdef ITF_SUPPORT_RAKI
    const f32 timeNow = raki::Time::getTime();
#else
    const f32 timeNow = (f32)SYSTEM_ADAPTER->getTime();
#endif

    if ( m_synchronizationData.m_musicPart 
        && ( timeNow >= m_synchronizationData.m_measurementTime )
        && ( timeNow < ( m_synchronizationData.m_measurementTime + MAX_TIME_OFFSET ))
        && newData )  
    {
        u32 sampleCorretion = u32 ( ( timeNow - m_synchronizationData.m_measurementTime ) * (f32) DEFAULT_SAMPLING_RATE );
        syncSamplePosition += sampleCorretion;

        // don't take into account rare situation when corrected pos is greater than size 
        if ( syncSamplePosition < m_synchronizationData.m_musicPart->getSampleSize() )
        {
            const u32 sizeInBars = m_synchronizationData.m_musicPart->getSizeInBars();

            const f64 samplesPerBars = (f64)m_synchronizationData.m_musicPart->getSampleSize() / (f64)sizeInBars;
            const f64 barPercentageFull = ( (f64) syncSamplePosition / samplesPerBars );

            const u32 measureInPart = (u32)(f32)barPercentageFull;

            const f32 barPercentage = f32_Modulo( (f32)barPercentageFull, 1.f );

            if ( samplesPerBars && ( measureInPart != m_measureInPart ) )
           {
                ++m_measureSinceStart;
                //RAKI_OUTPUT("!!MusicInstance::update m_measureSinceStart %d measureInPart %d (syncSamplePosition %d samplesPerBars %f)", m_measureSinceStart, measureInPart, syncSamplePosition, samplesPerBars);
            }

            m_measureInPart = measureInPart;
         //   RAKI_OUTPUT("MusicInstance::update m_measureSinceStart %d measureInPart %d (syncSamplePosition %d barPercentage %f)", m_measureSinceStart, measureInPart, syncSamplePosition, barPercentage);

            METRONOME_MANAGER->update( m_measureInPart, m_measureSinceStart, barPercentage, metronomeType );

            // Volume changed on next node ?
            if ( m_lastSamplePosition > samplePosition
                && m_nextVolume != m_currentVolume )
            {
              //  LOG("0x%x update MusicInstance:ask for a new fade", this);
                setNewFade(m_nextVolume,0.5f);
            }

        }


        //LOG("%.3f\t%.3f", timeNow, barPercentage);
    }


    // Fading ?
    if ( m_state == fading )
    {
        m_fadeCurrentTime += _dt;

        //if( m_nextNodeToPlay != StringID::Invalid
        //    && m_lastSamplePosition > samplePosition)
        //{
        //    // Apply short fade to avoid brutal volume increase
        //    m_fadeCurrentTime   = 0.0f;
        //    m_fadeTotalTime     = 0.5;
        //    m_fadeStartVolume   = getLinearVolume();
        //    m_fadeEndVolume     = 1.0f;
        //}

        // Reached end of fade ?
        if (m_fadeCurrentTime >= m_fadeTotalTime)
        {
            setLinearVolume( m_fadeEndVolume );
                
            bbool bfadeOut = m_fadeEndVolume < 0.01f;

            m_fadeCurrentTime   = 0.0f;
            m_fadeTotalTime     = 0.0f;
            m_fadeEndVolume     = 0.0f;
            m_fadeStartVolume   = 0.0f;

            // Fade out -> stop music if sound reaches 0 (linear volume)
            if ( bfadeOut )
            {
                stopVoice();
            }
            else
            {
                m_state = playing;
            }
        }
        else
        {
            f32 timeRatio   = m_fadeCurrentTime / m_fadeTotalTime;
            f32 volumeRange = m_fadeEndVolume - m_fadeStartVolume;
            f32 nextVolume  = m_fadeStartVolume + (timeRatio * volumeRange);

            setLinearVolume(nextVolume);
        }
    }
    else if (!m_waitingToReinit && m_state == stopped 
        && m_nextNodeToPlay != StringID::Invalid)
    {
        play(m_nextNodeToPlay,m_nextFadeTime,m_nextVolume);
    }

    m_lastSamplePosition = samplePosition;


    if (m_waitingToReinit && canReinit())
    {
        reinit();

        // A play has been asked, so we need to play it right away
        if ( m_nextNodeToPlay != StringID::Invalid )
        {
            play(m_nextNodeToPlay,m_nextFadeTime,m_nextVolume);
        }
    }

#endif // USE_MUSICINSTANCE
}


void MusicInstance::notifyEnd( const MusicPart* _musicPart)
{
    if (_musicPart)
    {
        SynchronizationData synchronizationData{};
        synchronizationData.setAsEndNotification(_musicPart);

        m_synchronizationDataArray.write(synchronizationData);
        m_pendingStop++;

    }
}


void MusicInstance::updateFromAudioCallback(const void* _clientData, f32 _timePositionSec, f32 _soundDurationSec)
{

    if (_clientData)
    {
        MusicPart* musicPart = (MusicPart*)_clientData;

        SynchronizationData synchronizationData((u32)(_timePositionSec * DEFAULT_SAMPLING_RATE), _timePositionSec, _soundDurationSec, musicPart);
        m_synchronizationDataArray.write(synchronizationData);
    }
    m_musicSyncPlay.audioCallback((MusicPart*)_clientData, (u32)(_timePositionSec* DEFAULT_SAMPLING_RATE));
}

void MusicInstance::play( const StringID _node, f32 _fadeTime /*= 0.0f*/, f32 _volume /*= .0f*/, const bbool _loop /*= btrue*/, const bbool _stopAndPlay /*= bfalse*/ )
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::play <%s> m_music 0x%x getMusicTreeTemplate 0x%x _volume %.3f", this, _node.getDebugString(), m_music, m_music->getMusicTreeTemplate(), _volume );
#endif // LOG_MUSICINSTANCE

    //manage stop and play case
    if( _stopAndPlay )
    {
        //if we want to play a different music node
        if( m_lastPlayedNode != _node )
        {
            //stop current node with wanted fade out time
            stop(_fadeTime);
            //reset fade time because we don't want the next play to fade in
            _fadeTime = 0.f;
        }
        else
            // Don't do anything if we're trying to play the same node in stopAndPlay condition
            // as we will already play it ASAP
            return;
        //continue in order to play wanted node
    }
    if ( m_lastPlayedNode != _node )
        m_dataProvider.musicInstanceHasRequestedNewPlay();

    m_lastPlayedNode = _node;

    m_musicTree.playNode( _node );
    m_nextVolume = _volume;
#ifdef ITF_SUPPORT_RAKI

    if (m_sound != nullptr)
        m_sound->resetRequestEndAfterSubmittedBuffers();
#endif

    // Fade ?
    if ( !F32_ALMOST_EQUALS( _fadeTime, 0.0f, MTH_EPSILON )
        && ( !isPlaying() || isStopping() ) )
    {
#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT( "0x%x MusicInstance::play setNewFade _fadeTime %.3f m_state %d endVolume %.3f m_waitingToReinit: %s", 
            this, _fadeTime, m_state, m_fadeEndVolume, m_waitingToReinit ? "true" : "false" );
#endif // LOG_MUSICINSTANCE

        setNewFade( _volume, _fadeTime );
    }

    // Play during a without fade in during a fade out ?
    // Or  reinit has not been done yet, so play will have wait a bit
    if ( ( F32_ALMOST_EQUALS( _fadeTime, 0.f, MTH_EPSILON ) && isStopping() )
        || m_waitingToReinit )
    {
#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT( "0x%x MusicInstance::play setting next node to play _fadeTime %.3f m_state %d endVolume %.3f m_waitingToReinit: %s", 
            this, _fadeTime, m_state, m_fadeEndVolume, m_waitingToReinit ? "true" : "false" );
#endif // LOG_MUSICINSTANCE

        m_nextNodeToPlay    = _node;
        m_nextFadeTime      = _fadeTime;

        return;
    }

    if ( m_dataProvider.isStopping() )
    {
        m_nextNodeToPlay    = _node;
        m_nextFadeTime      = _fadeTime;

        m_waitingToReinit = true;

        return;
    }

#ifdef ITF_SUPPORT_RAKI

#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::play hasValidStream() %s - isPlaying() %s m_sound->isPlaying() %d", 
        this, m_dataProvider.hasValidStream() ? "TRUE" : "false", isPlaying() ? "TRUE" : "false", m_sound ? m_sound->isPlaying() : false );
#endif // LOG_MUSICINSTANCE


    if ( m_dataProvider.hasValidStream() /*|| isPlaying()*/ || ( m_sound && m_sound->isPlaying() ) )
    {
#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT( "0x%x MusicInstance::play hasValidStream() || isPlaying() -> not doing anything _fadeTime %.3f m_state %d endVolume %.3f m_waitingToReinit: %s", 
            this, _fadeTime, m_state, m_fadeEndVolume, m_waitingToReinit ? "true" : "false" );
#endif // LOG_MUSICINSTANCE

        return;
    }
#else
    if (AUDIO_ADAPTER->isPlaying(m_playingHandle))
        return;

#endif

#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::play really playing now _fadeTime %.3f m_state %d endVolume %.3f m_waitingToReinit: %s", 
        this, _fadeTime, m_state, m_fadeEndVolume, m_waitingToReinit ? "true" : "false" );
#endif // LOG_MUSICINSTANCE

#ifdef USE_MUSICINSTANCE
    // Reset musicTree so that all nodes are correctly pointing to valid leafs
    m_musicTree.reset();

    m_musicTree.update( 0.f, m_inputList );

    MusicTreeResult * result = m_musicTree.getResult();
#else
    MusicTreeResult* result = nullptr;
#endif

    if ( result )
    {
        // Resource is not ready yet, then play it ASAP
        if ( !m_music->isPhysicallyLoaded() )
        {
#ifdef LOG_MUSICINSTANCE
            RAKI_OUTPUT("0x%x MusicInstance::play result->m_musicLeaf.m_partName <%s> returning coz !m_music->isPhysicallyLoaded()", this, result->m_musicLeaf.m_partName.getDebugString());
#endif // LOG_MUSICINSTANCE

            m_nextNodeToPlay    = _node;
            m_nextFadeTime      = _fadeTime;

            return;
        }

        const MusicPart * part = m_music->getMusicPart( result->m_musicLeaf.m_partName );

#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT("0x%x MusicInstance::play result->m_musicLeaf.m_partName <%s> part 0x%x", this, result->m_musicLeaf.m_partName.getDebugString(), part );
#endif // LOG_MUSICINSTANCE

        if ( part )
        {
#ifdef ITF_SUPPORT_RAKI
            ITF_ASSERT( m_sound );

            ITF_ASSERT_MSG( part->getWaveFile(), "MusicInstance::play: problem loading music part <%s>", part->getPath().getString8().cStr() );

            if ( !part->getWaveFile() )
                return;

            m_dataProvider.init( this, part, m_sound );
#else
            if (!part->getWwiseEventGuid().isValid())
            {
                ITF_ASSERT_MSG(0, "MusicInstance::play: Invalid WWise Id problem playing music part <%s>", part->getPath().getString8().cStr());
                return;
            }
                
            m_dataProvider.init(this, part, NULL);
#endif


#ifdef USE_MUSICINSTANCE
#ifdef ITF_SUPPORT_RAKI
            if ( !m_sound->hasValidLowLevelVoice())
            {
                const char fmt[] = "fmt ";
                const bool isMusic = true;

                raki::Format format( isMusic );
#ifdef RAKI_PLATFORM_WII            
                format.setFormat( (WAVEFORMATEX*const)part->getWaveFile()->getChunkData( fmt ) );
#else // RAKI_PLATFORM_WII            
                format.setData( (WAVEFORMATEX*const)part->getWaveFile()->getChunkData( fmt ), part->getWaveFile()->getChunkSize( fmt ) );
#endif // RAKI_PLATFORM_WII            
                m_sound->createVoice( &format );


                m_initialized = btrue;
            }

            m_sound->prepare( &m_dataProvider );
            m_sound->getStreamBufferInfo()->setStreamUpdateClient( this );
#else
#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE)
            PathString_t name;
            part->getPath().getBasenameWithoutExtension(name);
            m_wwiseObjectref = AUDIO_ADAPTER->createWwiseActor(StringConverter(name).getChar());
#else
            m_wwiseObjectref = AUDIO_ADAPTER->createWwiseActor();

#endif
#endif

            if ( m_state != fading ) 
            {
                setLinearVolume( _volume );
                m_state = playing;
            }
            else
            {
                setLinearVolume( 0.f );
            }

#ifdef ITF_SUPPORT_RAKI
            m_sound->updateVolume( true );
            m_sound->start();
#else


            StringID wwiseId = part->getWwiseEventGuid();


            AudioPlayRequest	playRequest
            (
                AUDIO_ADAPTER->getIDFromGUID(wwiseId),
                m_wwiseObjectref
            );

            playRequest.m_sourcePlayPosition = btrue;
            playRequest.m_metronomeID = MetronomeType(m_music->getMusicTreeTemplate()->getMetronomeType());
            playRequest.m_musicPart = part;
            playRequest.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;

            m_playingHandle = AUDIO_ADAPTER->play(playRequest);
            if (m_muted)
                AUDIO_ADAPTER->setVolume(m_wwiseObjectref, 0);
            else
                AUDIO_ADAPTER->setVolume(m_wwiseObjectref, m_currentVolume);

#endif

            m_measureSinceStart = 0;
            m_measureInPart = 0;

            // Start metronome
            METRONOME_MANAGER->start(MetronomeType(m_music->getMusicTreeTemplate()->getMetronomeType()));

#endif // USE_MUSICINSTANCE

            // Play new music with requested volume
            // As we played next node, reset waiting values
            m_nextNodeToPlay    = StringID::Invalid;
            m_nextFadeTime      = 0.0f;
        }
    }
}


void MusicInstance::stopVoice()
{
#ifdef LOG_MUSICINSTANCE
    LOG( "0x%x MusicInstance::stopVoice", this );
#endif // LOG_MUSICINSTANCE

#ifdef USE_MUSICINSTANCE
    {
#ifdef ITF_SUPPORT_RAKI
        raki::AudioCallbackDisabler audioCallbackDisabler;
#endif

        m_musicSyncPlay.resetMusicAndForceSyncPlays();
#ifdef ITF_SUPPORT_RAKI
        m_sound->resetPendingSyncStart();
#endif
    }
#ifdef ITF_SUPPORT_RAKI
    m_sound->stop();

    m_sound->destroyVoice();
#else
     if(m_playingHandle.isValid())
        AUDIO_ADAPTER->stop(m_playingHandle);
    m_playingHandle.invalidate();

#endif

#endif // USE_MUSICINSTANCE

    // Reset metronome
    if ( m_music->getMusicTreeTemplate() != NULL )
        METRONOME_MANAGER->reset(MetronomeType(m_music->getMusicTreeTemplate()->getMetronomeType()));

    m_state = stopped;
    m_waitingToReinit = btrue;

    m_lastPlayedNode = StringID::Invalid;
}


void MusicInstance::stop(f32 _fadeTime /*= 0.0f*/)
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::stop, fadeTime : %f", this, _fadeTime );
#endif // LOG_MUSICINSTANCE

    if ( isPlaying())
    {
#ifdef LOG_MUSICINSTANCE
        LOG( "0x%x MusicInstance::stop entering isPlaying", this );
#endif // LOG_MUSICINSTANCE
        // Start fading
        if ( _fadeTime > 0.0f )
        {
            if (m_state != fading)
            {
              //  LOG("0x%x MusicInstance::setNewFade", this);
                if (!isStopping())
                    setNewFade(0.f, _fadeTime);
            }

        }
        // No fade, then launch stop process
        else
        {
            //LOG("0x%x MusicInstance::stop voice", this);
            stopVoice();
        }

    }
    // else : don't do anything as no music is currently playing

    // Reset next node to play
    m_nextNodeToPlay    = StringID::Invalid;
    m_nextFadeTime      = 0.0f;
    m_nextVolume        = 0.0f;
    m_pendingStop.exchange( 0);
}


bbool MusicInstance::isPlaying() const
{
    if (m_nextNodeToPlay.isValid())
        return btrue;

//     if (m_pendingStop)
//         return btrue;

    return m_state != stopped;
}


bbool MusicInstance::isStopping()
{
    return ( m_state == fading ) && F32_ALMOST_EQUALS( m_fadeEndVolume, 0.0f, MTH_EPSILON ) ;
}


void MusicInstance::setNewFade( f32 _destVolume, f32 _fadeTime )
{
    ITF_ASSERT( _fadeTime > 0.f );
    ITF_ASSERT( _destVolume >= 0.f );
    ITF_ASSERT( _destVolume <= 1.f );
    if (m_state == stopped)
        return;

    m_fadeCurrentTime   = 0.0f;
    m_fadeTotalTime     = _fadeTime;
    m_fadeStartVolume   = getLinearVolume();
    m_fadeEndVolume     = _destVolume;

    m_state = fading;
}


void MusicInstance::pause()
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::pause", this );
#endif // LOG_MUSICINSTANCE

#ifdef USE_MUSICINSTANCE
#ifdef ITF_SUPPORT_RAKI
    if ( m_sound )
        m_sound->pause();

#else
    if (m_playingHandle.isValid())
        AUDIO_ADAPTER->pause(m_playingHandle);
    else
    {
#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT("0x%x MusicInstance not pause because of invalid handl", this);
#endif // LOG_MUSICINSTANCE

    }


#endif
#endif // USE_MUSICINSTANCE
}


void MusicInstance::resume()
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::resume", this );
#endif // LOG_MUSICINSTANCE

#ifdef USE_MUSICINSTANCE
#ifdef ITF_SUPPORT_RAKI
    if ( m_sound )
        m_sound->resume();
#else
    if (m_playingHandle.isValid())
        AUDIO_ADAPTER->resume(m_playingHandle);

#endif
#endif // USE_MUSICINSTANCE
}


void MusicInstance::setLinearVolume( const f32 _linearVolume )
{
#ifdef LOG_MUSICINSTANCE
//     if ( m_currentVolume != _linearVolume )
//     {
//        RAKI_OUTPUT( "0x%x MusicInstance::setLinearVolume %.3f (%s)", this, _linearVolume, m_muted ? "MUTED" : "not muted" );
//     }
#endif // LOG_MUSICINSTANCE

#ifdef USE_MUSICINSTANCE
#ifdef ITF_SUPPORT_RAKI
    if ( m_sound )
        m_sound->setLinearVolume( m_muted ? 0.0f : _linearVolume );
#else
    if(m_muted)
        AUDIO_ADAPTER->setVolume(m_wwiseObjectref, 0);
    else
        AUDIO_ADAPTER->setVolume(m_wwiseObjectref, _linearVolume);

        
#endif

    m_currentVolume = _linearVolume;
#endif // USE_MUSICINSTANCE
}

f32 MusicInstance::getLinearVolume()
{
#ifdef USE_MUSICINSTANCE
#ifdef ITF_SUPPORT_RAKI
    if ( m_sound )
        return m_currentVolume;
#endif
#endif // USE_MUSICINSTANCE

    return m_currentVolume;
}

void MusicInstance::mute(bbool _mute)
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::mute %s", this, _mute ? "TRUE" : "FALSE" );
#endif // LOG_MUSICINSTANCE

#ifdef USE_MUSICINSTANCE
#ifdef ITF_SUPPORT_RAKI
    if ( m_sound )
    {

        if ( !_mute )
        {
            m_sound->setLinearVolume( 0.0f );
            f32 endVolume   = m_currentVolume;
            m_currentVolume = 0.0f;
            setNewFade( endVolume, 1.0f );
        }
        else
        {
            m_sound->setLinearVolume( 0.0f );
        }
    }
#else
    if (!_mute)
    {
        AUDIO_ADAPTER->setVolume(m_wwiseObjectref, 0);
        f32 endVolume = m_currentVolume;
        m_currentVolume = 0.0f;
        setNewFade(endVolume, 1.0f);
    }
    else
    {
        AUDIO_ADAPTER->setVolume(m_wwiseObjectref, 0);
    }

#endif
    m_muted = _mute;
#endif // USE_MUSICINSTANCE
}


const MusicPart * MusicInstance::getNextPart() 
{
    m_musicTree.update( 0.f, m_inputList );

    MusicTreeResult * result = m_musicTree.getResult();

    if ( result && !result->m_isfinished )
    {
       const  MusicPart * part = m_music ? m_music->getMusicPart( result->m_musicLeaf.m_partName ) : NULL;

#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT( "0x%x MusicInstance::getNextPart m_musicLeaf.m_partName <%s> part 0x%x", this, result->m_musicLeaf.m_partName.getDebugString(), part);
#endif // LOG_MUSICINSTANCE

        if ( part )
        {
            if ( ( (MusicPart*)part)->isPhysicallyLoaded() )
                return part;

            ITF_ASSERT_MSG( 0, "MusicInstance::getNextPart called on non loaded part!" );
        }

        return NULL;
    }

#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstance::getNextPart returning NULL ", this );
#endif // LOG_MUSICINSTANCE

    return NULL;
}

#ifdef ITF_SUPPORT_RAKI
void MusicInstance::updateStreamPosition( const void * _clientData, const u64 _samplePosition )
{
    if ( _clientData )
    {
        const MusicPart * musicPart = ( const MusicPart* ) _clientData;

        SynchronizationData synchronizationData( (u32)_samplePosition, musicPart );
        m_synchronizationDataArray.write( synchronizationData );

        //RAKI_OUTPUT("%.3f\t%d", raki::Time::getTime(), (u32)_samplePosition );
    }

    m_musicSyncPlay.audioCallback( (MusicPart*)_clientData, (u32) _samplePosition );
}
#endif

void MusicInstance::notifySoundEnded(const void* _clientData)
{
    if (_clientData)
    {
        MusicPart* musicPart = (MusicPart*)_clientData;

        SynchronizationData synchronizationData{};
        synchronizationData.setAsEndNotification(musicPart);

        m_synchronizationDataArray.write(synchronizationData);
        m_pendingStop++;
    }
}

StringID MusicInstance::debugGetMusicName()
{
    if ( m_synchronizationData.m_musicPart )
    {
        return m_synchronizationData.m_musicPart->getName();
    }

    return ITF_GET_STRINGID_CRC(none,2748391529);
}

bbool MusicInstance::isMusic()
{
    return btrue;
}

const u32 MusicInstance::getNbChannels()
{
#ifdef ITF_SUPPORT_RAKI
    return m_dataProvider.getNbChannels();
#else
    return 2;
#endif
}

void MusicInstance::updateVolume()
{
#ifdef ITF_SUPPORT_RAKI
    if ( m_sound )
        m_sound->updateVolume();
#else
   // AUDIO_ADAPTER->update(); //???

#endif
}

void MusicInstance::setRoutingGroup( raki::RoutingGroup * _routingGroup )
{
#ifdef ITF_SUPPORT_RAKI
    if ( m_sound )
        m_sound->setRoutingGroup( _routingGroup );
#else

#endif
}

f32 MusicInstance::getPosInSec()
{
    f32 samplePositionsec = 0;
    if (m_playingHandle.isValid())
    {
        samplePositionsec = (f32)(AUDIO_ADAPTER->getSourcePlayPosition(m_playingHandle) * 0.001f);
    }
    return samplePositionsec;
}


///////////////////////////////////////////////////////////////////////
// MUSIC INSTANCE SIMPLE
///////////////////////////////////////////////////////////////////////

MusicInstanceSimple::~MusicInstanceSimple()
{
    clear();
}

void MusicInstanceSimple::init( const MusicSimple * _music )
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstanceSimple::init music 0x%x", this, _music);
#endif // LOG_MUSICINSTANCE

    ITF_ASSERT( _music );

    m_music = _music;
    m_state = stopped;
}


void MusicInstanceSimple::update( f32 _dt )
{
#ifdef USE_MUSICINSTANCE

#ifdef ITF_SUPPORT_RAKI
    const u64 samplePosition = m_sound ? m_sound->getSamplePosition() : 0;

    bool enableNewAsyncReadRequests = isPlaying() && !isStopping();
    m_dataProvider.update( enableNewAsyncReadRequests );

    if ( m_sound )
    {
        m_sound->update();

        if ( m_sound->isStopped() && ( m_state != stopped ) )
        {
#ifdef LOG_MUSICINSTANCE
            RAKI_OUTPUT( "0x%x MusicInstanceSimple::update calling stopVoice", this );
#endif // LOG_MUSICINSTANCE

            stopVoice();
        }
    }
#else
    bool enableNewAsyncReadRequests = isPlaying() && !isStopping();
    m_dataProvider.update(enableNewAsyncReadRequests);
    u64 samplePosition = 0;
    if (m_playingHandle.isValid())
        samplePosition = (u64)(AUDIO_ADAPTER->getSourcePlayPosition(m_playingHandle) * 0.001 * DEFAULT_SAMPLING_RATE);


#endif

    // Fading ?
    if ( m_state == fading )
    {
        m_fadeCurrentTime += _dt;

        if( m_nextNodeToPlay != StringID::Invalid
            && m_lastSamplePosition > samplePosition)
        {
            // Apply short fade to avoid brutal volume increase
            m_fadeCurrentTime   = 0.0f;
            m_fadeTotalTime     = 0.5;
            m_fadeStartVolume   = getLinearVolume();
            m_fadeEndVolume     = 1.0f;
        }

        // Reached end of fade ?
        if (m_fadeCurrentTime >= m_fadeTotalTime)
        {
            setLinearVolume( m_fadeEndVolume );

            bbool bfadeOut = m_fadeEndVolume < 0.01f;

            m_fadeCurrentTime   = 0.0f;
            m_fadeTotalTime     = 0.0f;
            m_fadeEndVolume     = 0.0f;
            m_fadeStartVolume   = 0.0f;

            // Fade out -> stop music if sound reaches 0 (linear volume)
            if ( bfadeOut )
            {
                stopVoice();
            }
            else
            {
                m_state = playing;
            }
        }
        else
        {
            f32 timeRatio   = m_fadeCurrentTime / m_fadeTotalTime;
            f32 volumeRange = m_fadeEndVolume - m_fadeStartVolume;
            f32 nextVolume  = m_fadeStartVolume + (timeRatio * volumeRange);

            setLinearVolume(nextVolume);
        }
    }

    m_lastSamplePosition = samplePosition;

    if (m_waitingToReinit && canReinit())
    {
        reinit();

        // A play has been asked, so we need to play it right away
        if ( m_nextNodeToPlay != StringID::Invalid )
        {
            play(m_nextNodeToPlay,m_nextFadeTime);
        }
    }

#endif // USE_MUSICINSTANCE
}

void MusicInstanceSimple::play( const StringID _node, f32 _fadeTime /*= 0.0f*/, f32 _volume /*= .0f*/, const bbool _loop /*= btrue*/, const bbool _stopAndPlay /*= bfalse*/ )
{
#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstanceSimple::play <%s> _volume %.3f", this, _node.getDebugString(), _volume );
#endif // LOG_MUSICINSTANCE

    // get looping value
    m_isLooping = _loop;
//#ifdef ITF_SUPPORT_RAKI
    if ( m_lastPlayedNode != _node )
        m_dataProvider.musicInstanceHasRequestedNewPlay();
//#endif

    m_lastPlayedNode = _node;

    // Fade ?
    if ( !F32_ALMOST_EQUALS( _fadeTime, 0.0f, MTH_EPSILON )
        && ( !isPlaying() || isStopping() ) )
    {
#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT( "0x%x MusicInstanceSimple::play setNewFade _fadeTime %.3f m_state %d endVolume %.3f m_waitingToReinit: %s", 
            this, _fadeTime, m_state, m_fadeEndVolume, m_waitingToReinit ? "true" : "false" );
#endif // LOG_MUSICINSTANCE

        setNewFade( _volume, _fadeTime );
    }

    // Play during a without fade in during a fade out ?
    // Or  reinit has not been done yet, so play will have wait a bit
    if ( ( F32_ALMOST_EQUALS( _fadeTime, 0.f, MTH_EPSILON ) && isStopping() )
        || m_waitingToReinit )
    {
#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT( "0x%x MusicInstanceSimple::play setting next node to play _fadeTime %.3f m_state %d endVolume %.3f m_waitingToReinit: %s", 
            this, _fadeTime, m_state, m_fadeEndVolume, m_waitingToReinit ? "true" : "false" );
#endif // LOG_MUSICINSTANCE

        m_nextNodeToPlay    = _node;
        m_nextFadeTime      = _fadeTime;
        m_nextVolume        = _volume;
        return;
    }

//#ifdef ITF_SUPPORT_RAKI
    if ( m_dataProvider.isStopping() )
    {
        m_nextNodeToPlay    = _node;
        m_nextFadeTime      = _fadeTime;
        m_nextVolume        = _volume;

        m_waitingToReinit = true;

        return;
    }
//#endif

#ifdef USE_MUSICINSTANCE

    bool validstream = m_dataProvider.hasValidStream();

    if (validstream && isPlaying() )
    {
#ifdef LOG_MUSICINSTANCE
        RAKI_OUTPUT( "0x%x MusicInstanceSimple::play hasValidStream() && isPlaying() -> not doing anything _fadeTime %.3f m_state %d endVolume %.3f m_waitingToReinit: %s", 
            this, _fadeTime, m_state, m_fadeEndVolume, m_waitingToReinit ? "true" : "false" );
#endif // LOG_MUSICINSTANCE

        return;
    }

    const MusicPart * part = m_music->getMusicPart( _node );

#ifdef LOG_MUSICINSTANCE
    RAKI_OUTPUT( "0x%x MusicInstanceSimple::play really playing now _fadeTime %.3f m_state %d endVolume %.3f m_waitingToReinit: %s part 0x%x", 
        this, _fadeTime, m_state, m_fadeEndVolume, m_waitingToReinit ? "true" : "false", part );
#endif // LOG_MUSICINSTANCE

    if ( part )
    {
#ifdef ITF_SUPPORT_RAKI
        ITF_ASSERT( m_sound );
        ITF_ASSERT_MSG( part->getWaveFile(), "MusicInstanceSimple::play: problem loading stream part <%s>", part->getPath().getString8().cStr() );
        RAKI_ASSERT( ( (MusicPart*) part )->isPhysicallyLoaded() );

        if ( !part->getWaveFile() )
            return;

        m_dataProvider.init( this, part, m_sound );
#else
        if (!part->getWwiseEventGuid().isValid())
        {
            ITF_ASSERT_MSG(0, "MusicInstanceSimple::play: Invalid WWise Id problem playing music part <%s>", part->getPath().getString8().cStr());
            return;
        }

        m_dataProvider.init(this, part, NULL);
#endif


#ifdef ITF_SUPPORT_RAKI
        if ( !m_sound->hasValidLowLevelVoice() )
        {
            const char fmt[] = "fmt ";
            const bool isMusic = false;
            raki::Format format( isMusic );
#ifdef RAKI_PLATFORM_WII            
            format.setFormat( (WAVEFORMATEX*const)part->getWaveFile()->getChunkData( fmt ) );
#else // RAKI_PLATFORM_WII            
            format.setData( (WAVEFORMATEX*const)part->getWaveFile()->getChunkData( fmt ), part->getWaveFile()->getChunkSize( fmt ) );
#endif // RAKI_PLATFORM_WII            
            m_sound->createVoice( &format );

            m_initialized = btrue;
        }

        m_sound->prepare( &m_dataProvider );
        m_sound->getStreamBufferInfo()->setStreamUpdateClient( this );
#else
#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE)
        PathString_t name;
        part->getPath().getBasenameWithoutExtension(name);
        m_wwiseObjectref = AUDIO_ADAPTER->createWwiseActor(StringConverter(name).getChar());
#else
        m_wwiseObjectref = AUDIO_ADAPTER->createWwiseActor();

#endif
#endif

        if ( m_state != fading ) 
        {
            setLinearVolume( _volume );
            m_state = playing;
        }
        else
        {
            setLinearVolume( 0.f );
        }
#ifdef ITF_SUPPORT_RAKI
        m_sound->updateVolume( true );
        m_sound->start();
#else
        StringID wwiseId = part->getWwiseEventGuid();

        AudioPlayRequest	playRequest
        (
            AUDIO_ADAPTER->getIDFromGUID(wwiseId),
            m_wwiseObjectref
        );

        playRequest.m_sourcePlayPosition = btrue;
        playRequest.m_metronomeID = METRONOME_TYPE_INVALID;
        playRequest.m_musicPart = part;
        playRequest.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;


        m_playingHandle = AUDIO_ADAPTER->play(playRequest);

#endif
    }
#endif // USE_MUSICINSTANCE

}


 const MusicPart * MusicInstanceSimple::getNextPart() 
{
    if ( m_isLooping && m_music && m_music->getFirstMusicPart() )
    {
        ITF_ASSERT_MSG( ( (MusicPart*) m_music->getFirstMusicPart() )->isPhysicallyLoaded(), "MusicInstanceSimple::getNextPart called on non loaded part!");

        if ( (MusicPart*) m_music->getFirstMusicPart() )
            return m_music->getFirstMusicPart();
    }

    return NULL;
}


bbool MusicInstanceSimple::isMusic()
{
    return bfalse;
}



} // namespace ITF
