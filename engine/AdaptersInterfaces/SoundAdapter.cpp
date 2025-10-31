#include "precompiled_engine.h"


#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_


#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_INSTANCEMANAGER_H_
#include "engine/sound/InstanceManager.h"
#endif // _ITF_INSTANCEMANAGER_H_

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef RAKI_VOICEBASE_H
#include "RakiEngine/Sounds/VoiceBase.h"
#endif // RAKI_VOICEBASE_H

#ifndef _ITF_SOFTWAREBUS_H_
#include "engine/sound/routing/SoftwareBus.h"
#endif // _ITF_SOFTWAREBUS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_SIMPLECACHEREADFILESTREAM_H_
#include "engine/sound/raki/SimpleCacheReadFileStream.h"
#endif // _ITF_SIMPLECACHEREADFILESTREAM_H_




#if defined ( ITF_WINDOWS) || defined ( ITF_X360 ) || defined ( ITF_PS5 ) || defined ( ITF_NINTENDO ) || defined(ITF_XBOX_SERIES)

#ifndef RAKI_AUDIOCALLBACKMANAGER_H
#include "RakiEngine/System/AudioCallbackManager/AudioCallbackManager.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_H

#define SOUND_ADAPTER_MAX_NB_VOICES 256

#elif defined ( ITF_PS3 )

#ifndef RAKI_AUDIOCALLBACKMANAGER_H
#include "RakiEngine/System/AudioCallbackManager/AudioCallbackManager.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_H

#define SOUND_ADAPTER_MAX_NB_VOICES 128

#elif defined ( ITF_WII )

#ifndef RAKI_AUDIOCALLBACKMANAGER_WII_H
#include "RakiEngine/Specific/_Wii/AudioCallbackManager_Wii.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_WII_H

#define SOUND_ADAPTER_MAX_NB_VOICES 80
//AX_MAX_VOICES is 96

#endif


#include "engine/sound/soundconfigtemplate.h"




namespace ITF
{

    BEGIN_SERIALIZATION(BusDef)
        SERIALIZE_MEMBER("name",m_name);
        SERIALIZE_CONTAINER("outputs",m_outputs);
        SERIALIZE_MEMBER("volume",m_volume);
        SERIALIZE_MEMBER("filterFrequency",m_filterFrequency);
        SERIALIZE_MEMBER("filterQ",m_filterQ);
        SERIALIZE_ENUM_BEGIN("filterType",m_filterType);
        SERIALIZE_ENUM_VAR(FilterType_LowPass);
        SERIALIZE_ENUM_VAR(FilterType_BandPass);
        SERIALIZE_ENUM_VAR(FilterType_HighPass);
        SERIALIZE_ENUM_VAR(FilterType_Notch);
        SERIALIZE_ENUM_VAR(FilterType_None);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("hasReverb",m_hasReverb);
        SERIALIZE_MEMBER("reverbActive",m_reverbActive);
        SERIALIZE_ENUM_BEGIN("reverbPreset", m_reverbPreset);
            SERIALIZE_ENUM_VAR(ReverbPreset_DEFAULT);
            SERIALIZE_ENUM_VAR(ReverbPreset_GENERIC);
            SERIALIZE_ENUM_VAR(ReverbPreset_PADDEDCELL);
            SERIALIZE_ENUM_VAR(ReverbPreset_ROOM);
            SERIALIZE_ENUM_VAR(ReverbPreset_BATHROOM);
            SERIALIZE_ENUM_VAR(ReverbPreset_LIVINGROOM);
            SERIALIZE_ENUM_VAR(ReverbPreset_STONEROOM);
            SERIALIZE_ENUM_VAR(ReverbPreset_AUDITORIUM);
            SERIALIZE_ENUM_VAR(ReverbPreset_CONCERTHALL);
            SERIALIZE_ENUM_VAR(ReverbPreset_CAVE);
            SERIALIZE_ENUM_VAR(ReverbPreset_ARENA);
            SERIALIZE_ENUM_VAR(ReverbPreset_HANGAR);
            SERIALIZE_ENUM_VAR(ReverbPreset_CARPETEDHALLWAY);
            SERIALIZE_ENUM_VAR(ReverbPreset_HALLWAY);
            SERIALIZE_ENUM_VAR(ReverbPreset_STONECORRIDOR);
            SERIALIZE_ENUM_VAR(ReverbPreset_ALLEY);
            SERIALIZE_ENUM_VAR(ReverbPreset_FOREST);
            SERIALIZE_ENUM_VAR(ReverbPreset_CITY);
            SERIALIZE_ENUM_VAR(ReverbPreset_MOUNTAINS);
            SERIALIZE_ENUM_VAR(ReverbPreset_QUARRY);
            SERIALIZE_ENUM_VAR(ReverbPreset_PLAIN);
            SERIALIZE_ENUM_VAR(ReverbPreset_PARKINGLOT);
            SERIALIZE_ENUM_VAR(ReverbPreset_SEWERPIPE);
            SERIALIZE_ENUM_VAR(ReverbPreset_UNDERWATER);
            SERIALIZE_ENUM_VAR(ReverbPreset_SMALLROOM);
            SERIALIZE_ENUM_VAR(ReverbPreset_MEDIUMROOM);
            SERIALIZE_ENUM_VAR(ReverbPreset_LARGEROOM);
            SERIALIZE_ENUM_VAR(ReverbPreset_MEDIUMHALL);
            SERIALIZE_ENUM_VAR(ReverbPreset_LARGEHALL);
            SERIALIZE_ENUM_VAR(ReverbPreset_PLATE);
            SERIALIZE_ENUM_VAR(ReverbPreset_CUSTOM);
            SERIALIZE_ENUM_VAR(ReverbPreset_NONE);   
        SERIALIZE_ENUM_END();
        SERIALIZE_CONTAINER_WITH_FACTORY("modifiers", m_modifiers, SOUND_MANAGER()->getModifierFactory());
        SERIALIZE_MEMBER("pauseSensitive", m_pauseSensitive);
        SERIALIZE_MEMBER("volumeRtpc", m_volumeRtpc);
	SERIALIZE_MEMBER("pitchRtpc", m_pitchRtpc);
	SERIALIZE_MEMBER("lpfRtpc", m_lpfRtpc);
	SERIALIZE_MEMBER("hpfRtpc", m_hpfRtpc);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(LimiterDef)
        SERIALIZE_MEMBER("name",m_name);
        SERIALIZE_MEMBER("maxInstances",m_maxInstances);
        SERIALIZE_MEMBER("mode",m_mode);
    END_SERIALIZATION()


