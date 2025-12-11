#include "precompiled_engine.h"

#include <algorithm>

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_SOUNDDESCRIPTOR_H_
#include "engine/sound/SoundDescriptor.h"
#endif //_ITF_SOUNDDESCRIPTOR_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"



#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_


#include "engine/sound/SoundConfigTemplate.h"

#include "engine/sound/common/SoundVolume.h"

namespace ITF
{

    //************************************
    // @name	ITF::sortLowestVolume
    // @public
    // @param	const InstanceRef & _left
    // @param	const InstanceRef & _right
    // @return	ITF::i32
    //************************************
    i32 sortLowestVolume(const InstanceRef & _left, const InstanceRef & _right)
    {
        f32 leftVolume = 0.f;
        f32 rightVolume = 0.f;

        Actor * actor  = ((ActorRef)(_left.m_actor)).getActor();
        ITF_ASSERT(actor);
        if (actor)
        {
            SoundComponent * sound = actor->GetComponent<SoundComponent>();
            ITF_ASSERT(sound);
            ITF_ASSERT(_left.m_handle != SoundHandle::getInvalidHandle());

            leftVolume = sound->getVolume(_left.m_handle);
        }

        actor = ((ActorRef)(_right.m_actor)).getActor();
        ITF_ASSERT(actor);
        if (actor)
        {
            SoundComponent* sound = actor->GetComponent<SoundComponent>();
            ITF_ASSERT(_right.m_handle != SoundHandle::getInvalidHandle());

            rightVolume = sound->getVolume(_right.m_handle);
        }
        return leftVolume < rightVolume;
    }

    //************************************
    // @name	ITF::sortOldest
    // @public
    // @param	const InstanceRef & _left
    // @param	const InstanceRef & _right
    // @return	ITF::i32
    //************************************
    i32 sortOldest(const InstanceRef & _left, const InstanceRef & _right)
    {
        return _left.m_time < _right.m_time;
    }


    //************************************
    // @name	ITF::InstanceLimiter::stopLowestVolume
    // @private
    // @return	void
    //************************************
    void InstanceLimiter::stopLowestVolume()
    {
        std::sort(m_instances.begin(), m_instances.end(), sortLowestVolume);

        Actor * actor  = ActorRef(m_instances[0].m_actor).getActor();
        ITF_ASSERT(actor);
        if (actor)
        {
            SoundComponent * sound = actor->GetComponent<SoundComponent>();
            ITF_ASSERT(sound);
            ITF_ASSERT(m_instances[0].m_handle != SoundHandle::getInvalidHandle());
            sound->stopSound(m_instances[0].m_handle);
        }
        m_instances.erase(m_instances.begin());

    }

    //************************************
    // @name	ITF::InstanceLimiter::stopOldest
    // @private
    // @return	void
    //************************************
    void InstanceLimiter::stopOldest()
    {
        sort(m_instances.begin(), m_instances.end(), sortOldest);

        Actor * actor  = ActorRef(m_instances[0].m_actor).getActor();
        ITF_ASSERT(actor);
        if (actor)
        {
            SoundComponent * sound = actor->GetComponent<SoundComponent>();
            ITF_ASSERT(sound);
            ITF_ASSERT(m_instances[0].m_handle != SoundHandle::getInvalidHandle());
            sound->stopSound(m_instances[0].m_handle);
        }
        m_instances.erase(m_instances.begin());
    }

    //************************************
    // @name	ITF::InstanceLimiter::init
    // @public
    // @param	const LimiterDef & _def
    // @return	void
    //************************************
    void InstanceLimiter::init(const LimiterDef & _def)
    {
        m_limitCategory = _def.m_name;
        m_maxInstances = _def.m_maxInstances;
        m_mode = (LimiterMode)_def.m_mode;
        m_curFrame = 0;
        m_soundsAddableInFrame = 0;
    }

