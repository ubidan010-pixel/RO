#include "precompiled_engine.h"



#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_


#include "engine/AdaptersInterfaces/SoundAdapter.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_


#ifndef RAKI_STREAMINGTHREAD_H
#include "RakiEngine/System/Streaming/StreamingThread.h"
#endif // RAKI_STREAMINGTHREAD_H


#ifndef _ITF_MUSICINSTANCECONTAINER_H_
#include "engine/sound/Music/MusicInstanceContainer.h"
#endif // _ITF_MUSICINSTANCECONTAINER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_MUSICINSTANCE_H_
#include "engine/sound/Music/MusicInstance.h"
#endif // _ITF_MUSICINSTANCE_H_



namespace ITF
{
    bool PrefetchSound( const Path& path, PrefetchFactory::Params& params )
    {
        ResourceID  id;
        csAutoLock  cs( RESOURCE_MANAGER->getQueryOpsCs() );

        if ( RESOURCE_MANAGER->getResourceIdFromFile( id, path.getStringID(), Resource::ResourceType_MusicPart ) )
        {
            params.type = Resource::ResourceType_MusicPart;
            params.res = id.getResource();
            return btrue;
        }

        if ( RESOURCE_MANAGER->getResourceIdFromFile( id, path.getStringID(), Resource::ResourceType_Sound ) )
        {
            params.type = Resource::ResourceType_Sound;
            params.res = id.getResource();
            return btrue;
        }

        return bfalse;
    }

    IMPLEMENT_OBJECT_RTTI(MusicManager);
#ifdef ITF_SUPPORT_RAKI
    class ITFStreamingThread : public raki::StreamingThread
    {
    public:
        ITFStreamingThread()
            : m_isDisabled( 0 )
            , m_disableRequest( false )
        {

        }

        virtual bool canProcessRequest()
        {
            m_isDisabled = m_disableRequest ? 1 : 0;

            return m_isDisabled ? false : true;
        }

        void disable()
        {
            m_disableRequest = true;
        }

        void enable()
        {
            m_disableRequest = false;
        }

        bool isDisabled()
        {
            return m_isDisabled ? true : false;
        }

    protected:

        virtual void preRun()
        {
            ITF::FILEMANAGER->loadBundles();
        }

        virtual void postRun()
        {
            ITF::FILEMANAGER->unloadBundles();
        }

    private:

        volatile int m_isDisabled;
        bool m_disableRequest;

    };


    raki::StreamingThread * MusicManager::getStreamingThread()
    { 
        return m_streamingThread; 
    }
#endif

    void MusicManager::enableStreamingThread()
    {
#ifdef ITF_SUPPORT_RAKI
        ITFStreamingThread * streamingThread = static_cast<ITFStreamingThread *>(getStreamingThread());
        if( streamingThread && streamingThread->isDisabled() )
            streamingThread->enable();
#endif
    }

    void MusicManager::disableStreamingThread()
    {
#ifdef ITF_SUPPORT_RAKI
        ITFStreamingThread * streamingThread = static_cast<ITFStreamingThread *>(getStreamingThread());
        if( streamingThread && !streamingThread->isDisabled() )
            streamingThread->disable();
#endif
    }





    IMPLEMENT_OBJECT_RTTI(MusicManager_Template)

    BEGIN_SERIALIZATION(MusicManager_Template)
    SERIALIZE_OBJECT("musicPartSet",m_musicPartSet);
    SERIALIZE_OBJECT("musicTree",m_musicTree);
    SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    END_SERIALIZATION()


    bbool MusicManager_Template::onTemplateLoaded()
    {
//         const ITF_VECTOR<MusicPart_Template>& musicPartList = m_musicPartSet.getMusicPartList();
// 
//         for ( int i = 0 ; i < musicPartList.size() ; ++i )

        return Super::onTemplateLoaded();
    }


