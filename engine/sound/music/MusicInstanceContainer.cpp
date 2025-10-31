#include "precompiled_engine.h"



#ifndef _ITF_MUSICINSTANCECONTAINER_H_
#include "engine/sound/Music/MusicInstanceContainer.h"
#endif // _ITF_MUSICINSTANCECONTAINER_H_




#ifndef _ITF_MUSICINSTANCE_H_
#include "engine/sound/Music/MusicInstance.h"
#endif // _ITF_MUSICINSTANCE_H_

#ifndef _ITF_MUSIC_H_
#include "engine/sound/Music/Music.h"
#endif // _ITF_MUSIC_H_

#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_

#ifdef ITF_SUPPORT_RAKI
#ifndef RAKI_WAVEFILE_H
#include "RakiEngine/Data/WaveFile/WaveFile.h"
#endif // RAKI_WAVEFILE_H
#endif


#include "engine/AdaptersInterfaces/SoundAdapter.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
#ifndef _ITF_SOFTWAREBUS_H_
#include "engine/sound/routing/SoftwareBus.h"
#endif // _ITF_SOFTWAREBUS_H_



#ifdef ITF_SUPPORT_RAKI
#define MUSICINSTANCECONTAINER_MUSIC_INSTANCE_POOL_SIZE                 2
#define MUSICINSTANCECONTAINER_STEREO_AMBIANCE_INSTANCE_POOL_SIZE       2
#define MUSICINSTANCECONTAINER_MONO_VOICE_INSTANCE_POOL_SIZE            2
#else
#define MUSICINSTANCECONTAINER_MUSIC_INSTANCE_POOL_SIZE                 2
#define MUSICINSTANCECONTAINER_STEREO_AMBIANCE_INSTANCE_POOL_SIZE       4
#define MUSICINSTANCECONTAINER_MONO_VOICE_INSTANCE_POOL_SIZE            2
#endif