    //************************************
    // @name	ITF::SoundManager::SoundManager
    // @public
    // @return
    //************************************
    SoundManager::SoundManager()
    {

        sound::Volume::init();
        m_instances.resize(MaxSoundInstances);
        m_streamInstances.resize(MaxStreamInstances);
        m_modifierFactory.RegisterObject<ScreenRollOff>(ITF_GET_STRINGID_CRC(ScreenRollOff,1323053104));
        m_modifierFactory.RegisterObject<ScreenRollOffXY>(ITF_GET_STRINGID_CRC(ScreenRollOffXY,1821326457));
        m_modifierFactory.RegisterObject<ZRollOff>(ITF_GET_STRINGID_CRC(ZRollOff,3667597554));
        m_modifierFactory.RegisterObject<ControlVolume>(ITF_GET_STRINGID_CRC(ControlVolume,3496911000));
        m_modifierFactory.RegisterObject<SpatializedPanning>(ITF_GET_STRINGID_CRC(SpatializedPanning,1763862685));
        m_modifierFactory.RegisterObject<Doppler>(ITF_GET_STRINGID_CRC(Doppler,4039480662));
        m_modifierFactory.RegisterObject<ControlPitch>(ITF_GET_STRINGID_CRC(ControlPitch,2612890702));
        m_modifierFactory.RegisterObject<ControlFilter>(ITF_GET_STRINGID_CRC(ControlFilter,1892405494));

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventSetBusVolume,1648440666),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventSetBusReverb,98707735),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventSetBusFilter,326081700),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventBusMix,2930162672),this);
    }
    SoundManager::~SoundManager()
    {
#ifdef ITF_SUPPORT_RAKI
        RAKIINTERFACE->uninitLowLevelAudio();
#else
        sound::Volume::term();
#endif
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventSetBusVolume,1648440666),this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventSetBusReverb,98707735),this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventSetBusFilter,326081700),this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventBusMix,2930162672),this);
        for (ITF_MAP<StringID,InstanceLimiter*>::iterator it = m_limiters.begin(); it != m_limiters.end(); ++it)
        {
            SF_DEL(it->second);
        }
        m_limiters.clear();
    }
    //************************************
    // @name	ITF::SoundManager::acquireInstance
    // @public
    // @param	SoundStreamerDataSource * _dataSource
    // @param	Sound * _sound
    // @param	SoundDescriptor * _desc
    // @return	SoundInstance*
    //************************************
    SoundInstance* SoundManager::acquireInstance(Sound * _sound, SoundDescriptor * _desc)
    {
        ITF_ASSERT(_sound);

        SoundInstance * instance = NULL;
        SourceHandle sourceHandle = InvalidSourceHandle;

        //if (_dataSource)
        //{
        //    sourceHandle = m_streamingVoiceList.createCompatibleVoice(_dataSource);
        //    if (sourceHandle == InvalidSourceHandle)
        //        return NULL;
        //}

        if ( _desc->getTemplate()->isMusic() )
        {
            // Check if there is already a music of the same name playing
            for (u32 i = 0; i < m_streamInstances.size(); ++i)
            {
                if (m_streamInstances[i].getState() != SoundInstance::InstanceState_Unused
                    && m_streamInstances[i].getSoundDescriptor()->getTemplate()->GetName() == _desc->getTemplate()->GetName())
                {
                    return &m_streamInstances[i];
                }
            }

            u32 nbChannels = _desc->getTemplate()->getParams()->getNumChannels();

            ITF_WARNING_CATEGORY( Sound, NULL, MUSICMANAGER->canCreateMusicInstanceSimple( nbChannels ), "Trying to play streamed sound (%d channels) while too many other instances are playing", nbChannels );

            if ( !MUSICMANAGER->canCreateMusicInstanceSimple( nbChannels ) )
                return NULL;

            for (u32 i = 0; i < m_streamInstances.size(); ++i)
            {
                if (m_streamInstances[i].getState() == SoundInstance::InstanceState_Unused)
                {
                    instance =  &m_streamInstances[i];
                    break;
                }
            }
        }
        else
        {
            for (u32 i = 0; i < m_instances.size(); ++i)
            {
                if (m_instances[i].getState() == SoundInstance::InstanceState_Unused)
                {
                    instance =  &m_instances[i];
                    break;
                }
            }
        }

//      GG disable it until we find a better way to handle it
//      ITF_ASSERT_CRASH(instance,"Out of sound instances (256), check for a leak or just too many sounds playing at once");
        //if ( _desc->getTemplate()->isMusic() )
        //{
        //    ITF_ASSERT_MSG(instance,"Out of sound streamed instances, check for a leak or just too many sounds playing at once");
        //}
        //else
        //{
        //    ITF_ASSERT_MSG(instance,"Out of sound buffer instances, check for a leak or just too many sounds playing at once");
        //}

        if (!instance)
        {
#ifdef ITF_SUPPORT_DEBUGFEATURE
            if (DEBUGINFO)
            {
                if ( _desc->getTemplate()->isMusic() )
                {
                    DEBUGINFO->addDebugInfoDisplay( "Out of sound streamed instances, check for a leak or just too many sounds playing at once", 5.0f );
                }
                else
                {
                    DEBUGINFO->addDebugInfoDisplay( "Out of sound buffer instances, check for a leak or just too many sounds playing at once", 5.0f );
                }
            }
#endif //ITF_SUPPORT_DEBUGFEATURE
            return NULL;
        }



        SourceInfo params;
        //initialize voice parameters
        params.m_numChannels = _desc->getTemplate()->getNumChannels();
        instance->setNumChannels(_desc->getTemplate()->getNumChannels());
        instance->setSoundDescriptor(_desc);
        params.m_loop = _desc->getTemplate()->getLoop();
        params.m_sound = _sound;
        params.m_isMusic = _desc->getTemplate()->isMusic();
        params.m_usePitch = _desc->getTemplate()->usePitch();
        params.m_useFilter = _desc->getTemplate()->useFilter();
        params.m_bus = _desc->getCategory();

        instance->setIsStreamed(params.m_isMusic);

        //create source from params
        if ( !((Resource *)(_sound))->isPhysicallyLoaded() )
        {
            // sound will be played later then
            instance->setPostponePlay(btrue);
            return instance;
        }
        else if ( params.m_isMusic )
        {
            // special handle
            sourceHandle = 0;
        }
        else if ( _desc->getTemplate()->getDelay() <= 0.f )
        {
#ifdef ITF_SUPPORT_RAKI
            sourceHandle = SOUND_ADAPTER->createSource(&params);
#else
            sourceHandle = AUDIO_ADAPTER->createSource(&params, instance);
#endif
        }

        if ( (sourceHandle == InvalidSourceHandle) && ( _desc->getTemplate()->getDelay() <= 0.f ) )
            instance = NULL;

        if ( instance )
        {
            instance->setHandle(sourceHandle);
        }

        return instance;
    }

    bbool SoundManager::acquireAndPlay(SoundInstance * _instance)
    {
        if ( _instance != NULL )
        {
            SoundDescriptor * soundDesc = _instance->getSoundDescriptor();
            ITF_ASSERT(soundDesc != NULL);

            if (soundDesc && soundDesc->getTemplate())//TODO IPA2 tmp FIX [MP] please check
            {
                SourceInfo params;
                //initialize voice parameters
                params.m_numChannels = soundDesc->getTemplate()->getNumChannels();
                _instance->setNumChannels(soundDesc->getTemplate()->getNumChannels());
                params.m_loop = soundDesc->getTemplate()->getLoop();
                params.m_isMusic = soundDesc->getTemplate()->isMusic();
                params.m_usePitch = soundDesc->getTemplate()->usePitch();
                params.m_useFilter = soundDesc->getTemplate()->useFilter();
                params.m_bus = soundDesc->getTemplate()->getCategory();
#ifdef ITF_SUPPORT_RAKI
                SourceHandle sourceHandle = SOUND_ADAPTER->createSource(&params);
#else
                SourceHandle sourceHandle = AUDIO_ADAPTER->createSource(&params, _instance);
#endif


                if (sourceHandle == InvalidSourceHandle)
                    return bfalse;


                if ( _instance )
                {
                    _instance->setHandle(sourceHandle);

                    play(_instance);

                    return btrue;
                }

            }
        }

        return bfalse;
    }

    //************************************
    // @name	ITF::SoundManager::releaseInstance
    // @public
    // @param	SoundInstance * _instance
    // @return	void
    //************************************
    void SoundManager::releaseInstance(SoundInstance * _instance)
    {
        ITF_ASSERT(_instance);
        if (_instance)
        {
            _instance->stopVoice();
            _instance->clear();
            i32 index = m_playingInstances.find(_instance);
            //ITF_ASSERT(index != -1);
            if ( index != -1 )
                m_playingInstances.eraseNoOrder(index);
        }
    }

    //************************************
    // @name	ITF::SoundManager::play
    // @public
    // @param	SoundInstance * _instance
    // @return	void
    //************************************
    void SoundManager::play(SoundInstance * _instance)
    {
        _instance->play();

        if ( m_playingInstances.find(_instance) == -1 )
            m_playingInstances.push_back(_instance);
    }

    //************************************
    // @name	ITF::SoundManager::update
    // @public
    // @param	f32 _dt
    // @return	void
    //************************************
    void SoundManager::staticUpdate(f32 _dt)
    {

#if defined(ITF_SUPPORT_RAKI)
         if (SOUND_MANAGER() && SOUND_ADAPTER ) // sound manager must be upated after the frame's camera is set, otherwise, 3d to 2d computations will be corrupted
        {
            SOUND_MANAGER()->update(_dt);
            if (SOUND_ADAPTER->isRunning())
                SOUND_ADAPTER->update(_dt);
#if !defined(ITF_CTR)
            MUSICMANAGER->update(_dt);
#endif // ITF_WII
        }
#else
         SOUND_MANAGER()->update(_dt);

         if (AUDIO_ADAPTER)
             AUDIO_ADAPTER->update(_dt);

         MUSICMANAGER->update(_dt);


#endif


    }
    void SoundManager::update(f32 _dt)
    {
#ifndef ITF_FINAL
        //DebugDraw::text(ITF_POS2D_AUTO, ITF_POS2D_AUTO, Color::white(), "Sound Instance : %d" , m_playingInstances.size());
#endif // ITF_FINAL

        for (u32 i = 0 ; i < m_playingInstances.size(); ++i)
        {
            m_playingInstances[i]->update(_dt);
        }

#ifdef ITF_SUPPORT_RAKI
        RAKIINTERFACE->update( _dt );
#endif

        updateBusMix(_dt);

    }

    //************************************
    // @name	ITF::SoundManager::stop
    // @public
    // @param	SoundInstance * _instance
    // @return	void
    //************************************
    void SoundManager::stop(SoundInstance * _instance)
    {
        ITF_ASSERT(_instance);
        _instance->stop();
    }

    void SoundManager::onEvent(Event * _event)
    {
        if (EventSetBusVolume * onSetBusVolume = _event->DynamicCast<EventSetBusVolume>(ITF_GET_STRINGID_CRC(EventSetBusVolume,1648440666)))
        {
#ifdef ITF_SUPPORT_RAKI
            ITF_ASSERT(SOUND_ADAPTER->isValidBus(onSetBusVolume->getBus()));
            SOUND_ADAPTER->setBusVolume(onSetBusVolume->getBus(),onSetBusVolume->getVolume(),onSetBusVolume->getTime());
#else
            AUDIO_ADAPTER->setBusVolume(onSetBusVolume->getBus(), onSetBusVolume->getVolume(), onSetBusVolume->getTime());

#endif
        }
        else if (EventSetBusReverb * onSetBusReverb = _event->DynamicCast<EventSetBusReverb>(ITF_GET_STRINGID_CRC(EventSetBusReverb,98707735)))
        {
#ifdef ITF_SUPPORT_RAKI
            ITF_ASSERT(SOUND_ADAPTER->isValidBus(onSetBusReverb->getBus()));
            if (onSetBusReverb->getChangeActivation())
                SOUND_ADAPTER->enableBusReverb(onSetBusReverb->getBus(),onSetBusReverb->getActivate());
            if (onSetBusReverb->getChangePreset())
                SOUND_ADAPTER->setBusReverbPreset(onSetBusReverb->getBus(),onSetBusReverb->getPreset());
#else
#endif
        }
        else if (EventSetBusFilter * onSetBusFilter = _event->DynamicCast<EventSetBusFilter>(ITF_GET_STRINGID_CRC(EventSetBusFilter,326081700)))
        {
#ifdef ITF_SUPPORT_RAKI
            ITF_ASSERT(SOUND_ADAPTER->isValidBus(onSetBusFilter->getBus()));
            FilterType type;
            f32 frequency;
            f32 Q;
            SOUND_ADAPTER->getBusFilter(onSetBusFilter->getBus(),type,frequency,Q);
            if (onSetBusFilter->getChangeFrequency())
                frequency = onSetBusFilter->getFrequency();
            if (onSetBusFilter->getChangeQ())
                Q = onSetBusFilter->getQ();
            if (onSetBusFilter->getChangeType())
                type = onSetBusFilter->getType();
            SOUND_ADAPTER->setBusFilter(onSetBusFilter->getBus(),type,frequency, Q);
#else
#endif
        }
        else if (EventBusMix * onBusMix = _event->DynamicCast<EventBusMix>(ITF_GET_STRINGID_CRC(EventBusMix,2930162672)))
        {
            if (onBusMix->getSender().isValid())
            {
                if (onBusMix->getActivate())
                {
                    onBusMix->getBusMix().m_ref = onBusMix->getSender();
                    playBusMix(&onBusMix->getBusMix());
                }
                else
                {
                    onBusMix->getBusMix().m_ref = onBusMix->getSender();
                    stopBusMix(&onBusMix->getBusMix());
                }
            }

        }
    }

    void SoundManager::init()
    {

#ifdef ITF_SUPPORT_RAKI
        const SoundConfig_Template * config = SOUND_ADAPTER->getConfig();
#else
        if (!AUDIO_ADAPTER)
            return;
        const SoundConfig_Template* config = AUDIO_ADAPTER->getConfig();
#endif
        if (config)
        {
            const SafeArray<LimiterDef> & limiterDefs = config->getLimiterDefs();
            u32 size = limiterDefs.size();
            for (u32 i = 0; i < size; ++i)
            {
                InstanceLimiter * limiter = newAlloc(mId_AudioEngine,InstanceLimiter);
                limiter->init(limiterDefs[i]);
                ITF_MAP<StringID,InstanceLimiter*>::iterator it = m_limiters.find(limiter->getLimitCategory());
                if (it != m_limiters.end() || !limiter->getLimitCategory().isValid())
                {
                    delete limiter;
                }
                else
                {
                    m_limiters[limiter->getLimitCategory()] = limiter;
                }
            }
        }

        m_busMixStack.resize(MAX_BUS_MIX);
        for (u32 i = 0; i < MAX_BUS_MIX; ++i)
            m_busMixStack[i].m_priority = i;
        if (config)
        {
            BusMix & busMix = m_busMixStack[0];
            busMix.m_busDefs = config->getBusDefs();
            m_relativeBusMixTarget.m_busDefs = config->getBusDefs();
        }

    }


    void  SoundManager::playBusMix( const BusMix * _busMix, bbool _relative  )
    {
        ITF_ASSERT(_busMix);
        if (!_busMix)
            return;

        if (_relative)
        {
            //for each bus,
            for (ITF_VECTOR<BusDef>::iterator it = m_relativeBusMixTarget.m_busDefs.begin(); it != m_relativeBusMixTarget.m_busDefs.end(); ++it)
            {
                //find bus in mix
                ITF_VECTOR<BusDef>::const_iterator it2 = find(_busMix->m_busDefs.begin(),_busMix->m_busDefs.end(),*it);

                //if found set target to proper volume
                if (it2 != _busMix->m_busDefs.end())
                {
                    it->m_volume = it2->m_volume;
                }
                //else set it to 0
                else
                {
                    it->m_volume.setdB(0.0f);
                }
            }
            //find current mix
            u32 index = 0;
            for (u32 i = MAX_BUS_MIX-1; i > 0; i-- )
            {
                if (m_busMixStack[i].m_ref.isValid())
                {
                    index = i;
                    break;
                }
            }
            setBusVolumes(index ,_busMix->m_fadeIn);

        }
        else
        {
            ITF_ASSERT(_busMix->m_ref.isValid());
            //priority 0 reserved for base mix
            if (_busMix->m_priority == 0)
                return ;
            ITF_WARNING(_busMix->m_ref.getObject(),_busMix->m_priority != 0,"Invalid priority (0)  for bus mix");
            if (_busMix->m_priority >= MAX_BUS_MIX)
                return ;
            ITF_WARNING(_busMix->m_ref.getObject(),_busMix->m_priority != 0,"Invalid priority (%d)  for bus mix, must be lower than %d",_busMix->m_priority,MAX_BUS_MIX);

            //find current mix
            u32 index = 0;
            for (u32 i = MAX_BUS_MIX-1; i > 0; i-- )
            {
                if (m_busMixStack[i].m_ref.isValid())
                {
                    index = i;
                    break;
                }
            }

            //remove previous mix if it has a duration ( we won't be going back to it )
            if (m_busMixStack[index].m_duration > 0.0f)
                m_busMixStack[index].m_ref.invalidate();

            //copy busmix
            m_busMixStack[_busMix->m_priority] = *_busMix;

            //if it is the top priority mix
            if (_busMix->m_priority >= index)
            {
                setBusVolumes(_busMix->m_priority ,_busMix->m_fadeIn);
                m_busMixTotalTimer = _busMix->m_duration;
            }
        }
    }

    void  SoundManager::setBusVolumes(u32 _priority, f32 _duration)
    {
        for (ITF_VECTOR<BusDef>::iterator it = m_relativeBusMixTarget.m_busDefs.begin(); it != m_relativeBusMixTarget.m_busDefs.end(); ++it)
        {
            //for each bus
            //find corresponding absolute volume
            i32 priority = _priority;
            while (priority >= 0)
            {
                ITF_VECTOR<BusDef>::iterator it2 = find(m_busMixStack[priority].m_busDefs.begin(),m_busMixStack[priority].m_busDefs.end(),*it);
                if (it2 != m_busMixStack[priority].m_busDefs.end())
                {
#ifdef ITF_SUPPORT_RAKI
                    Volume vol(it->m_volume.dB() + it2->m_volume.dB(),btrue);
                    SOUND_ADAPTER->setBusVolume(it->m_name,vol,_duration);
                    //LOG("Set [%s] volume : %f", it->m_name.getDebugString(), vol.dB());
                    if (static_cast<u32>(it2->m_reverbPreset) != U32_INVALID)
                    {
                        SOUND_ADAPTER->setBusReverbPreset(it->m_name,it2->m_reverbPreset);
                    }
#else
                    Volume vol(it->m_volume.dB() + it2->m_volume.dB(), btrue);
                    AUDIO_ADAPTER->setBusVolume(it->m_name, vol, _duration);
#ifndef ITF_FINAL
                    // if (static_cast<u32>(it2->m_reverbPreset) != U32_INVALID)
                    // {
                    //     // ReverbPreset reverb = it2->m_reverbPreset;
                    //     // auto name = it->m_name;
                    //     // LOG(" TO DO : reverb %s should be applied with preset  %d and volume % f", name.getDebugString(), (u32) reverb, vol.dB());
                    //   //  SOUND_ADAPTER->setBusReverbPreset(it->m_name, it2->m_reverbPreset);
                    // }
#endif

#endif
                    break;
                }
                priority--;
            }
            ITF_ASSERT(priority >= 0);
        }
    }

    void SoundManager::playBusMix( StringID _name, bbool _relative /*= bfalse*/ )
    {
#ifdef ITF_SUPPORT_RAKI
        const SoundConfig_Template * config = SOUND_ADAPTER->getConfig();
#else
        const SoundConfig_Template* config = AUDIO_ADAPTER->getConfig();
#endif
        if (config)
        {
            for (ITF_VECTOR<BusMix>::const_iterator it = config->getBusMixBank().begin(); it != config->getBusMixBank().end(); ++it)
            {
                if (it->m_name == _name)
                {
                    playBusMix(&(*it),_relative);
                    return;
                }
            }
        }
    }

    void SoundManager::stopBusMix(const BusMix * _busMix, bbool _relative )
    {
        if (_relative)
        {
            if (_busMix)
            {
                for (ITF_VECTOR<BusDef>::iterator it = m_relativeBusMixTarget.m_busDefs.begin(); it != m_relativeBusMixTarget.m_busDefs.end(); ++it)
                {
                    it->m_volume.setdB(0.0f);
                }
            }
            else
            {
                //immediate stop
                for (ITF_VECTOR<BusDef>::iterator it = m_relativeBusMixTarget.m_busDefs.begin(); it != m_relativeBusMixTarget.m_busDefs.end(); ++it)
                {
                    it->m_volume.setdB(0.0f);
                }
            }
        }
        else
        {
            ITF_ASSERT(_busMix);
            if (!_busMix)
                return;
            if (_busMix->m_priority == 0)
                return;
            ITF_WARNING(_busMix->m_ref.getObject(),_busMix->m_priority != 0,"Invalid priority (0)  for bus mix");
            if (_busMix->m_priority >= MAX_BUS_MIX)
                return ;
            ITF_WARNING(_busMix->m_ref.getObject(),_busMix->m_priority != 0,"Invalid priority (%d)  for bus mix, must be lower than %d",_busMix->m_priority,MAX_BUS_MIX);

            BusMix & busMix = m_busMixStack[_busMix->m_priority];


            //find current mix
            u32 index = 0;
            for (u32 i = MAX_BUS_MIX-1; i > 0; i-- )
            {
                if (m_busMixStack[i].m_ref.isValid())
                {
                    index = i;
                    break;
                }
            }

            if (index == _busMix->m_priority)
            {
                m_busMixTotalTimer = -1.0f;
                setBusVolumes(index,busMix.m_fadeOut);
            }
            busMix.m_ref.invalidate();
        }
    }

    void SoundManager::stopBusMix( StringID _name, bbool _relative /*= bfalse*/ )
    {
#ifdef ITF_SUPPORT_RAKI
        const SoundConfig_Template * config = SOUND_ADAPTER->getConfig();
#else
        const SoundConfig_Template* config = AUDIO_ADAPTER->getConfig();
#endif
        if (config)
        {
            for (ITF_VECTOR<BusMix>::const_iterator it = config->getBusMixBank().begin(); it != config->getBusMixBank().end(); ++it)
            {
                if (it->m_name == _name)
                {
                    stopBusMix(&(*it),_relative);
                    return;
                }
            }
        }
        if (_relative)
            stopBusMix(NULL,_relative);
    }

    ITF::bbool SoundManager::isBusMixPlaying( ObjectRef _ref, const BusMix * _busMix)
    {
        ITF_ASSERT(_busMix);
        if (!_busMix)
            return bfalse;
        if (_busMix->m_priority == 0)
            return bfalse;
        ITF_WARNING(_busMix->m_ref.getObject(),_busMix->m_priority != 0,"Invalid priority (0)  for bus mix");
        if (_busMix->m_priority >= MAX_BUS_MIX)
            return bfalse;
        ITF_WARNING(_busMix->m_ref.getObject(),_busMix->m_priority != 0,"Invalid priority (%d)  for bus mix, must be lower than %d",_busMix->m_priority,MAX_BUS_MIX);

        return m_busMixStack[_busMix->m_priority].m_ref == _ref;
    }

    void SoundManager::updateBusMix( f32 _dt )
    {
        u32 index = 0;

        for (u32 i = MAX_BUS_MIX-1; i > 0; i-- )
        {
            if (m_busMixStack[i].m_ref.isValid())
            {
                index = i;
                break;
            }
        }

        //if bus mix timer has reached 0 stop it
        if (m_busMixTotalTimer == 0.0f)
        {
            stopBusMix(&m_busMixStack[index]);
        }
        //if timer is negative don't do anything ( it is an infinite bus mix )
        if (m_busMixTotalTimer > 0.0f)
            m_busMixTotalTimer = Max(0.0f,m_busMixTotalTimer-_dt);
    }

    ITF::Volume SoundManager::getLowerPrioVolume( u32 _prio, StringID _bus )
    {
        ITF_ASSERT(_bus.isValid());
        ITF_ASSERT(_prio > 0 && _prio < MAX_BUS_MIX);
        for (i32 i = _prio - 1; i >= 0; i-- )
        {
            for (u32 j = 0; j < m_busMixStack[i].m_busDefs.size(); ++j)
            {
                BusDef & busDef = m_busMixStack[i].m_busDefs[j];

                if (busDef.m_name == _bus)
                    return busDef.m_volume;
            }
        }
        return Volume();
    }


}