    MusicManager::MusicManager()
        : m_musicInstanceContainer( NULL )
        , m_paused(bfalse)
    {
#ifdef MUSICMANAGER_MUSICTEST
        m_musicInstanceContainer = newAlloc( mId_AudioEngine, MusicInstanceContainer );

        // Register event for inputs
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayMusic,2305092816),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventStopMusic,2288454954),this);
#endif // MUSICMANAGER_MUSICTEST
    }

    MusicManager::~MusicManager()
    {
#ifdef MUSICMANAGER_MUSICTEST
        delete m_musicInstanceContainer;

        // clear m_musicArray
        while ( m_musicArray.size() )
        {
            MusicContainer::iterator it = m_musicArray.begin();

            Music * music = (*it).second;
            delete music;

            m_musicArray.erase( it );
        }

        // clear m_ambianceArray
        while ( m_ambianceArray.size() )
        {
            AmbianceContainer::iterator it = m_ambianceArray.begin();

            Ambiance ambiance = (*it).second;
            Music * music = ambiance.m_music;
            delete music;

            m_ambianceArray.erase( it );
        }

#ifdef ITF_SUPPORT_RAKI
        // stop streaming thread 
        if ( m_streamingThread )
        {
            m_streamingThread->stop();

            while ( m_streamingThread->isRunning() )
                raki::Thread::sleep( 5 );

            delete m_streamingThread;
        }

        m_streamingThread = NULL;
#endif

        m_templateClientHandler.freeUsedTemplates();

        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903), this);
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayMusic,2305092816), this);
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventStopMusic,2288454954), this);
        }

