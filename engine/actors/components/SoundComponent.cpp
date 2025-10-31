

#include "precompiled_engine.h"


#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#include "engine/sound/SoundDescriptor.h"

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

namespace ITF
{

  //  const u32 ITF::SoundHandle::getInvalidHandle() = 0xFFFFFFFF;

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(SoundComponent_Template)
    BEGIN_SERIALIZATION(SoundComponent_Template)
    SERIALIZE_CONTAINER_OBJECT("soundList",m_soundList);
    SERIALIZE_MEMBER("defaultSound",m_defaultSound);
    SERIALIZE_MEMBER("volume",m_volume);
    SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    SERIALIZE_CONTAINER("WwiseBankList", m_wwiseBankList);
    END_SERIALIZATION()

    SoundComponent_Template::SoundComponent_Template()
    {
    }

    SoundComponent_Template::~SoundComponent_Template()
    { 
        for (u32 i = 0 ; i < m_soundList.size() ; ++i )
        {             
            if (getActorTemplate())  
                m_soundList[i]->removeFromResourceGroup(getActorTemplate()->getTemplateResourceGroup());

            SF_DEL(m_soundList[i]);
        }

        m_soundList.clear();
#ifdef ITF_SUPPORT_WWISE

        for (u32 i = 0; i < m_wwiseBankList.size(); ++i)
        {
            auto bankPath = m_wwiseBankList[i].getBasename();
            AUDIO_ADAPTER->unloadBank(bankPath, this);
        }
#endif
    }

    void SoundComponent_Template::onTemplateLoaded()
    {
        Super::onTemplateLoaded();

		for ( u32 i = 0 ; i < m_soundList.size() ; ++i )
		{
            m_soundList[i]->onTemplateLoaded();
			m_soundList[i]->addToResourceGroup(getActorTemplate()->getTemplateResourceGroup());
		}
#ifdef ITF_SUPPORT_WWISE
        for (u32 i = 0; i < m_wwiseBankList.size(); ++i)
        {
            auto bankPath = m_wwiseBankList[i].getBasename();
            AUDIO_ADAPTER->loadBank(bankPath, this);
        }
#endif
    }



    IMPLEMENT_OBJECT_RTTI(SoundComponent)
    BEGIN_SERIALIZATION_CHILD(SoundComponent)
#ifndef ITF_FINAL
    BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
        SERIALIZE_MEMBER("mute",m_debugEditorMute);
    END_CONDITION_BLOCK()

    SERIALIZE_FUNCTION(postPropertyLoad,ESerialize_PropertyEdit_Load);
#endif //ITF_FINALe
    END_SERIALIZATION()
    
    BEGIN_VALIDATE_COMPONENT(SoundComponent)

    //// validate behaviors
    const u32 soundCount = m_soundList.size();
    for ( u32 i = 0; i < soundCount; i++ )
    {
        m_soundList[i].validate(_isComponentValidated);
    }
    VALIDATE_COMPONENT_PARAM_CATEGORY(Sound,"",_isComponentValidated,"Invalid SoundComponent for actor %s, see above for reasons",m_actor->getUserFriendly().cStr());

    END_VALIDATE_COMPONENT()
    //************************************
    // @name	ITF::SoundComponent::clear
    // @private 
    // @return	void
    //************************************
    void SoundComponent::clear()
    {
        for (u32 i = 0; i < MAX_NUM_INSTANCES; ++i)
        {
            clearInstance(getHandleFromIndex(i));
        }

        for (ITF_MAP<StringID,InstanceLimiter*>::iterator it = m_limiters.begin(); it != m_limiters.end(); ++it)
        {
            SF_DEL(it->second);
        }

        m_limiters.clear();
    }


    //************************************
    // @name	ITF::SoundComponent::SoundComponent
    // @public 
    // @return	
    //************************************

    SoundComponent::SoundComponent()
    : Super()
    , m_defaultSoundInstance(ITF::SoundHandle::getInvalidHandle())
    , m_soundInputUpdate(NULL)
    , m_dieRequested(bfalse)
    , m_volume(0.0f,btrue)
#ifndef ITF_FINAL
    , m_debugIsMute(bfalse)
    , m_debugEditorMute(bfalse)
#endif //ITF_FINAL