        BEGIN_SERIALIZATION(BusMix)
        SERIALIZE_MEMBER("name",m_name);
    SERIALIZE_MEMBER("priority",m_priority);
    SERIALIZE_MEMBER("duration",m_duration);
    SERIALIZE_MEMBER("fadeIn",m_fadeIn);
    SERIALIZE_MEMBER("fadeOut",m_fadeOut);
    SERIALIZE_CONTAINER_OBJECT("busDefs",m_busDefs);
    END_SERIALIZATION()

    #define masterBus ITF_GET_STRINGID_CRC(master,2884859710)

#if defined(ITF_SUPPORT_RAKI)
    SoundAdapter::SoundAdapter()
        : m_bAudioCaptureAllowed( bfalse )
        , m_config( NULL )
        , m_instanceManager( NULL )
    {
        SimpleCacheReadFileStream::allocateStaticCache( 4096 * 2 );

        m_instanceManager = newAlloc( mId_AudioEngine, InstanceManager )( SOUND_ADAPTER_MAX_NB_VOICES );
    }


    SoundAdapter::~SoundAdapter()
    {
        if (TEMPLATEDATABASE)
        {
            static const Path soundConfig(GETPATH_ALIAS("soundconfig"));
            TEMPLATEDATABASE->removeTemplateClient(soundConfig.getStringID(), this);
        }

        SimpleCacheReadFileStream::freeStaticCache();
    }


    void SoundAdapter::update(f32 _dt)
    {
#if defined ( ITF_WII ) && defined ( AUDIOCALLBACKMANANGER_WII_CHECK_LAG_IN_UPDATES )
        ++raki::AudioCallbackManager::ms_gameUpdate_mainFrameCounter;
#endif // defined ( ITF_WII ) && defined ( AUDIOCALLBACKMANANGER_WII_CHECK_LAG_IN_UPDATES )

        ITF_MAP<StringID,SoftwareBus*>::iterator it;

        // update fades on buses 
        for ( it = m_buses.begin() ; it != m_buses.end() ; ++it )
        {
            it->second->updateFade( _dt );
        }

        // update volume on buffer instances 
        m_instanceManager->update();

        // update volume on streamed instances 
        MUSICMANAGER->musicInstanceContainerUpdate( _dt );

        // now reset volume flags 
        for ( it = m_buses.begin() ; it != m_buses.end() ; ++it )
        {
            it->second->resetVolumeUpdateFlag();
        }
    }