#endif // MUSICMANAGER_MUSICTEST
    }

    bbool MusicManager::isPhysicallyLoaded() const
    {
        MusicContainer::const_iterator it       = m_musicArray.begin();
        MusicContainer::const_iterator it_end   = m_musicArray.end();

        bbool physicallyLoaded = btrue;
        for ( ; it != it_end && physicallyLoaded ; it++ )
        {
            if ( it->second != NULL && !it->second->isPhysicallyLoaded() )
                physicallyLoaded = bfalse;
        }

        return physicallyLoaded;
    }

    void MusicManager::update( f32 _deltaTime )
    {
#ifdef ITF_SUPPORT_RAKI
#ifdef MUSICMANAGER_MUSICTEST
        if ( !m_streamingThread )
        {
            m_streamingThread = newAlloc( mId_AudioEngine, ITFStreamingThread );
            m_streamingThread->start();
        }
#endif // MUSICMANAGER_MUSICTEST
#endif
    }


    void MusicManager::musicInstanceContainerUpdate( f32 _deltaTime )
    {
#ifdef MUSICMANAGER_MUSICTEST
        m_musicInstanceContainer->update( _deltaTime );
#endif // MUSICMANAGER_MUSICTEST
    }


    void MusicManager::init()
    {
#ifdef MUSICMANAGER_MUSICTEST
        //static const Path menuConfigFile(GETPATH_ALIAS("menumusicconfig"));
        //addMusicFromConfigFile(menuConfigFile);

        //static const Path lumkingConfigFile(GETPATH_ALIAS("lumkingmusicconfig"));
        //addMusicFromConfigFile(lumkingConfigFile);

#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::addMusicFromConfigFile(const Path& _configFile)
    {
#ifdef MUSICMANAGER_MUSICTEST
        m_templateClientHandler.addUsedTemplate(_configFile.getStringID());
        const MusicManager_Template* config = TEMPLATEDATABASE->getTemplate<MusicManager_Template>(&m_templateClientHandler, _configFile);
        if (!config)
        {
            ITF_FATAL_ERROR("Couldn't load music config: %s", _configFile.getString8().cStr());
        }

        addMusic( config );
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::onEvent( Event* _event )
    {
#ifdef MUSICMANAGER_MUSICTEST
        if (EventSetUintInput * eventSetInput = _event->DynamicCast<EventSetUintInput>(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903)))
        {
            setInput(eventSetInput->getInputName(), eventSetInput->getInputValue());
        }
        else if (EventPlayMusic * eventPlayMusic = _event->DynamicCast<EventPlayMusic>(ITF_GET_STRINGID_CRC(EventPlayMusic,2305092816)))
        {
            if ( eventPlayMusic->getMetronomeType() != StringID::Invalid )
            {
                play(eventPlayMusic->getMetronomeType(), eventPlayMusic->getNodeName(), eventPlayMusic->getFadeTime(), 
                    eventPlayMusic->getVolume().ratio(), eventPlayMusic->getPlayOnNext(), eventPlayMusic->getStopAndPlay() );
            }
        }
        else if (EventStopMusic * eventStopMusic = _event->DynamicCast<EventStopMusic>(ITF_GET_STRINGID_CRC(EventStopMusic,2288454954)))
        {
            if ( eventStopMusic->getMetronomeType() != StringID::Invalid )
            {
                stop(eventStopMusic->getMetronomeType(), eventStopMusic->getFadeTime());
            }
        }
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::addMusic( const MusicManager_Template * _musicManagerTemplate )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 metronomeType = _musicManagerTemplate->getMusicTreeTemplate()->getMetronomeType();
        // Invalid music type
        if ( metronomeType > METRONOME_TYPE_COUNT )
            return;

        // A music with the same music type has already been added ?
        MusicContainer::iterator it = m_musicArray.find(metronomeType);
        if (it != m_musicArray.end())
        {
            Music * music   = (*it).second;
            ITF_ASSERT(music);
            ITF_ASSERT_MSG(!music->isPlaying(), "Trying to modify music tree but music of metronome %d is still playing, so it will be stopped brutally", metronomeType);

            if ( music )
            {
                // Music is still playing, so stop it now !
                stop(metronomeType);

                // Music is not currently playing, so we can remove it from music tree as it will be replaced by the new one
                destroyMusicInstance(metronomeType);
                delete music;
                m_musicArray.erase( it );
            }
        }

        Music * music = newAlloc( mId_AudioEngine, Music );

        music->init( _musicManagerTemplate->getMusicPartSetTemplate(), _musicManagerTemplate->getMusicTreeTemplate(), _musicManagerTemplate->getInputList() );

        createMusicInstance( music, ITF_GET_STRINGID_CRC(MUSIC,863906348) );

        m_musicArray[metronomeType] = music;

#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::addMusic( const StringID _name, const Path& _musicPath, const StringID _busName, const StringID& _wwwiseid)
    {
#ifdef MUSICMANAGER_MUSICTEST

        // New Music ?
        AmbianceContainer::iterator it = m_ambianceArray.find(_name);
        if (it == m_ambianceArray.end())
        {
            Ambiance newAmbiance;
            newAmbiance.m_music = newAlloc( mId_AudioEngine, MusicSimple );

            newAmbiance.m_music->init( _name, _musicPath, _wwwiseid);

            newAmbiance.m_handle = createMusicInstanceSimple( newAmbiance.m_music, _busName );

            m_ambianceArray[_name] = newAmbiance;
        }

#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::removeMusic( const MusicPartSet_Template * _musicPartSetTemplate, const MusicTree_Template * _musicTreeTemplate )
    {
        // TODO
#ifdef MUSICMANAGER_MUSICTEST

#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::removeMusic( const StringID _name )
    {
        // TODO
#ifdef MUSICMANAGER_MUSICTEST
        // New Music ?
        AmbianceContainer::iterator it = m_ambianceArray.find(_name);
        if (it != m_ambianceArray.end())
        {
            Ambiance ambiance = it->second;
            if ( ambiance.m_music )
            {
                // Music is still playing, so stop it now !
                if ( ambiance.m_music->isPlaying() )
                    stop(_name);

                // delete
                destroyMusicInstance(_name);
                delete ambiance.m_music;
                m_ambianceArray.erase( it );
            }
        }
#endif // MUSICMANAGER_MUSICTEST
    }
    bbool MusicManager::isMusicPlaying(const StringID _name) const
    {
        // TODO
#ifdef MUSICMANAGER_MUSICTEST
        // New Music ?
        AmbianceContainer::const_iterator it = m_ambianceArray.find(_name);
        if (it != m_ambianceArray.end())
        {
            Ambiance ambiance = it->second;
            MusicInstance * musicInstance = m_musicInstanceContainer->getMusicInstance(ambiance.m_handle);
            if ( musicInstance )
            {
                // Music is still playing, so stop it now !
                if ( musicInstance->isPlaying() )
                {
                    return btrue;
                }
            }
        }
#endif // MUSICMANAGER_MUSICTEST
        return bfalse;
    }
    u32  MusicManager::getHandle(const u32 _metronomeType)
    { 

#ifdef MUSICMANAGER_MUSICTEST
        return m_musicInstanceContainer->getHandle(_metronomeType);
#else // MUSICMANAGER_MUSICTEST
        return 0;
#endif // MUSICMANAGER_MUSICTEST
    }

    u32  MusicManager::getHandle(const StringID _ambianceName)
    { 

#ifdef MUSICMANAGER_MUSICTEST
        AmbianceContainer::iterator it = m_ambianceArray.find(_ambianceName);
        if ( it != m_ambianceArray.end() )
            return it->second.m_handle;
        return U32_INVALID;
#else // MUSICMANAGER_MUSICTEST
        return 0;
#endif // MUSICMANAGER_MUSICTEST
    }
    
    u32 MusicManager::createMusicInstance( Music * _music, const StringID _busName )
    {
#ifdef MUSICMANAGER_MUSICTEST
        return m_musicInstanceContainer->createMusicInstance( _music, _busName );
#else // MUSICMANAGER_MUSICTEST
        return 0;
#endif // MUSICMANAGER_MUSICTEST
    }
    
    u32 MusicManager::createMusicInstanceSimple( MusicSimple * _music, const StringID _busName )
    {
#ifdef MUSICMANAGER_MUSICTEST
        return m_musicInstanceContainer->createMusicInstanceSimple( _music, _busName  );
#else // MUSICMANAGER_MUSICTEST
        return 0;
#endif // MUSICMANAGER_MUSICTEST
    }

    bbool MusicManager::canCreateMusicInstanceSimple( const u32 _nbChannels )
    {
#ifdef MUSICMANAGER_MUSICTEST
        return m_musicInstanceContainer->canCreateMusicInstanceSimple( _nbChannels );
#else // MUSICMANAGER_MUSICTEST
        return bfalse;
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::destroyMusicInstance( u32 _metronomeType )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_metronomeType);
        m_musicInstanceContainer->destroyMusicInstance( handle );
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::destroyMusicInstance( const StringID _ambianceName )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_ambianceName);
        m_musicInstanceContainer->destroyMusicInstance( handle );
#endif // MUSICMANAGER_MUSICTEST
    }
    
    
    void MusicManager::notifyEnd(const u32 _metronomeType,const MusicPart* _musicPart)
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_metronomeType);
        m_musicInstanceContainer->notifyEnd(handle, _musicPart);
#endif // MUSICMANAGER_MUSICTEST

    }
    void MusicManager::notifyDuration(const u32 _metronomeType, const MusicPart* _musicPart, f32 duration)
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_metronomeType);
        m_musicInstanceContainer->notifyDuration(handle, _musicPart, duration);