    {
        m_random.SetSeed(u32(uPtr(this)));
        m_random.Reset();

        m_instance.reserve(MAX_NUM_INSTANCES);
        //Fill in the instance list (size is MAX_NUM_INSTANCES)
        for (u32 i = 0; i < MAX_NUM_INSTANCES; ++i)
        {
           m_instance.push_back(std::pair<u32,SoundInstance*>(0,(SoundInstance*) NULL));
        }

    }

    //************************************
    // @name	ITF::SoundComponent::~SoundComponent
    // @public 
    // @return	
    //************************************
    SoundComponent::~SoundComponent()
    {
        clear();
    }

#ifndef ITF_FINAL

    //************************************
    // @name	ITF::SoundComponent::postPropertyLoad
    // @public 
    // @return	void
    //************************************
    void SoundComponent::postPropertyLoad()
    {
        debugMute(m_debugEditorMute);
    }
#endif //!ITF_FINAL

    //************************************
    // @name	ITF::SoundComponent::clearInstance
    // @protected 
    // @param	u32 _index
    // @return	void
    //************************************
    void SoundComponent::clearInstance(SoundHandle _handle)
    {
        SoundInstance * instance = getInstanceFromHandle(_handle);
        if (instance)
        {
            if (instance->getCategoryLimiter())
                instance->getCategoryLimiter()->removeInstance(GetActor()->getRef(),_handle);
            if (instance->getLimiter())
                instance->getLimiter()->removeInstance(GetActor()->getRef(),_handle);
            SOUND_MANAGER()->releaseInstance(instance);
            m_instance[getIndexFromHandle(_handle)].second = NULL;
        }
    }

    //************************************
    // @name	ITF::SoundComponent::onUnloadResources
    // @public 
    // @return	void
    //************************************
    void SoundComponent::onUnloadResources()
    {
        // stop voices
        for (u32 i = 0; i < MAX_NUM_INSTANCES; ++i)
        {
            clearInstance(getHandleFromIndex(i));
        }
    }

    //************************************
    // @name	ITF::SoundComponent::keepAlive
    // @public 
    // @param	void
    // @return	bool, true is soudn still playing
    //************************************
    bbool SoundComponent::keepAlive()
    {
        ///For each instance, if voice is allocated...
        for (u32 i = 0; i < MAX_NUM_INSTANCES; i++)
        {
            SoundInstance * instance = m_instance[i].second;
            if (instance && instance->isPlaying())
                return btrue;
        }
        return bfalse;
    }


    //************************************
    // @name	ITF::SoundComponent::Update
    // @public 
    // @param	f32 _deltaTime
    // @return	void
    //************************************
    void SoundComponent::Update( f32 _deltaTime )
    {
        ITF_ASSERT(!m_pause);


        if ( m_soundInputUpdate )
        {
            m_soundInputUpdate->updateSoundInput();
        }

        if (getTemplate()->getDefaultSound().isValid() 
            && !m_defaultSoundInstance.isValid()
            && m_actor->getResourceGroup()->isPhysicallyLoaded())
        {
            m_defaultSoundInstance = playSound(getTemplate()->getDefaultSound());
        }

        Vec3d pos = m_actor->getPos();
        const Vec3d& micPos = CAMERA->getPos();
        const Vec3d& micSpeed = micPos - m_cameraPrevPos;

        ///For each instance, if voice is allocated...
        for (u32 i = 0; i < MAX_NUM_INSTANCES; i++)
        {
            SoundInstance * instance = m_instance[i].second;
            if (instance)
            {
                if (instance->isPlaying())
                {
                    if (instance->getPlayMode() == FXPlayMode_AttachedToBone)
                    {
                        AnimLightComponent * animComponent = m_actor->GetComponent<AnimLightComponent>();
                        if (animComponent)
                        {
                            animComponent->getBonePos(instance->getBoneIndex(),pos);
                        }
                    }
                    else if (instance->getPlayMode() == FXPlayMode_Position )
                    {
                        pos = instance->getDesiredPos();
                    }
                    instance->setPos(pos);
                    instance->setMicPos(micPos);
                    instance->setMicSpeed(micSpeed);
                }
                else
                    clearInstance(getHandleFromIndex(i));
            }
        }

        m_cameraPrevPos = micPos;
    }