    //************************************
    // @name	ITF::SoundAdapter::init
    // @public 
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter::init(bbool _audioCaptureAllowed)
    {
        m_bAudioCaptureAllowed=_audioCaptureAllowed;

        static const Path soundConfig(GETPATH_ALIAS("soundconfig"));

        TEMPLATEDATABASE->addTemplateClient(soundConfig.getStringID(), this);
        m_config = TEMPLATEDATABASE->getTemplate<SoundConfig_Template>(this, soundConfig);
        if (!m_config)
        {
            ITF_FATAL_ERROR("Couldn't load sound config: %s", soundConfig.getString8().cStr());
        }

        // create and route software buses 

        const SoundConfig_Template * config = SOUND_ADAPTER->getConfig();
        u32 numBuses = config->getBusDefs().size();
        u32 u;
        for ( u = 0 ; u < numBuses ; ++u )
        {
            ITF_ASSERT_CRASH(m_buses.find(config->getBusDefs()[u].m_name) == m_buses.end(), "Bus <%s> defined twice in sound config", config->getBusDefs()[u].m_name.getDebugString() );

            SoftwareBus * bus = newAlloc( mId_AudioEngine, SoftwareBus( &config->getBusDefs()[u] ) );
            m_buses[ bus->getBusDef()->m_name ] = bus;
        }

        ITF_MAP<StringID,SoftwareBus*>::const_iterator it = m_buses.find( masterBus );
        ITF_ASSERT_CRASH(it != m_buses.end(),"You must define the master bus in your soundconfig.isg");

        for ( it = m_buses.begin() ; it != m_buses.end() ; ++it )
        {
            SoftwareBus * bus = it->second;
            u32 numOutputs = bus->getBusDef()->m_outputs.size();
            ITF_UNUSED(numOutputs);

            ITF_ASSERT_CRASH( (numOutputs == 0 && bus->getBusDef()->m_name == masterBus) || (numOutputs > 0),
                "Bus %s has no output defined", bus->getBusDef()->m_name.getDebugString() );

            ITF_ASSERT_CRASH( bus->getBusDef()->m_outputs.size() <= 1, "Bus %s has more than one output",
                bus->getBusDef()->m_name.getDebugString() );

            if ( bus->getBusDef()->m_outputs.size() )
            {
                ITF_ASSERT_CRASH(m_buses.find(bus->getBusDef()->m_outputs[0]) != m_buses.end(), "Bus %s has invalid output",
                    bus->getBusDef()->m_name.getDebugString() );

                bus->setParentGroup( m_buses[ bus->getBusDef()->m_outputs[0] ] );
            }
        }

        // check that there is no circular dependency
        for ( it = m_buses.begin() ; it != m_buses.end() ; ++it )
        {
            SoftwareBus * bus = it->second;
            SoftwareBus * parent = (SoftwareBus*) bus->getParentGroup();

            while ( parent )
            {
                ITF_ASSERT_CRASH( parent != bus, "Bus %s has circular dependency", bus->getBusDef()->m_name.getDebugString() );

                parent = (SoftwareBus*) parent->getParentGroup();
            }
        }

        return btrue;
    }


    //************************************
    // @name	ITF::SoundAdapter::shutdown
    // @public 
    // @return	void
    //************************************
    void SoundAdapter::shutdown()
    {
        ITF_MAP<StringID, SoftwareBus*>::iterator it;

        for ( it = m_buses.begin() ; it != m_buses.end() ; ++it )
        {
            SoftwareBus * bus = it->second;

            delete bus;
        }

        m_buses.clear();

        delete m_instanceManager;
        m_instanceManager = NULL;
    }


    //************************************
    // @name	ITF::SoundAdapter::isValidBus
    // @public 
    // @param	StringID _bus
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter::isValidBus(StringID _bus) const
    {
        bbool bfound = bfalse;
        for (ITF_VECTOR<BusDef>::const_iterator it = m_config->getBusDefs().begin(); it != m_config->getBusDefs().end(); ++it)
        {
            if ( (*it).m_name == _bus)
            {
                bfound = btrue;
                break;
            }
        }
        return bfound;
    }
    //************************************
    // @name	ITF::SoundAdapter::isValidLimitCategory
    // @public 
    // @param	StringID _category
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter::isValidLimitCategory(StringID _category) const
    {
        bbool bfound = bfalse;
        for (u32 i = 0; i< m_config->getLimiterDefs().size(); ++i)
        {
            if ( m_config->getLimiterDefs()[i].m_name == _category)
            {
                bfound = btrue;
                break;
            }
        }
        return bfound;
    }


    //************************************
    // @name	ITF::SoundAdapter::setVolume
    // @public 
    // @param	const SourceHandle _source
    // @param	const f32 _volume
    // @return	void
    //************************************
    void SoundAdapter::setVolume(const SourceHandle _source, const f32 _volume)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        f32 volume = _volume;

        if ( volume > 1.f )
            volume = 1.f;

        if ( volume < 0.f )
            volume = 0.f;

