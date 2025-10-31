#include "precompiled_engine.h"


#ifndef _ITF_INSTANCEMANAGER_H_
#include "engine/sound/InstanceManager.h"
#endif // _ITF_INSTANCEMANAGER_H_




#ifndef _ITF_MUSICINSTANCE_H_
#include "engine/sound/Music/MusicInstance.h"
#endif // _ITF_MUSICINSTANCE_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_SOFTWAREBUS_H_
#include "engine/sound/routing/SoftwareBus.h"
#endif // _ITF_SOFTWAREBUS_H_



#if defined ( ITF_WINDOWS ) || defined ( ITF_X360 )

#ifndef RAKI_BUFFERSOUNDNATIVE_XAUDIO2_H
#include "RakiEngine/Sounds/_XAudio2/BufferSoundNative.h"
#endif // RAKI_BUFFERSOUNDNATIVE_XAUDIO2_H

#elif defined( ITF_XBOX_SERIES) // todo add a full empty wrapper for xbox series

#include "RakiEngine/Sounds/_XBoxSeries/BufferSoundNative_XboxSeries.h"

#elif defined ( ITF_PS3 ) 

#ifndef RAKI_BUFFERSOUNDNATIVE_PS3_H
#include "RakiEngine/Sounds/_PS3/BufferSoundNative_PS3.h"
#endif // RAKI_BUFFERSOUNDNATIVE_PS3_H

#elif defined ( ITF_WII ) 

#ifndef RAKI_BUFFERSOUNDNATIVE_WII_H
#include "RakiEngine/Sounds/_Wii/BufferSoundNative_Wii.h"
#endif // RAKI_BUFFERSOUNDNATIVE_WII_H

#elif defined ( ITF_PS5 ) 
#ifdef ITF_SUPPORT_RAKI
#include "RakiEngine/Sounds/_PS5/BufferSoundNative_PS5.h"
#endif

#elif defined ( ITF_NINTENDO ) 

#include "RakiEngine/Sounds/_Nintendo/BufferSoundNative_Nintendo.h"

#endif

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

//#define LOG_INSTANCEMANAGER