#endif // MUSICMANAGER_MUSICTEST

    }

    void MusicManager::play(const u32 _metronomeType, const StringID _node, const f32 _fadeTime /*= 0.0f*/, const f32 _volume /*= .0f*/, const u32 _playOnNext /*= U32_INVALID*/, const bbool _stopAndPlay /*= bfalse*/)
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_metronomeType);
        Music * music = NULL;
        // A music with the same music type has already been added ?
        MusicContainer::iterator it = m_musicArray.find(_metronomeType);
        if (it != m_musicArray.end())
        {
            music   = (*it).second;
        }

        if ( music == NULL || !music->isPlaying() || (music->isPlaying() && _playOnNext == U32_INVALID) )
            m_musicInstanceContainer->play(handle, _node, _fadeTime, _volume, btrue, _stopAndPlay );
        // play a music synchronized with current one
        else
        {
            // TODO : uncomment this to make sure that no one is trying to play a synchronized  music
            // while another one is already waiting to be played
            //ITF_WARNING_CATEGORY(sound, NULL,!m_musicInstanceContainer->hasTransitionInstance(handle),"Trying to play another synchronized music, but there is already one waiting to be played. Please check your music trigger");
            //if ( !m_musicInstanceContainer->hasTransitionInstance(handle) )
            {
                ITF_ASSERT_CRASH(music, "A Music is playing but the pointer to it is NULL ?!");
                m_musicInstanceContainer->createMusicInstance( music, ITF_GET_STRINGID_CRC(MUSIC,863906348) );
                // TODO : call playTransitionInstance with given parameters.
                //m_musicInstanceContainer->playTransitionInstance(handle,)
            }
        }