namespace ITF
{


MusicInstanceContainer::MusicInstanceContainer()
    : m_nextHandle( 1 )
{
    int i;

    for ( i = 0 ; i < MUSICINSTANCECONTAINER_MUSIC_INSTANCE_POOL_SIZE ; ++ i )
    {
        MusicInstance * musicInstance = ( MusicInstance* ) newAlloc( mId_AudioEngine, MusicInstance )( 2 );
        
        m_freeMusicInstanceArray.push_back( musicInstance );
    }

    for ( i = 0 ; i < MUSICINSTANCECONTAINER_STEREO_AMBIANCE_INSTANCE_POOL_SIZE ; ++ i )
    {
        MusicInstanceSimple * stereoAmbianceInstance = ( MusicInstanceSimple* ) newAlloc( mId_AudioEngine, MusicInstanceSimple )( 2 );

        m_freeStereoAmbianceInstanceArray.push_back( stereoAmbianceInstance );
    }

    for ( i = 0 ; i < MUSICINSTANCECONTAINER_MONO_VOICE_INSTANCE_POOL_SIZE ; ++ i )
    {
        MusicInstanceSimple * monoVoiceInstance = ( MusicInstanceSimple* ) newAlloc( mId_AudioEngine, MusicInstanceSimple )( 1 );

        m_freeMonoVoiceInstanceArray.push_back( monoVoiceInstance );
    }
}


MusicInstanceContainer::~MusicInstanceContainer()
{
    if ( m_musicInstanceArray.size() )
    {
        // first stop instances 
        for ( MusicInstanceStructVector::iterator it = m_musicInstanceArray.begin() ; it != m_musicInstanceArray.end() ; ++it )
            it->m_musicInstance->stop();
#ifdef ITF_SUPPORT_RAKI
        // wait for callbacks to acknowledge the stop 
        raki::Thread::sleep( 20 );
#endif

        while ( m_musicInstanceArray.size() )
        {
            destroyMusicInstance( m_musicInstanceArray[ 0 ].m_handle );
        }
    }

    // free music instances 

    while ( m_freeMusicInstanceArray.size() )
    {
        MusicInstance * musicInstance = m_freeMusicInstanceArray[ 0 ];
        
        int count = 0;
        while (!musicInstance->canReinit())
        {
#ifdef ITF_SUPPORT_RAKI
            raki::Thread::sleep( 5 );
#endif
            musicInstance->update(1.f/60.f);
            ++count;
            if ( count > 200 )
            { 
                break;
            }
        }

        if ( musicInstance->canReinit() )
            delete musicInstance;

        m_freeMusicInstanceArray.erase( m_freeMusicInstanceArray.begin() );
    }


    // free stereo ambiance instances 

    while ( m_freeStereoAmbianceInstanceArray.size() )
    {
        MusicInstanceSimple * stereoAmbianceInstance = m_freeStereoAmbianceInstanceArray[ 0 ];

        int count = 0;
        while ( !stereoAmbianceInstance->canReinit() )
        {
#ifdef ITF_SUPPORT_RAKI
            raki::Thread::sleep(5);
#endif
            stereoAmbianceInstance->update(1.f/60.f);
            ++count;
            if ( count > 200 )
            {
                break;
            }
        }

        if ( stereoAmbianceInstance->canReinit() )
            delete stereoAmbianceInstance;

        m_freeStereoAmbianceInstanceArray.erase( m_freeStereoAmbianceInstanceArray.begin() );
    }


    // free mono voice instances 

    while ( m_freeMonoVoiceInstanceArray.size() )
    {
        MusicInstanceSimple * monoVoiceInstance = m_freeMonoVoiceInstanceArray[ 0 ];

        int count = 0;
        while ( !monoVoiceInstance->canReinit() )
        {
#ifdef ITF_SUPPORT_RAKI
            raki::Thread::sleep(5);
#endif
            monoVoiceInstance->update(1.f/60.f); // arbitrary dt...
            ++count;
            if ( count > 200 )
            {
                break;
            }
        }


        if ( monoVoiceInstance->canReinit() )
            delete monoVoiceInstance;

        m_freeMonoVoiceInstanceArray.erase( m_freeMonoVoiceInstanceArray.begin() );
    }
}


void MusicInstanceContainer::update( f32 _dt )
{
    u32 u;

    for ( u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        m_musicInstanceArray[ u ].m_musicInstance->update( _dt );

        m_musicInstanceArray[ u ].m_musicInstance->updateVolume();
    }

    for ( u = 0 ; u < m_freeMusicInstanceArray.size() ; ++u )
    {
        m_freeMusicInstanceArray[ u ]->update( _dt );
    }

    for ( u = 0 ; u < m_freeStereoAmbianceInstanceArray.size() ; ++u )
    {
        m_freeStereoAmbianceInstanceArray[ u ]->update( _dt );
    }

    for ( u = 0 ; u < m_freeMonoVoiceInstanceArray.size() ; ++u )
    {
        m_freeMonoVoiceInstanceArray[ u ]->update( _dt );
    }
}


u32 MusicInstanceContainer::createMusicInstance( Music * _music, const StringID _busName )
{
    ITF_VECTOR<MusicInstance*>::iterator it;

    for ( it = m_freeMusicInstanceArray.begin() ; it != m_freeMusicInstanceArray.end() ; ++it )
    {
        MusicInstance * musicInstance = *it;

        if ( musicInstance->canReinit() )
        {
            m_freeMusicInstanceArray.erase( it );

            musicInstance->reinit();

            musicInstance->init( _music );

            musicInstance->resetMuteFlag();

#if defined(ITF_SUPPORT_RAKI)
            SoftwareBus * const bus = SOUND_ADAPTER->getBus( _busName );
            musicInstance->setRoutingGroup(bus);
#else
            SoftwareBus* const bus = AUDIO_ADAPTER->getBus(_busName);

#endif




            //set pause sensitive flag from bus definition
            bbool pauseSensitive = btrue;
            if( bus )
            {
                const BusDef * const busDefinition = bus->getBusDef();
                if( busDefinition )
                {
                    pauseSensitive = busDefinition->m_pauseSensitive;
                }
            }

            musicInstance->setPauseSensitive(pauseSensitive);

            HandleInstanceStruct handleInstanceStruct( m_nextHandle, musicInstance );

            m_musicInstanceArray.push_back( handleInstanceStruct );

            ++m_nextHandle;

            return handleInstanceStruct.m_handle;
        }
    }

    ITF_ASSERT_MSG( 0, "MusicInstanceContainer::createMusicInstance out of instances!");
    ITF_ASSERT( 0 );

    return 0;
}


u32 MusicInstanceContainer::createMusicInstanceSimple( MusicSimple * _music, const StringID _busName )
{
    ITF_VECTOR<MusicInstanceSimple*>::iterator it;

    ITF_ASSERT( _music );
    ITF_ASSERT( _music->getFirstMusicPart() );
#if defined(ITF_SUPPORT_RAKI)
    ITF_ASSERT( _music->getFirstMusicPart()->getWaveFile() );
    ITF_ASSERT_MSG( _music->getFirstMusicPart()->getWaveFile(), "MusicInstanceContainer::createMusicInstanceSimple: problem loading stream part <%s>", _music->getFirstMusicPart()->getPath().getString8().cStr() );
    if (!_music->getFirstMusicPart()->getWaveFile())
        return U32_INVALID;

#else
    if (  !_music->getFirstMusicPart()->getWwiseEventGuid().isValid())
        return U32_INVALID;
#endif

    u32 channel = 2;
#if defined(ITF_SUPPORT_RAKI)
    WAVEFORMATEX * waveformatex = ( WAVEFORMATEX* ) _music->getFirstMusicPart()->getWaveFile()->getChunkData( "fmt " );

    ITF_ASSERT_MSG( waveformatex, "MusicInstanceContainer::createMusicInstanceSimple: problem with format loading stream part <%s>", _music->getFirstMusicPart()->getPath().getString8().cStr() );

    if ( !waveformatex )
        return U32_INVALID;

    channel = waveformatex->nChannels;
#else
    //todoisap
#endif

    switch (channel)
    {

    case 1: // mono -> voice 
        {
            for ( it = m_freeMonoVoiceInstanceArray.begin() ; it != m_freeMonoVoiceInstanceArray.end() ; ++it )
            {
                MusicInstanceSimple * monoVoiceInstance = *it;

                if ( monoVoiceInstance->canReinit() )
                {
                    m_freeMonoVoiceInstanceArray.erase( it );

                    monoVoiceInstance->reinit();

                    monoVoiceInstance->init( _music );

                    monoVoiceInstance->resetMuteFlag();
#if defined(ITF_SUPPORT_RAKI)
                    SoftwareBus * const bus = SOUND_ADAPTER->getBus( _busName );
                    monoVoiceInstance->setRoutingGroup( bus );
#else
                    SoftwareBus* const bus = AUDIO_ADAPTER->getBus(_busName);
                   // monoVoiceInstance->setRoutingGroup(bus);
#endif

                    //set pause sensitive flag from bus definition
                    bbool pauseSensitive = btrue;
                    if( bus )
                    {
                        const BusDef * const busDefinition = bus->getBusDef();
                        if( busDefinition )
                        {
                            pauseSensitive = busDefinition->m_pauseSensitive;
                        }
                    }
                    monoVoiceInstance->setPauseSensitive(pauseSensitive);

                    HandleInstanceStruct handleInstanceStruct( m_nextHandle, monoVoiceInstance );

                    m_musicInstanceArray.push_back( handleInstanceStruct );

                    ++m_nextHandle;

                    return handleInstanceStruct.m_handle;
                }
            }
        }
        break;


    case 2: // stereo -> ambiance 
        {
            for ( it = m_freeStereoAmbianceInstanceArray.begin() ; it != m_freeStereoAmbianceInstanceArray.end() ; ++it )
            {
                MusicInstanceSimple * stereoAmbianceInstance = *it;

                if ( stereoAmbianceInstance->canReinit() )
                {
                    m_freeStereoAmbianceInstanceArray.erase( it );

                    stereoAmbianceInstance->reinit();

                    stereoAmbianceInstance->init( _music );

                    stereoAmbianceInstance->resetMuteFlag();
#if defined(ITF_SUPPORT_RAKI)
                    SoftwareBus * const bus = SOUND_ADAPTER->getBus( _busName );
                    stereoAmbianceInstance->setRoutingGroup( bus );
#else
                    SoftwareBus* const bus = AUDIO_ADAPTER->getBus(_busName);
                    //stereoAmbianceInstance->setRoutingGroup(bus);
#endif

                    //set pause sensitive flag from bus definition
                    bbool pauseSensitive = btrue;
                    if( bus )
                    {
                        const BusDef * const busDefinition = bus->getBusDef();
                        if( busDefinition )
                        {
                            pauseSensitive = busDefinition->m_pauseSensitive;
                        }
                    }
                    stereoAmbianceInstance->setPauseSensitive(pauseSensitive);


                    HandleInstanceStruct handleInstanceStruct( m_nextHandle, stereoAmbianceInstance );

                    m_musicInstanceArray.push_back( handleInstanceStruct );

                    ++m_nextHandle;

                    return handleInstanceStruct.m_handle;
                }
            }
        }
        break;


    }




    ITF_ASSERT_MSG( 0, "MusicInstanceContainer::createMusicInstanceSimple requesting %s voice out of instances!", ( (channel == 2 ) ? "STEREO" : "MONO" ) );

    return 0;
}




bbool MusicInstanceContainer::canCreateMusicInstanceSimple( const u32 _nbChannels )
{
    ITF_VECTOR<MusicInstanceSimple*>::iterator it;

    switch ( _nbChannels )
    {

    case 1: // mono -> voice 
        {
            for ( it = m_freeMonoVoiceInstanceArray.begin() ; it != m_freeMonoVoiceInstanceArray.end() ; ++it )
            {
                MusicInstanceSimple * monoVoiceInstance = *it;

                if ( monoVoiceInstance->canReinit() )
                    return btrue;
            }
        }
        break;


    case 2: // stereo -> ambiance 
        {
            for ( it = m_freeStereoAmbianceInstanceArray.begin() ; it != m_freeStereoAmbianceInstanceArray.end() ; ++it )
            {
                MusicInstanceSimple * stereoAmbianceInstance = *it;

                if ( stereoAmbianceInstance->canReinit() )
                    return btrue;
            }
        }
        break;


    }

    return bfalse;
}

void MusicInstanceContainer::notifyEnd(u32 _handle, const MusicPart* _musicPart)
{
    MusicInstance* musicInstance = getMusicInstance(_handle);

    if (musicInstance)
    {
        musicInstance->notifyEnd(_musicPart);
    }
}

void MusicInstanceContainer::notifyDuration(u32 _handle, const MusicPart* _musicPart, f32 duration)
{
    MusicInstance* musicInstance = getMusicInstance(_handle);

    if (musicInstance)
    {
        f32 pos = musicInstance->getPosInSec();
        musicInstance->updateFromAudioCallback(_musicPart,(f32)pos,duration);
    }
}


void MusicInstanceContainer::updateFromAudioCallback(u32 _handle, const MusicPart* _musicPart, f32 _timePositionSec, f32 _soundDurationSec)
{
    MusicInstance* musicInstance = getMusicInstance(_handle);
    if (musicInstance)
    {
        musicInstance->updateFromAudioCallback(_musicPart, _timePositionSec, _soundDurationSec);
    }
}

void MusicInstanceContainer::destroyMusicInstance( u32 _handle )
{
    MusicInstanceStructVector::iterator it = m_musicInstanceArray.begin();

    while ( it != m_musicInstanceArray.end() )
    {
        if ( it->m_handle == _handle )
        {
            it->m_musicInstance->init( NULL );

            if ( it->m_musicInstance->isMusic() )
                m_freeMusicInstanceArray.push_back( it->m_musicInstance );
            else
            {
                if ( it->m_musicInstance->getNbChannels() == 1 )
                    m_freeMonoVoiceInstanceArray.push_back( (MusicInstanceSimple*)it->m_musicInstance );
                else
                    m_freeStereoAmbianceInstanceArray.push_back( (MusicInstanceSimple*)it->m_musicInstance );
            }

            m_musicInstanceArray.erase( it );

            return;
        }
        ++it;
    }
    //remove handle from paused instances list
    removeHandleFromPausedInstanceList(_handle);

    ITF_ASSERT_MSG( 0, "Invalid handle passed to MusicInstanceContainer::destroyMusicInstance" );
}

u32 MusicInstanceContainer::getHandle(const u32 _metronomeType)
{
    for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        if ( m_musicInstanceArray[ u ].m_musicInstance->getMetronomeType() == _metronomeType )
            return m_musicInstanceArray[ u ].m_handle;
    }