namespace ITF
{


InstanceManager::InstanceManager( u32 _nbInstances )
    : m_nextHandle( 1 )
    , m_nbInstances( _nbInstances )
{
#ifdef ITF_SUPPORT_RAKI
    m_bufferSoundArray = newAlloc( mId_AudioEngine, raki::VoiceBase* )[ m_nbInstances ];


    for ( u32 instanceIndex = 0 ; instanceIndex < m_nbInstances ; ++instanceIndex )
    {
        m_bufferSoundArray[ instanceIndex ] = newAlloc( mId_AudioEngine, raki::BufferSoundNative );

        m_freeInstanceArray.push_back( instanceIndex );
    }
#else

    for (u32 instanceIndex = 0; instanceIndex < m_nbInstances; ++instanceIndex)
    {

        m_freeInstanceArray.push_back(instanceIndex);
    }

#endif
}


InstanceManager::~InstanceManager()
{
#ifdef ITF_SUPPORT_RAKI
    // first force stop all instances 

    for ( u32 instanceIndex = 0 ; instanceIndex < m_nbInstances ; ++instanceIndex )
    {
        m_bufferSoundArray[ instanceIndex ]->stop();
    }

    // wait for callbacks to acknowledge the stop 

    raki::Thread::sleep( 20 );


    for ( u32 instanceIndex = 0 ; instanceIndex < m_nbInstances ; ++instanceIndex )
    {
        delete m_bufferSoundArray[ instanceIndex ];
        m_bufferSoundArray[ instanceIndex ] = NULL;
    }

    delete [] m_bufferSoundArray;


    m_bufferSoundArray = NULL;
#else
    // first force stop all instances 

#endif
    m_nbInstances = 0;
}


#ifdef ITF_SUPPORT_RAKI

u32 InstanceManager::createSource( const raki::Format * _format, const StringID & _busName )
{
    ITF_LIST<u32>::iterator it = m_freeInstanceArray.begin(); 

    if ( it != m_freeInstanceArray.end() )
    {
        u32 instanceIndex = *it;

        raki::BufferSoundNative * bufferSound = (raki::BufferSoundNative *)m_bufferSoundArray[ instanceIndex ];

        if ( bufferSound->createVoice( _format ) )
        {
            m_freeInstanceArray.erase( it );

            SoftwareBus * const bus = SOUND_ADAPTER->getBus( _busName );
            bufferSound->setRoutingGroup( bus );

            //set pause sensitive flag from bus definition
            bbool pauseSensitive = btrue;
            if( bus != NULL )
            {
                const BusDef * const busDefinition = bus->getBusDef();
                if( busDefinition != NULL )
                {
                    pauseSensitive = busDefinition->m_pauseSensitive;
                }
            }
            return registerInstanceIndexAndReturnHandle( instanceIndex, pauseSensitive );
        }
    }

    return 0xFFFFFFFF;
}
#else
u32 InstanceManager::createSource(const StringID& _busName, SoundInstance* _inst)
{
    ITF_LIST<u32>::iterator it = m_freeInstanceArray.begin();

    if (it != m_freeInstanceArray.end())
    {

        u32 instanceIndex = *it;

        m_freeInstanceArray.erase(it);

        SoftwareBus* const bus = AUDIO_ADAPTER->getBus(_busName);

        //set pause sensitive flag from bus definition
        bbool pauseSensitive = btrue;
        if (bus != NULL)
        {
            const BusDef* const busDefinition = bus->getBusDef();
            if (busDefinition != NULL)
            {
                pauseSensitive = busDefinition->m_pauseSensitive;
            }
        }
        return registerInstanceIndexAndReturnHandle(instanceIndex, pauseSensitive,  _inst);
    }

    return ITF::SoundHandle::c_invalidValue;
}

#endif
void InstanceManager::update()
{
#ifdef ITF_SUPPORT_RAKI
    InstanceVector::iterator it = m_usedInstanceArray.begin(); 

    while ( it != m_usedInstanceArray.end() )
    {
        const u32 instanceIndex = it->m_instanceIndex; 


        m_bufferSoundArray[ instanceIndex ]->updateVolume();

        if ( !m_bufferSoundArray[ instanceIndex ]->isPlaying() )
        {
#ifdef LOG_INSTANCEMANAGER
            LOG("InstanceManager::update removing _instance 0x%x handle 0x%x", m_bufferSoundArray[ instanceIndex ], it->m_handle );
#endif // LOG_INSTANCEMANAGER

            raki::BufferSoundNative * bufferSound = (raki::BufferSoundNative *)m_bufferSoundArray[ instanceIndex ];

            bufferSound->destroyVoice();

            m_freeInstanceArray.push_back( it->m_instanceIndex );

            it = m_usedInstanceArray.erase( it );
        }
        else
        {
            ++it;
        }

    }
#else
    InstanceVector::iterator it = m_usedInstanceArray.begin();

    while (it != m_usedInstanceArray.end())
    {

        if (it->m_Instance)
        {
            it->m_Instance->updateVolume();

           // if (!m_bufferSoundArray[instanceIndex]->isPlaying())
            if(!it->m_Instance->isPlaying())
            {
    #ifdef LOG_INSTANCEMANAGER
                LOG("InstanceManager::update removing _instance 0x%x handle 0x%x", it->m_Instance, it->m_handle);
    #endif // LOG_INSTANCEMANAGER

             //   it->m_Instance->stopVoice();

                m_freeInstanceArray.push_back(it->m_instanceIndex);

                it = m_usedInstanceArray.erase(it);
            }
            else
            {
                ++it;
            }
        }

    }

#endif
}


u32 InstanceManager::registerInstanceIndexAndReturnHandle( const u32 _instanceIndex, const bbool _pauseSensitive, SoundInstance* _inst)
{
    ITF_ASSERT( !isHandleRegistered( m_nextHandle ) );
    ITF_ASSERT( !isInstanceIndexRegistered( _instanceIndex ) );

    HandleInstanceStruct element( m_nextHandle, _instanceIndex, _pauseSensitive );


    element.m_Instance = _inst;


    m_usedInstanceArray.push_back( element );

    ++m_nextHandle; 
    
    // hope that game gets killed before...
    if ( m_nextHandle > 0xfffffff0 )
        m_nextHandle = 1;

#ifdef LOG_INSTANCEMANAGER
    LOG("InstanceManager::registerInstanceAndReturnHandle adding _instance 0x%x returning handle 0x%x", _inst, element.m_handle );
#endif // LOG_INSTANCEMANAGER

    return element.m_handle;
}




bbool InstanceManager::isHandleRegistered( u32 _handle ) const
{
    for ( InstanceVector::const_iterator it = m_usedInstanceArray.begin() ; it != m_usedInstanceArray.end() ; ++it )
    {
        if ( it->m_handle == _handle )
            return btrue;
    }

    return bfalse;
}


bbool InstanceManager::isInstanceIndexRegistered( const u32 _instanceIndex ) const
{
    for ( InstanceVector::const_iterator it = m_usedInstanceArray.begin() ; it != m_usedInstanceArray.end() ; ++it )
    {
        if ( it->m_instanceIndex == _instanceIndex )
            return btrue;
    }

    return bfalse;
}


const u32 InstanceManager::getHandleFromInstanceIndex( const u32 _instanceIndex )
{
    for ( InstanceVector::iterator it = m_usedInstanceArray.begin() ; it != m_usedInstanceArray.end() ; ++it )
    {
        if ( it->m_instanceIndex == _instanceIndex )
            return it->m_handle;
    }

    return 0xFFFFFFFF;
}

SoundInstance* InstanceManager::getInstanceFromInstanceIndex(const u32 _instanceIndex)
{
    for (InstanceVector::iterator it = m_usedInstanceArray.begin(); it != m_usedInstanceArray.end(); ++it)
    {
        if (it->m_instanceIndex == _instanceIndex)
            return it->m_Instance;
    }

    return nullptr;
}



void InstanceManager::getPlayingVoices( SafeArray<u32> & _playingVoices )
{
    for ( InstanceVector::iterator it = m_usedInstanceArray.begin() ; it != m_usedInstanceArray.end() ; ++it )
    {
        _playingVoices.push_back( it->m_handle );
    }
}

void InstanceManager::getPauseSensitivePlayingVoices(SafeArray<u32> & _playingVoices)
{
    for ( InstanceVector::iterator it = m_usedInstanceArray.begin() ; it != m_usedInstanceArray.end() ; ++it )
    {
        if( it->m_pauseSensitive )
        {
#if defined(ITF_SUPPORT_RAKI)
            _playingVoices.push_back( it->m_handle );
#else
            if(it->m_Instance)
                _playingVoices.push_back(it->m_Instance->getPlayingHandle().getValueAsU32());
#endif
        }
    }
}




} // namespace ITF