#endif // MUSICMANAGER_MUSICTEST
    }
    
    void MusicManager::play( const StringID _ambianceName, const f32 _fadeTime /*= 0.0f*/, const f32 _volume /*= 1.0f*/, const bbool _loop /*= btrue*/ )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_ambianceName);
        m_musicInstanceContainer->play( handle, _ambianceName, _fadeTime, _volume, _loop );
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::stop( const u32 _metronomeType, const f32 _fadeTime /*= 0.0f*/ )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_metronomeType);
        m_musicInstanceContainer->stop( handle, _fadeTime );
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::stop( const StringID _ambianceName, const f32 _fadeTime /*= 0.0f*/ )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_ambianceName);
        m_musicInstanceContainer->stop( handle, _fadeTime );
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::stopAll( f32 _fadeTime /*= 0.0f*/ )
    {
#ifdef MUSICMANAGER_MUSICTEST
        m_musicInstanceContainer->stopAll( _fadeTime );
#endif // MUSICMANAGER_MUSICTEST
    }
    
    
    void MusicManager::setLinearVolume( const u32 _metronomeType, const f32 _linearVolume )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_metronomeType);
        m_musicInstanceContainer->setLinearVolume( handle, _linearVolume );
#endif // MUSICMANAGER_MUSICTEST
    }

    f32 MusicManager::getLinearVolume( const u32 _metronomeType )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_metronomeType);
        return m_musicInstanceContainer->getLinearVolume( handle );