    return U32_INVALID;
}

void MusicInstanceContainer::play( const u32 _handle, const StringID _node, f32 _fadeTime /*= 0.0f*/, f32 _volume /*= .0f*/, const bbool _loop /*= btrue*/, const bbool _stopAndPlay /*= bfalse*/ )
{
    MusicInstance * musicInstance = getMusicInstance( _handle );

    if ( musicInstance )
    {
        musicInstance->play( _node, _fadeTime, _volume, _loop, _stopAndPlay );
    }
}


void MusicInstanceContainer::stop( const u32 _handle, f32 _fadeTime /*= 0.0f*/ )
{
    MusicInstance * musicInstance = getMusicInstance( _handle );

    if ( musicInstance )
    {
        musicInstance->stop(_fadeTime);
    }
    removeHandleFromPausedInstanceList(_handle);
}

void MusicInstanceContainer::stopAll( f32 _fadeTime /*= 0.0f*/ )
{
    for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        m_musicInstanceArray[ u ].m_musicInstance->stop(_fadeTime);
    }
    //clear paused instances list
    m_pausedInstanceList.clear();
}

void MusicInstanceContainer::muteAll( bbool _mute )
{
    for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        m_musicInstanceArray[ u ].m_musicInstance->mute(_mute);
    }
}