    // Specific add sound
    // Must be called before loadResources
    void SoundComponent::setSoundList(const SafeArray<SoundDescriptor_Template *> & soundDescriptorList)
    {
        ITF_ASSERT(!m_soundList.size());

        int index = m_soundList.size();
        u32 count = index + soundDescriptorList.size();
        m_soundList.resize(count);

        for (u32 i=index; i<count; i++)
        {
            SoundDescriptor * desc = &m_soundList[i];
            desc->init(soundDescriptorList[i-index],this,bfalse);
            desc->onActorLoaded(m_actor);
            m_sounds[desc->getTemplate()->GetName()] = i;
        }
    }

     void SoundComponent::setSoundPos(SoundHandle _handle, const Vec3d& _pos)
     {
         SoundInstance * instance = getInstanceFromHandle(_handle);
         if (instance)
         {
             instance->setDesiredPos(_pos);
             instance->setPlayMode(FXPlayMode_Position);
         }
     }

     void SoundComponent::attachToBone(SoundHandle _handle, u32 _boneIndex)
     {
         SoundInstance * instance = getInstanceFromHandle(_handle);
         if (instance)
         {
             instance->setBoneIndex(_boneIndex);
             instance->setPlayMode(FXPlayMode_AttachedToBone);
         }
     }

    //************************************
    // @name	ITF::SoundComponent::onActorLoaded
    // @public 
    // @return	void
    //************************************
    void SoundComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_volume = getTemplate()->getVolume();

        m_inputList.resize(getTemplate()->getInputList().size());

        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            Input* input = &m_inputList[i];
            const InputDesc& inputDesc = getTemplate()->getInputList()[i];

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

        //FX

        m_soundList.resize(getTemplate()->getSoundList().size());
        u32 i = 0;
        for (SoundComponent_Template::SoundList::const_iterator it = getTemplate()->getSoundList().begin(); it != getTemplate()->getSoundList().end(); ++it)
        {
            SoundDescriptor * desc = &m_soundList[i];
            desc->init((*it),this);
            desc->onActorLoaded(m_actor);
            m_sounds[desc->getTemplate()->GetName()] = i;
            ++i;
        }

        //Register this component for events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDie,1997554881),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903),this);