        if ( instance )
            instance->setLinearVolume( volume );
    }

    //************************************
    // @name	ITF::SoundAdapter::getVolume
    // @public 
    // @param	const SourceHandle _source
    // @return	ITF::f32
    //************************************
    f32  SoundAdapter::getVolume(const SourceHandle _source) const 
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        if ( instance )
            return instance->getLinearVolume();

        return 0.f;
    }


    //************************************
    // @name	ITF::SoundAdapter::setPan
    // @public 
    // @param	const SourceHandle _source
    // @param	const f32 _pan
    // @return	void
    //************************************
    void SoundAdapter::setPan(const SourceHandle _source, const f32 _pan)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        f32 pan = _pan;

        if ( _pan > 1.f )
            pan = 1.f;

        if ( _pan < -1.f )
            pan = -1.f;

        if ( instance )
            instance->setPan( pan );
    }

    //************************************
    // @name	ITF::SoundAdapter::play
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter::play(const SourceHandle _source)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        if ( instance )
        {
            instance->updateVolume( true );

            instance->start();
        }
    }

    //************************************
    // @name	ITF::SoundAdapter::resume
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter::resume(const SourceHandle _source)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        if ( instance )
            instance->resume();
    }

    //************************************
    // @name	ITF::SoundAdapter_XAudio::pause
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter::pause(const SourceHandle _source)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        if ( instance )
            instance->pause();
    }

    //************************************
    // @name	ITF::SoundAdapter::stop
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter::stop(const SourceHandle _source)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        if ( instance )
        {
            if ( instance->getIsPendingSyncStart() )
            {
                raki::AudioCallbackDisabler audioCallbackDisabler;

                instance->resetPendingSyncStart();
            }

            instance->stop();
        }
    }

    //************************************
    // @name	ITF::SoundAdapter::getPlayingVoices
    // @public 
    // @param	SafeArray<SourceHandle> & _playingVoices
    // @return	void
    //************************************
    void SoundAdapter::getPlayingVoices(SafeArray<SourceHandle> &_playingVoices)
    {
        return m_instanceManager->getPlayingVoices( _playingVoices );
    }

    //************************************
    // @name	ITF::SoundAdapter::getPauseSensitivePlayingVoices
    // @public 
    // @param	SafeArray<SourceHandle> & _playingVoices
    // @return	void
    //************************************
    void SoundAdapter::getPauseSensitivePlayingVoices(SafeArray<SourceHandle> &_playingVoices)
    {
        return m_instanceManager->getPauseSensitivePlayingVoices( _playingVoices );
    }

    //************************************
    // @name	ITF::SoundAdapter::isPlaying
    // @public 
    // @param	const SourceHandle _source
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter::isPlaying(const SourceHandle _source)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        if ( instance )
            return instance->isPlaying();

        return bfalse;
    }



    //************************************
    // @name	ITF::SoundAdapter::setPitch
    // @public 
    // @param	const SourceHandle _source
    // @param	f32 _pitch
    // @return	void
    //************************************
    void SoundAdapter::setPitch(const SourceHandle _source, f32 _pitch)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        if ( instance )
            instance->setPitch( Clamp( _pitch, 0.5f, 1.5f ) );
    }


    //************************************
    // @name	ITF::SoundAdapter::destroySource
    // @public 
    // @param	const SourceHandle _source
    // @return	void
    //************************************
    void SoundAdapter::destroySource(const SourceHandle _source)
    {
        raki::VoiceBase * instance = m_instanceManager ? m_instanceManager->getInstance( (const u32) _source ) : NULL;

        if ( instance )
            return instance->destroyVoice();
    }

    void SoundAdapter::setBusVolume(StringID _bus, Volume _volume, f32 _time)
    {
        ITF_MAP<StringID,SoftwareBus*>::const_iterator it = m_buses.find( _bus );

        if ( it != m_buses.end() )
            it->second->setDecibelVolume( _volume.dB(), _time );
    }
    
    void SoundAdapter::setBusFilter(StringID _bus,FilterType _filterType, f32 _cutoff, f32 _oneOverQ)
    {

    }
    
    void SoundAdapter::getBusFilter(StringID _bus,FilterType &_filterType, f32 &_cutoff, f32 &_oneOverQ)
    {

    }

    void SoundAdapter::setMasterVolume(f32 _linearVolume)
    {
        ITF_MAP<StringID,SoftwareBus*>::const_iterator it = m_buses.find( masterBus );

        if ( it != m_buses.end() )
            it->second->setDecibelVolume( Volume::AmplitudeRatioToDecibels( _linearVolume ) );
    }

    void SoundAdapter::enableBusReverb(StringID _bus,bbool _enable)
    {

    }

    void SoundAdapter::setBusReverbPreset(StringID _bus,ReverbPreset _preset)
    {

    }

    void SoundAdapter::resetBusses()
    {

    }

    SoftwareBus * SoundAdapter::getBus( const StringID _name )
    {
        ITF_MAP<StringID,SoftwareBus*>::iterator it = m_buses.find( _name );

        if ( it != m_buses.end() )
            return it->second;

        return NULL;
    }
#endif
} //namespace ITF