void MusicInstanceContainer::mute(bbool _mute, u32 _handle)
{
    MusicInstance * musicInstance = getMusicInstance( _handle );

    if ( musicInstance )
    {
        musicInstance->mute( _mute );
    }
}

bbool MusicInstanceContainer::isMuted()
{
    for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        if  ( m_musicInstanceArray[ u ].m_musicInstance->isMuted() )
            return btrue;
    }

    return bfalse;
}

bbool MusicInstanceContainer::isMuted(const u32 _handle)
{
    MusicInstance * musicInstance = getMusicInstance( _handle );

    if ( musicInstance )
    {
        return musicInstance->isMuted();
    }

    return bfalse;
}

void MusicInstanceContainer::setLinearVolume( const u32 _handle, const f32 _linearVolume )
{
    MusicInstance * musicInstance = getMusicInstance( _handle );

    if ( musicInstance )
    {
        musicInstance->setLinearVolume( _linearVolume );
    }
}

f32 MusicInstanceContainer::getLinearVolume( const u32 _handle )
{
    MusicInstance * musicInstance = getMusicInstance( _handle );

    if ( musicInstance )
    {
        return musicInstance->getLinearVolume();
    }

    return 0.0f;
}


void MusicInstanceContainer::pause(bbool _force)
{
    ITF_ASSERT(m_pausedInstanceList.size() == 0);
    for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        if( m_musicInstanceArray[u].m_musicInstance->isPauseSensitive() || _force )
        {
            m_musicInstanceArray[ u ].m_musicInstance->pause();
            m_pausedInstanceList.push_back(u);
        }
        else
        {
#ifdef LOG_MUSICINSTANCE
            RAKI_OUTPUT("0x%x MusicInstance not paused", this);
#endif // LOG_MUSICINSTANCE
        }
    }
}