        m_dieRequested = bfalse;
    }

    //************************************
    // @name	ITF::SoundComponent::onActorClearComponents
    // @public 
    // @return	void
    //************************************
    void SoundComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        if (m_defaultSoundInstance != SoundHandle::getInvalidHandle())
        {
            stopSound(m_defaultSoundInstance);
            m_defaultSoundInstance = SoundHandle::getInvalidHandle();
        }

		// Release FeedbackFXManager sound descriptors
		FEEDBACKFX_MANAGER->releaseSoundDescriptors(m_actor);
		m_feedbackSounds.clear();
    }

    //************************************
    // @name	ITF::SoundComponent::playSound
    // @public 
    // @param	StringID _name
    // @param	u32 _animBoneIndex
    // @return	ITF::u32
    //************************************
    ITF::SoundHandle SoundComponent::playSound(StringID _name, u32 _animBoneIndex, const SoundDescriptor_Template * _soundDesc, const StringID& _customCategory /*= StringID::Invalid*/)
    {
		SoundDescriptor * desc = NULL;

		// A sound descriptor has been specified, then check feedback list first to get descriptor
		if ( _soundDesc )
		{
			ITF_MAP<StringID,SoundDescriptor *>::const_iterator feedback_it = m_feedbackSounds.find(_name);

			// A slot has already been taken from FeedbackFXManager ?
			if ( feedback_it != m_feedbackSounds.end() )
			{
				desc = feedback_it->second;
			}
			// Ask a free sound descriptor slot to FeedbackFXManager, and get SoundDescriptor
			else
			{
				desc = FEEDBACKFX_MANAGER->getNewSoundDescriptor( m_actor, _soundDesc, this);
				if ( desc )
				{
					m_feedbackSounds[_name] = desc;
				}
			}
		}
		else
		{
			SoundMap::const_iterator it = m_sounds.find(_name);
			desc = it != m_sounds.end() ? &m_soundList[it->second] : NULL;
		}

        ITF_WARNING_CATEGORY(Sound, NULL, desc != NULL,"Unknown sound name %s on actor %s",_name.getDebugString(),m_actor ? m_actor->getUserFriendly().cStr() : "null actor");
     //   RAKI_OUTPUT(" %s on actor %s", _name.getDebugString(), m_actor ? m_actor->getUserFriendly().cStr() : "null actor");

        if (desc)
        {
            // set custom category
            desc->setCustomCategory(_customCategory);

            u32 soundCount = desc->getTemplate()->getFileCount();
            u32 soundIndex = 0;

            if (!soundCount)
                return SoundHandle::getInvalidHandle();


 
            u32 i = 0;
            //find local instance pointer
            for (i = 0; i < MAX_NUM_INSTANCES; ++i)
            {
                if (m_instance[i].second == NULL)
                    break;
            }
            if (i >= MAX_NUM_INSTANCES)
            {
                ITF_WARNING_CATEGORY(GPP, NULL, 0,"No more available sound slots to play %s on %s",_name.getDebugString(),m_actor->getUserFriendly().cStr());
                return SoundHandle::getInvalidHandle();
            }

            InstanceLimiter * categoryLimiter = NULL;

            if (desc->getTemplate()->getLimitCategory().isValid())
            {
                categoryLimiter = SOUND_MANAGER()->getLimiter(desc->getTemplate()->getLimitCategory());
                if (categoryLimiter && categoryLimiter->canPlay())
                {
                }
                else
                {
#ifndef ITF_FINAL
                    //ITF_WARNING_CATEGORY(Sound, NULL, categoryLimiter,"No category limiter found for %s on %s",_name.getDebugString(),m_actor->getUserFriendly());
                    if ( categoryLimiter && !categoryLimiter->canPlay() )
                        //ITF_WARNING_CATEGORY(Sound, NULL, 0,"Sound Category Limit exceeded for %s on %s",_name.getDebugString(),m_actor->getUserFriendly());
#endif // !ITF_FINAL
                    return SoundHandle::getInvalidHandle();
                }
            }

            soundIndex = desc->getSoundIndex();

            ITF_WARNING(m_actor,soundIndex < desc->getResourceList().size(),"SoundComponent::playSound while trying to play sound %s invalid resource index %d should be less than %d",_name.getDebugString(),soundIndex,desc->getResourceList().size());

            if (soundIndex >= desc->getResourceList().size())
                return SoundHandle::getInvalidHandle();

            Sound *sound = static_cast<Sound *>(desc->getResourceList()[soundIndex].getResource());
            if (!sound)
                return SoundHandle::getInvalidHandle();

            if (!desc->getTemplate()->getWwiseEventGuid().isValid())
            {
                ITF_ASSERT_MSG(0, "SoundComponent::play: problem playing sound <%s>", _name.getDebugString());
                return SoundHandle::getInvalidHandle();
            }

            ITF_MAP<StringID,InstanceLimiter*>::iterator it = m_limiters.find(desc->getTemplate()->GetName());
            InstanceLimiter * newLimiter = NULL;
            InstanceLimiter * limiter = NULL;
            if (it == m_limiters.end())
            {
                //not found
                LimiterDef def = desc->getTemplate()->getLimiter();
                if (def.m_maxInstances != U32_INVALID)
                {
                    newLimiter = newAlloc(mId_AudioEngine,InstanceLimiter);
                    newLimiter->init(def);
                    limiter = newLimiter;
                }
            }
            else
            {
                //found
                limiter = it->second;
                if (!limiter->canPlay())
                {
                    //ITF_WARNING_CATEGORY(GPP, NULL, 0,"Sound Limit exceeded for %s on %s",_name.getDebugString(),m_actor->getUserFriendly().cStr());
                    return SoundHandle::getInvalidHandle();
                }
            }


            SoundInstance * instance = SOUND_MANAGER()->acquireInstance(sound, desc);

            if (instance)
            {
                if (newLimiter)
                    m_limiters[desc->getTemplate()->GetName()] = newLimiter;



#if !defined( ITF_FINAL) || defined( ITF_WWISE_PROFILE) 
    #if defined(ITF_DEBUG_STRINGID)
                instance->setDebugName(GetActor()->getUserFriendly() + "::" + desc->getTemplate()->GetName().getDebugString() + "(" + desc->getResourceList()[soundIndex].getDebugString() + ")");
    #else
                auto actorname = GetActor()->getUserFriendly();
                auto soundname = desc->getResourceList()[soundIndex].getPath();
                PathString_t name;
                soundname->getBasenameWithoutExtension(name);
                instance->setDebugName(actorname + ":" + StringConverter(name).getChar() + ")");
    #endif
#endif
                instance->setParams(GetActor()->getRef(), desc);

                instance->setBoneIndex(_animBoneIndex);
                Vec3d pos = m_actor->getPos();
                if (_animBoneIndex != U32_INVALID)
                {
                    AnimLightComponent * animComponent = m_actor->GetComponent<AnimLightComponent>();
                    if (animComponent)
                    {
                        animComponent->getBonePos(_animBoneIndex,pos);
                        instance->setPlayMode(FXPlayMode_AttachedToBone);
                    }
                }
                else
                {
                    instance->setPlayMode(FXPlayMode_Actor);
                }
                    
                instance->setPos(pos);
                instance->setMicPos(CAMERA->getPos());
                
                SOUND_MANAGER()->play(instance);
                
                m_instance[i].second = instance;

                SoundHandle handle = getHandleFromIndexAndIncrement(i);

                if (limiter)
                    limiter->addInstance(GetActor()->getRef(),handle);
                instance->setLimiter(limiter);

                if (categoryLimiter)
                    categoryLimiter->addInstance(GetActor()->getRef(),handle);
                instance->setCategoryLimiter(categoryLimiter);

               // LOG("ask to play %s on % s on bus %s", instance->getDebugName().cStr(), m_actor->getUserFriendly().cStr(), desc->getCategory().getDebugString());

                return handle;
            }
            else
            {
                ITF_WARNING_CATEGORY(GPP, NULL, 0,"Unable to acquire sound instance for %s on %s",_name.getDebugString(),m_actor->getUserFriendly().cStr());
                SF_DEL(newLimiter);
            }
        }

        return SoundHandle::getInvalidHandle();
    }

    //************************************
    // @name	ITF::SoundComponent::stopSound
    // @public 
    // @param	u32 _index
    // @return	void
    //************************************
    void SoundComponent::stopSound(SoundHandle _handle, bbool _onlyStopLooping)
    {
        SoundInstance * instance = getInstanceFromHandle(_handle);
        if (instance)
        {
            if (!_onlyStopLooping || (_onlyStopLooping && instance->getLoop()))
            {
                SOUND_MANAGER()->stop(instance);
            }
        }
    }

    //************************************
    // @name	ITF::SoundComponent::stopAll
    // @public 
    // @return	void
    //************************************
    void SoundComponent::stopAll()
    {
        for(u32 index = 0; index < m_instance.size(); ++index)
        {
            const auto handle = getHandleFromIndex(index);
            stopSound(handle);
        }
    }


    //************************************
    // @name	ITF::SoundComponent::isPlaying
    // @public 
    // @param	u32 _index
    // @return	ITF::bbool
    //************************************
    bbool SoundComponent::isPlaying(SoundHandle _handle) const
    {
        const SoundInstance *instance = getInstanceFromHandle(_handle);
        if (instance)
            return instance->isPlaying();
        else
            return bfalse;
    }

    //************************************
    // @name	ITF::SoundComponent::onEvent
    // @public 
    // @param	Event * _event
    // @return	void
    //************************************
    void SoundComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (EventTrigger * evt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (evt->getActivated())
            {
                if (getTemplate()->getDefaultSound().isValid() && !m_defaultSoundInstance.isValid() && !m_dieRequested)
                {
                    m_defaultSoundInstance = playSound(getTemplate()->getDefaultSound());
                }
            }
            else
            {
                if (m_defaultSoundInstance != SoundHandle::getInvalidHandle())
                {
                    stopSound(m_defaultSoundInstance);
                    m_defaultSoundInstance = SoundHandle::getInvalidHandle();
                }
            }
        } else if (EventDie * eventDie = _event->DynamicCast<EventDie>(ITF_GET_STRINGID_CRC(EventDie,1997554881)))
        {
            m_dieRequested = !eventDie->isDisableOnly();
            for (u32 i = 0; i < MAX_NUM_INSTANCES; ++i)
            {
                stopSound(getHandleFromIndex(i),btrue);
            }
        }
        else if (EventSetFloatInput * onSetInput = _event->DynamicCast<EventSetFloatInput>(ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645)))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
        else if (EventSetUintInput * onSetInput = _event->DynamicCast<EventSetUintInput>(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903)))
        {
            setInput(onSetInput->getInputName(),onSetInput->getInputValue());
        }
    }

    //************************************
    // @name	ITF::SoundComponent::onBecomeActive
    // @public 
    // @return	void
    //************************************
    void SoundComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if (getTemplate()->getDefaultSound().isValid() 
            && !m_defaultSoundInstance.isValid()
            && m_actor->getResourceGroup()->isPhysicallyLoaded() && !m_dieRequested)
        {
            m_defaultSoundInstance = playSound(getTemplate()->getDefaultSound());
        }

        ///For each instance, if voice is allocated...
        for (u32 i = 0; i < MAX_NUM_INSTANCES; i++)
        {
            SoundInstance * instance = m_instance[i].second;
            if (instance && instance->getPostponePlay())
            {
                if (!SOUND_MANAGER()->acquireAndPlay(instance))
                    clearInstance(getHandleFromIndex(i));
            }
        }
    }

    //************************************
    // @name	ITF::SoundComponent::onBecomeInactive
    // @public 
    // @return	void
    //************************************
    void SoundComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if (m_defaultSoundInstance != SoundHandle::getInvalidHandle())
        {
            stopSound(m_defaultSoundInstance);
            m_defaultSoundInstance = SoundHandle::getInvalidHandle();
        }

		// Release FeedbackFXManager sound descriptors
		FEEDBACKFX_MANAGER->releaseSoundDescriptors(m_actor);
		m_feedbackSounds.clear();
    }

    //************************************
    // @name	ITF::SoundComponent::getVolume
    // @public 
    // @param	u32 _index
    // @return	ITF::f32
    //************************************
    f32 SoundComponent::getVolume(SoundHandle _handle) const
    {
        SoundInstance * instance = getInstanceFromHandle(_handle);
        ITF_ASSERT(instance);
        if (instance)
            return instance->getCurrentVolume();
        return 0.0f;
    }

    ITF::f32 SoundComponent::getVolume() const
    {
        return getTemplate()->getVolume().ratio();
    }

    ITF::SoundHandle SoundComponent::getHandleFromIndexAndIncrement( u32 _index ) 
    {
        ITF_ASSERT(_index < MAX_NUM_INSTANCES);
        u32 counter = m_instance[_index].first;
        counter++;
        counter = counter % 0x00010000;
        m_instance[_index].first = counter;
        return ITF::SoundHandle((_index << 16) | counter);
    }

    u32 SoundComponent::getIndexFromHandle( ITF::SoundHandle _handle ) const
    {
        if(_handle == SoundHandle::getInvalidHandle())
            return U32_INVALID;

        u32 counter = _handle.getValueAsU32() & 0x0000FFFF;
        u32 index = (_handle.getValueAsU32() >> 16) & 0x0000FFFF;
        ITF_ASSERT(index < MAX_NUM_INSTANCES);

        u32 currentCounter = m_instance[index].first;

        if (currentCounter != counter)
            return U32_INVALID;
        else
            return index;
    }

    SoundInstance * SoundComponent::getInstanceFromHandle(ITF::SoundHandle _handle) const
    {
        u32 index = getIndexFromHandle(_handle);
        if (index < MAX_NUM_INSTANCES)
        {
            return m_instance[index].second;
        }
        return NULL;
    }

    ITF::SoundHandle SoundComponent::getHandleFromIndex( u32 _index ) const
    {
        ITF_ASSERT(_index < MAX_NUM_INSTANCES);
        return ITF::SoundHandle((_index << 16) | m_instance[_index].first);
    }

    u32 SoundComponent::findInputIndex( StringID _id ) const
    {
        for ( u32 i = 0; i < m_inputList.size(); i++ )
        {
            const Input& input = m_inputList[i];

            if ( input.getId() == _id )
            {
                return i;
            }
        }

        return U32_INVALID;
    }

}