#else
        return 0.0f;
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::setLinearVolume( const StringID _ambianceName, const f32 _linearVolume )
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_ambianceName);
        m_musicInstanceContainer->setLinearVolume( handle, _linearVolume );
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::muteAll(bbool _mute)
    {
#ifdef MUSICMANAGER_MUSICTEST
        m_musicInstanceContainer->muteAll(_mute);
#endif // MUSICMANAGER_MUSICTEST
    }

    void MusicManager::muteAllBut(bbool _mute, u32 _metronomeType)
    {
#ifdef MUSICMANAGER_MUSICTEST
        muteAll(_mute);
        // Do the opposite for the one specified with the metronome
        for (u32 i = 0 ; i < METRONOME_TYPE_COUNT ; ++i)
        {
            if ( _metronomeType == i )
            {
                u32 handle = getHandle(i);
                m_musicInstanceContainer->mute( !_mute, handle );
            }
        }
#endif // MUSICMANAGER_MUSICTEST
    }

    bbool MusicManager::isMuted()
    {
#ifdef MUSICMANAGER_MUSICTEST
        return m_musicInstanceContainer->isMuted();
#else
        return bfalse;
#endif // MUSICMANAGER_MUSICTEST
    }
    
    bbool MusicManager::isMuted(const u32 _metronomeType)
    {
#ifdef MUSICMANAGER_MUSICTEST
        u32 handle = getHandle(_metronomeType);
        return m_musicInstanceContainer->isMuted( handle );
#else
        return bfalse;
#endif // MUSICMANAGER_MUSICTEST
    }
    
    void MusicManager::pause(bbool _force)
    {
#ifdef MUSICMANAGER_MUSICTEST
        m_musicInstanceContainer->pause(_force);
#endif // MUSICMANAGER_MUSICTEST
        m_paused = btrue;
    }
    
    
    void MusicManager::resume()
    {
#ifdef MUSICMANAGER_MUSICTEST
        m_musicInstanceContainer->resume();
#endif // MUSICMANAGER_MUSICTEST
        m_paused = bfalse;
    }

    void MusicManager::setInput(StringID _inputName, f32 _value )
    {
        if ( m_musicInstanceContainer != NULL )
        {
            m_musicInstanceContainer->setInput(_inputName, _value);
        }
    }

    void MusicManager::setInput(StringID _inputName, u32 _value )
    {
        if ( m_musicInstanceContainer != NULL )
        {
            m_musicInstanceContainer->setInput(_inputName, _value);
        }
    }

    const StringID MusicManager::debugGetMusicName(const u32 _metronomeType)
    {
#ifdef MUSICMANAGER_MUSICTEST
        if ( m_musicInstanceContainer != NULL )
        {
            u32 handle = getHandle(_metronomeType);
            return m_musicInstanceContainer->debugGetMusicName(handle);
        }
#endif // MUSICMANAGER_MUSICTEST
        return ITF_GET_STRINGID_CRC(none,2748391529);
    }


    bbool MusicManager::playSyncWithMetronome( const u32 _handle, const u32 _metronomeType, const u32 _playOnNextParam, const f32 _volume )
    {
#ifdef MUSICMANAGER_MUSICTEST
        
        MusicContainer::iterator it = m_musicArray.find( _metronomeType );

        if ( it != m_musicArray.end() )
        {
            MusicInstance * musicInstance = it->second->getMusicInstance();

            if ( musicInstance )
                return musicInstance->registerForPlayAtNextDivision( _handle, _playOnNextParam, _volume );
        }
#endif // MUSICMANAGER_MUSICTEST

        return bfalse;
    }

#if defined(ITF_SUPPORT_RAKI)
    bbool MusicManager::playSyncWithMetronome( raki::VoiceBase * _voice, const u32 _metronomeType, const u32 _playOnNextParam, const f32 _volume )
    {
#ifdef MUSICMANAGER_MUSICTEST

        MusicContainer::iterator it = m_musicArray.find( _metronomeType );

        if ( it != m_musicArray.end() )
        {
            MusicInstance * musicInstance = it->second->getMusicInstance();

            if ( musicInstance )
                return musicInstance->registerForPlayAtNextDivision( _voice, _playOnNextParam, _volume );
        }
#endif // MUSICMANAGER_MUSICTEST

        return bfalse;
    }

#else

    bbool MusicManager::playSyncWithMetronome(SoundInstance* _voice, const u32 _metronomeType, const u32 _playOnNextParam, const f32 _volume)
    {
#ifdef MUSICMANAGER_MUSICTEST

        MusicContainer::iterator it = m_musicArray.find(_metronomeType);

        if (it != m_musicArray.end())
        {
            MusicInstance* musicInstance = it->second->getMusicInstance();

            if (musicInstance)
                return musicInstance->registerForPlayAtNextDivision(_voice, _playOnNextParam, _volume);
        }
#endif // MUSICMANAGER_MUSICTEST
        return bfalse;
    }


#endif

    const u32 MusicManager::getNbStreamingInstances()
    {
        return m_musicInstanceContainer->getNbStreamingInstances();
    }

}