void MusicInstanceContainer::resume()
{
    for(u32 index = 0; index < m_pausedInstanceList.size(); ++index)
    {
        const u32 instanceIndex = m_pausedInstanceList[index];
        m_musicInstanceArray[instanceIndex].m_musicInstance->resume();
    }
    m_pausedInstanceList.clear();
}

// Inputs
void MusicInstanceContainer::setInput(StringID _inputName, f32 _value )
{
    for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        m_musicInstanceArray[ u ].m_musicInstance->setInput(_inputName, _value);
    }
}

void MusicInstanceContainer::setInput(StringID _inputName, u32 _value )
{
    for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        m_musicInstanceArray[ u ].m_musicInstance->setInput(_inputName, _value);
    }
}

const StringID MusicInstanceContainer::debugGetMusicName(const u32 _handle)
{
    MusicInstance * musicInstance = getMusicInstance( _handle );

    if ( musicInstance )
    {
        return musicInstance->debugGetMusicName();
    }

    return ITF_GET_STRINGID_CRC(NoMusic,3174787028);
}

const u32 MusicInstanceContainer::getNbStreamingInstances()
{
    u32 nbStreamingInstances = 0 ;

    for ( u32 u = 0 ; u < m_musicInstanceArray.size() ; ++u )
    {
        if ( m_musicInstanceArray[ u ].m_musicInstance->isPlaying() )
            ++nbStreamingInstances;
    }

    return nbStreamingInstances;
}


} // namespace ITF

