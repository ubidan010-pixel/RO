#include "precompiled_gameplay.h"

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

#ifndef _ITF_FXKIT_H_
#include "engine/gameplay/FxKit.h"
#endif //_ITF_FXKIT_H_


namespace ITF
{

	///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(FXControllerComponent_Template)
    BEGIN_SERIALIZATION(FXControllerComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("fxControlList",m_fxControlList);
		//SERIALIZE_CONTAINER_OBJECT("soundDescriptors",m_soundDescriptorTemplates);
		//SERIALIZE_CONTAINER_OBJECT("FxDescriptors",m_FxDescriptorTemplates);
        SERIALIZE_MEMBER("triggerFx",m_triggerFx);
        SERIALIZE_MEMBER("defaultFx",m_defaultFx);
    END_SERIALIZATION()

    FXControllerComponent_Template::FXControllerComponent_Template()
    {
    }

    FXControllerComponent_Template::~FXControllerComponent_Template()
    {
        for (ITF_VECTOR<FXControl*>::iterator it = m_fxControlList.begin(); it != m_fxControlList.end(); ++it)
        {
            delete *it;
        }
        m_events.clear();
        m_fxControlList.clear();
    }

    void FXControllerComponent_Template::onTemplateLoaded()
    {
        Super::onTemplateLoaded();

        m_events.clear();

        for (ITF_VECTOR<FXControl*>::iterator it = m_fxControlList.begin(); it != m_fxControlList.end(); )
        {
            FXControl * event = *it;
            if (event->m_name.isValid())
            {
                ITF_MAP<StringID,FXControl*>::iterator itEvent = m_events.find(event->m_name);
                if (itEvent != m_events.end())
                {
                    ITF_ERROR("fx: FxControl %s found twice", event->m_name.getDebugString());
                    it = m_fxControlList.erase(it);
                    delete event;
                }
                else
                {
                    m_events[event->m_name] = event;
                    event->init();
                    ++it;
                }
            }
            else
            {
                it = m_fxControlList.erase(it);
                delete event;
            }
        }

		// Tell FeedbackFXManager that there is a new potential data owner
		FEEDBACKFX_MANAGER->addOwner(getActorTemplate()->getArchetype(),getActorTemplate()->getType(), getActorTemplate()->getTemplateResourceGroup());
    }

    IMPLEMENT_OBJECT_RTTI(FXControllerComponent)
    BEGIN_SERIALIZATION_CHILD(FXControllerComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
             SERIALIZE_MEMBER("triggerFXisActive",m_triggerFXisActive);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()
    
    const String8 FXControllerComponent::s_default = "__default__";

    //************************************
    // @name	ITF::FXControllerComponent::FXControllerComponent
    // @public 
    // @return	
    //************************************
    FXControllerComponent::FXControllerComponent()
    : Super()
    , m_triggerFxHandle(U32_INVALID)
    , m_triggerFxStopOnUntrigger(bfalse)
    , m_defaultFxHandle(U32_INVALID)
    , m_soundComponent(NULL)
    , m_fxBankComponent(NULL)
    , m_animLightComponent(NULL)
    , m_dieRequested(bfalse)
    , m_playingFX(bfalse)
    , m_triggerFXisActive(bfalse)
    {
        m_fxInstances.reserve(3);
    }


    //************************************
    // @name	ITF::FXControllerComponent::~FXControllerComponent
    // @public 
    // @return	
    //************************************
    FXControllerComponent::~FXControllerComponent()
    {
        clear();
    }

    //************************************
    // @name	ITF::FXControllerComponent::clear
    // @private 
    // @return	void
    //************************************
    void FXControllerComponent::clear( )
    {
        m_stoppedAnims.clear();
        m_fxPlayOnceList.clear();
        m_fxToStop.clear();
        m_fxInstances.clear();
    }

    //************************************
    // @name	ITF::FXControllerComponent::onActorLoaded
    // @public 
    // @return	void
    //************************************
    void FXControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        stopDefaultFx();

        ITF_MAP<StringID,FXControl*>::const_iterator it = getTemplate()->getEvents().find(getTemplate()->getTriggerFx());
        if (it != getTemplate()->getEvents().end())
        {
            FXControl * fx = it->second;
            m_triggerFxStopOnUntrigger = fx->m_fxStopOnEndAnim;
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimFXEvent,3753665187),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventAnimChanged,153009624),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDie,1997554881),this);

        m_soundComponent = m_actor->GetComponent<SoundComponent>();
        m_fxBankComponent = m_actor->GetComponent<FxBankComponent>();
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();

        m_dieRequested = bfalse;
    }

    void FXControllerComponent::onCheckpointLoaded()
    {
		stopTriggerFx();
    }

    bbool FXControllerComponent::keepAlive()
    {
        for (u32 i = 0; i < m_fxInstances.size(); ++i)
        {
            if (isPlaying(i))
                return btrue;
        }
        return bfalse;
    }

    //************************************
    // @name	ITF::FXControllerComponent::onEvent
    // @public 
    // @param	Event * _event
    // @return	void
    //************************************
    void FXControllerComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if ( AnimFXEvent* animTrackEvent = _event->DynamicCast<AnimFXEvent>(ITF_GET_STRINGID_CRC(AnimFXEvent,3753665187)) )
        {
            if ( animTrackEvent->getName().isValid() )
            {
                if (animTrackEvent->getAction() == AnimFXEvent::FX_Start)
                {
                    u32 fxHandle = playMaterialFX(animTrackEvent->getName(),animTrackEvent->getFriendly(),NULL);
                    u32 boneIndex = animTrackEvent->getBoneParent();
                    if ( fxHandle != U32_INVALID && boneIndex != U32_INVALID )
                        attachToBone(fxHandle,boneIndex);
                }
                else if (animTrackEvent->getAction() == AnimFXEvent::FX_Stop)
                    stopFX(animTrackEvent->getName());
            }
        }
        else if (EventTrigger * evt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (evt->getActivated())
            {
                m_triggerFXisActive = btrue;

                if (getTemplate()->getTriggerFx().isValid() && !m_dieRequested)
                {
                    m_triggerFxHandle = playFXInternal(getTemplate()->getTriggerFx(),StringID::Invalid,-1.0f);
                }
            }
            else
            {
                m_triggerFXisActive = bfalse;

                if (m_triggerFxHandle != U32_INVALID)
                {
                    if (m_triggerFxStopOnUntrigger)
                    {
                        stopFX(m_triggerFxHandle);
                    }
                    m_triggerFxHandle = U32_INVALID;
                }
            }
        }
        else if (EventAnimChanged * onAnimChanged= _event->DynamicCast<EventAnimChanged>(ITF_GET_STRINGID_CRC(EventAnimChanged,153009624)))
        {
            m_stoppedAnims.push_back(onAnimChanged->getAnimFriendly());
        }
        else if (EventDie * eventDie = _event->DynamicCast<EventDie>(ITF_GET_STRINGID_CRC(EventDie,1997554881)))
        {
            m_dieRequested = !eventDie->isDisableOnly();
            for (u32 i = 0; i < m_fxInstances.size(); ++i)
            {
                stopFX(i,btrue);
            }
        }
    }

    //************************************
    // @name	ITF::FXControllerComponent::acquireFXInstance
    // @protected 
    // @return	ITF::u32
    //************************************
    u32 FXControllerComponent::acquireFXInstance()
    {
        u32 handle = U32_INVALID;
        u32 i = 0;
        u32 size = m_fxInstances.size();
        for (i = 0; i < size; i++)
        {
            if (m_fxInstances[i].m_control == NULL)
            {
                handle = i;
                break;
            }
        }
        if (i >= size && size <= MAX_FX_INSTANCES)
        {
            m_fxInstances.push_back(FXInstance());
            handle = i;
        }
        
        return handle;
    }

    //************************************
    // @name	ITF::FXControllerComponent::releaseFXInstance
    // @protected 
    // @param	u32 _handle
    // @return	void
    //************************************
    void FXControllerComponent::releaseFXInstance(u32 _handle)
    {
        if (_handle < m_fxInstances.size())
        {
            FXInstance & fx = m_fxInstances[_handle];
            fx.m_control    = NULL;
            fx.m_soundInstance   = ITF::SoundHandle::getInvalidHandle();
            fx.m_particleInstance = U32_INVALID;
            fx.m_soundInstances.clear();
            fx.m_particleInstances.clear();
        }
    }

    //************************************
    // @name	ITF::FXControllerComponent::playFXInternal
    // @protected 
    // @param	StringID _fxName
    // @param	const StringID & _animFriendly
    // @param	f32 _ratio
    // @param	const Vec3d & _pos
    // @return	ITF::u32
    //************************************
    u32 FXControllerComponent::playFXInternal(StringID _fxName,const StringID & _animFriendly, f32 _ratio)
    {
        if (!_fxName.isValid())
            return U32_INVALID;

        ITF_MAP<StringID,FXControl*>::const_iterator it = getTemplate()->getEvents().find(_fxName);
        if (it != getTemplate()->getEvents().end())
        {
            FXControl * fx = it->second;

            //checkAnimChange();

            u32 handle = acquireFXInstance();
            if (handle == U32_INVALID)
                return handle;

            FXInstance* currentInstance = &m_fxInstances[handle];

            currentInstance->m_control = fx;

            f32 lifeTimeMultiplier = 1.0f;

            //CHECK FLAGS
            bbool fxPlayOnce = _animFriendly.isValid()?fx->m_fxPlayOnce:bfalse;
            bbool fxStopOnEndAnim = _animFriendly.isValid()?fx->m_fxStopOnEndAnim:bfalse;

            currentInstance->m_fromAnim = _animFriendly.isValid();

            if (m_animLightComponent)
            {
                if (fxPlayOnce || fxStopOnEndAnim)
                {
                    if ( m_animLightComponent->getNumPlayingSubAnims() == 0 || !m_animLightComponent->getCurSubAnim(0) )
                    {
                        ITF_ASSERT_MSG(0, "Please check why this is happening !");
                        releaseFXInstance(handle);
                        return U32_INVALID;
                    }
                    m_currentSubAnim = m_animLightComponent->getCurSubAnim(0)->getFriendlyName();
                }
            }

            bbool addedToStopList = bfalse;
            if (fxPlayOnce)
            {
                if (find (m_fxPlayOnceList.begin(),m_fxPlayOnceList.end(),fx->m_name) == m_fxPlayOnceList.end())
                {
                    m_fxPlayOnceList.push_back(fx->m_name);
                    if (fxStopOnEndAnim)
                    {
                        Fx fxStruct;
                        fxStruct.animFriendly = _animFriendly;
                        fxStruct.handle = handle;
                        fxStruct.fxName = fx->m_name;
                        addedToStopList = btrue;
                        m_fxToStop.push_back(fxStruct);
                    }
                }
                else
                {
                    releaseFXInstance(handle);
                    return U32_INVALID;
                }
            }
            else
            {
                if (fxStopOnEndAnim)
                {
                    Fx fxStruct;
                    fxStruct.animFriendly = _animFriendly;
                    fxStruct.handle = handle;
                    fxStruct.fxName = fx->m_name;
                    addedToStopList = btrue;
                    m_fxToStop.push_back(fxStruct);
                }
            }

            // Set instance play mode
            if ( currentInstance->m_control->m_fxBoneIndex != U32_INVALID)
            {
                currentInstance->m_playMode = FXPlayMode_AttachedToBone;
            }
            else if ( currentInstance->m_control->m_fxEmitFromBase )
            {
                currentInstance->m_playMode = FXPlayMode_ActorBase;
            }
            else
                currentInstance->m_playMode = FXPlayMode_Actor;

            bbool playingSomething = bfalse;

            //PLAY SOUND
            if (fx->m_fxFlags & FXControl::flag_sound)
            {
                if (m_soundComponent)
                {
                    StringID soundToPlay = fx->m_sound;
                    if (soundToPlay.isValid())
                    {
                        currentInstance->m_soundInstance = m_soundComponent->playSound(soundToPlay,currentInstance->m_control->m_fxBoneIndex);
                        playingSomething = playingSomething || currentInstance->m_soundInstance != ITF::SoundHandle::getInvalidHandle();
                    }

                    for (u32 i = 0; i < fx->m_sounds.size(); ++i)
                    {
                        if (fx->m_sounds[i].isValid())
                        {
                            currentInstance->m_soundInstances.push_back(m_soundComponent->playSound(fx->m_sounds[i],currentInstance->m_control->m_fxBoneIndex));
                            playingSomething = playingSomething || currentInstance->m_soundInstances.back() != ITF::SoundHandle::getInvalidHandle();
                        }
                    }

                }
            }

            //PLAY FX
            if (fx->m_fxFlags & FXControl::flag_particle)
            {
                if (m_fxBankComponent )
                {
                    StringID particleToPlay =  fx->m_particle;
                        

                    ITF_ASSERT_MSG(m_fxBankComponent, "FxBankComponent not found");

                    if (particleToPlay.isValid())
                    {
                        currentInstance->m_particleInstance = m_fxBankComponent->playFx(
                            particleToPlay,
                            currentInstance->m_control->m_fxBoneIndex,
                            currentInstance->m_control->m_fxUseBoneOrientation,
                            currentInstance->m_control->m_fxEmitFromBase,
                            currentInstance->m_control->m_fxUseActorSpeed,
                            currentInstance->m_control->m_fxUseActorOrientation,
                            lifeTimeMultiplier);

                        playingSomething = playingSomething || currentInstance->m_particleInstance != U32_INVALID;
                    }

                    for (u32 i = 0; i < fx->m_particles.size(); ++i)
                    {
                        if (fx->m_particles[i].isValid())
                        {
                           currentInstance->m_particleInstances.push_back(m_fxBankComponent->playFx(
                                fx->m_particles[i],
                                currentInstance->m_control->m_fxBoneIndex,
                                currentInstance->m_control->m_fxUseBoneOrientation,
                                currentInstance->m_control->m_fxEmitFromBase,
                                currentInstance->m_control->m_fxUseActorSpeed, 
                                currentInstance->m_control->m_fxUseActorOrientation,
                                lifeTimeMultiplier));

                           playingSomething = playingSomething || currentInstance->m_particleInstances.back() != U32_INVALID;
                        }

                    }
                }
            }

            if ( !playingSomething )
            {
                if (addedToStopList)
                {
                    m_fxToStop.pop_back();

                }
                releaseFXInstance(handle);
                handle = U32_INVALID;
            }

            m_playingFX = m_playingFX || playingSomething;

            return handle;
        }
        else
            return U32_INVALID;
    }

	u32 FXControllerComponent::playFeedbackInternal( const StringID& _actor
                                                    , const StringID& _actorArchetype
                                                    , const StringID& _defaultActor
                                                    , const StringID& _action
                                                    , const StringID& _target
                                                    , const StringID& _targetArchetype
                                                    , const StringID& _defaultTarget
                                                    , const StringID& _animFriendly
                                                    , const StringID& _customCategory /*= StringID::Invalid*/ )
	{
		const FXControl * feedback = FEEDBACKFX_MANAGER->getFeedback( _actor, _actorArchetype, _defaultActor, _action,_target, _targetArchetype, _defaultTarget );

		if ( feedback != NULL )
		{
			u32 handle = acquireFXInstance();
			if (handle == U32_INVALID)
				return handle;

            FXInstance* currentInstance = &m_fxInstances[handle];

			currentInstance->m_control = feedback;

			f32 lifeTimeMultiplier = 1.0f;

			//CHECK FLAGS
			bbool fxPlayOnce = _animFriendly.isValid()?feedback->m_fxPlayOnce:bfalse;
			bbool fxStopOnEndAnim = _animFriendly.isValid()?feedback->m_fxStopOnEndAnim:bfalse;

			currentInstance->m_fromAnim = _animFriendly.isValid();

			if (m_animLightComponent)
			{
                if (fxPlayOnce || fxStopOnEndAnim)
                {
				    if ( m_animLightComponent->getNumPlayingSubAnims() == 0 || !m_animLightComponent->getCurSubAnim(0) )
				    {
					    ITF_ASSERT_MSG(0, "Please check why this is happening !");
					    releaseFXInstance(handle);
					    return U32_INVALID;
				    }

				    m_currentSubAnim = m_animLightComponent->getCurSubAnim(0)->getFriendlyName();
                }
			}

			if (fxPlayOnce)
			{
				if (find (m_fxPlayOnceList.begin(),m_fxPlayOnceList.end(),feedback->m_name) == m_fxPlayOnceList.end())
				{
					m_fxPlayOnceList.push_back(feedback->m_name);
					if (fxStopOnEndAnim)
					{
						Fx fxStruct;
						fxStruct.animFriendly = _animFriendly;
						fxStruct.handle = handle;
						fxStruct.fxName = feedback->m_name;
						m_fxToStop.push_back(fxStruct);
					}
				}
				else
				{
					releaseFXInstance(handle);
					return U32_INVALID;
				}
			}
			else
			{
				if (fxStopOnEndAnim)
				{
					Fx fxStruct;
					fxStruct.animFriendly = _animFriendly;
					fxStruct.handle = handle;
					m_fxToStop.push_back(fxStruct);
				}
			}

            // Set instance play mode
            if ( currentInstance->m_control->m_fxBoneIndex != U32_INVALID )
            {
                currentInstance->m_playMode = FXPlayMode_AttachedToBone;
            }
            else if ( currentInstance->m_control->m_fxEmitFromBase )
            {
                currentInstance->m_playMode = FXPlayMode_ActorBase;
            }

			//PLAY SOUND
			if (feedback->m_fxFlags & FXControl::flag_sound)
			{
				if (m_soundComponent)
				{
					StringID soundToPlay = feedback->m_sound;
					if (soundToPlay.isValid())
						currentInstance->m_soundInstance = m_soundComponent->playSound(soundToPlay,currentInstance->m_control->m_fxBoneIndex,FEEDBACKFX_MANAGER->getSDTemplate(soundToPlay),_customCategory);

					for (u32 i = 0; i < feedback->m_sounds.size(); ++i)
					{
						soundToPlay = feedback->m_sounds[i];
						if (soundToPlay.isValid())
							currentInstance->m_soundInstances.push_back(m_soundComponent->playSound(soundToPlay,currentInstance->m_control->m_fxBoneIndex,FEEDBACKFX_MANAGER->getSDTemplate(soundToPlay),_customCategory));
					}

				}
			}

			//PLAY FX
			if (feedback->m_fxFlags & FXControl::flag_particle)
			{
				if (m_fxBankComponent )
				{
					StringID particleToPlay		= feedback->m_particle;
                    u32 boneIndex = 0;
                    if ( m_animLightComponent && currentInstance->m_control->m_fxBoneName != U32_INVALID)
                    {
					    const StringID& boneName	= m_animLightComponent->getBoneFromGeneric(currentInstance->m_control->m_fxBoneName);
					    boneIndex				    = m_animLightComponent->getBoneIndex(boneName);
                    }

					ITF_ASSERT_MSG(m_fxBankComponent, "FxBankComponent not found");

					if (particleToPlay.isValid())
					{
						currentInstance->m_particleInstance = m_fxBankComponent->playFx(
							particleToPlay,
							boneIndex,
                            currentInstance->m_control->m_fxUseBoneOrientation,
							currentInstance->m_control->m_fxEmitFromBase,
							currentInstance->m_control->m_fxUseActorSpeed, 
                            currentInstance->m_control->m_fxUseActorOrientation,
							lifeTimeMultiplier,
							FEEDBACKFX_MANAGER->getFXTemplate(particleToPlay));
					}

					for (u32 i = 0; i < feedback->m_particles.size(); ++i)
					{
						particleToPlay = feedback->m_particles[i];
						if (particleToPlay.isValid())
						{
							currentInstance->m_particleInstances.push_back(m_fxBankComponent->playFx(
								particleToPlay,
								boneIndex,
                                currentInstance->m_control->m_fxUseBoneOrientation,
								currentInstance->m_control->m_fxEmitFromBase,
								currentInstance->m_control->m_fxUseActorSpeed,
                                currentInstance->m_control->m_fxUseActorOrientation,
								lifeTimeMultiplier,
								FEEDBACKFX_MANAGER->getFXTemplate(particleToPlay)));
						}

					}
				}
			}
            m_playingFX = m_playingFX || ( handle != U32_INVALID );

			return handle;
		}
		return U32_INVALID;
	}

    //************************************
    // @name	ITF::FXControllerComponent::playFX
    // @public 
    // @param	StringID _fxName
    // @param	const Vec3d & _pos
    // @return	ITF::u32
    //************************************
    u32 FXControllerComponent::playFX(StringID _fxName )
    {
        return playFXInternal(_fxName,StringID::Invalid,-1.f);
    }

	//************************************
    // @name	ITF::FXControllerComponent::playFX
    // @public 
    // @param	StringID _fxName
    // @param	const Vec3d & _pos
    // @return	ITF::u32
    //************************************
	u32 FXControllerComponent::playFeedback(const ObjectRef& _actorRef, const StringID& _action, const ObjectRef& _targetRef, const StringID & _animFriendly )
	{
		if ( !_actorRef.isValid() || !_targetRef.isValid() || (_action == StringID::Invalid) )
			return U32_INVALID;

		Actor * actor	= static_cast<Actor*>(GETOBJECT(_actorRef));
		Actor * target	= static_cast<Actor*>(GETOBJECT(_targetRef));
        if (!actor || !target )
			return U32_INVALID;

		return playFeedbackInternal( actor->getType()
									, actor->getArchetype()
                                    , FEEDBACKFX_MANAGER->getDefaultActor()
									, _action, target->getType()
									, target->getArchetype()
                                    , FEEDBACKFX_MANAGER->getDefaultActor()
									, _animFriendly);
	}

    //************************************
    // @name	ITF::FXControllerComponent::playMaterialFX
    // @public 
    // @param	StringID _fxName
    // @param	const Vec3d & _pos
    // @param	StringID _material
    // @return	ITF::u32
    //************************************
    u32 FXControllerComponent::playMaterialFX(StringID _fxName, const GameMaterial_Template * _mat )
    {

        StringID fx;
        const GameMaterial_Template* mat = _mat;

        if (!mat)
        {
            mat = getMaterial();
        }

        if (mat)
        {
            const FxKit * fxkit = mat->getFxKit();
            if (fxkit)
                fx = fxkit->getFx(_fxName);

            const StringID& matType         = mat->getType();
            const StringID& matArchetype    = mat->getArchetype();


		    // TEMP : for a smooth transition to feedback effects, old system is still used until all data are correctly set
		    u32 handle = playFeedbackInternal( m_actor->getType()
                                                , m_actor->getArchetype()
                                                , FEEDBACKFX_MANAGER->getDefaultActor()
                                                , _fxName
                                                , matType
                                                , matArchetype
                                                , FEEDBACKFX_MANAGER->getDefaultMaterial()
                                                , StringID::Invalid
                                                , FEEDBACKFX_MANAGER->getCustomCategory(m_actor->getType()));
		    if (handle != U32_INVALID)
                return handle;
        }

        StringID fxName = fx.isValid()?fx:_fxName;		
		return playFXInternal(fxName,StringID::Invalid,-1.f);
    }
    //************************************
    // @name	ITF::FXControllerComponent::playMaterialFX
    // @public 
    // @param	StringID _fxName
    // @param	const Vec3d & _pos
    // @param	StringID _material
    // @return	ITF::u32
    //************************************
    u32 FXControllerComponent::playMaterialFX(StringID _fxName, StringID _animFriendly , const GameMaterial_Template* _mat)
    {
        StringID fx;
    
        const GameMaterial_Template* mat = _mat;

        if (!mat)
        {
            mat = getMaterial();
        }

        if (mat)
        {
            const FxKit * fxkit = mat->getFxKit();
            if (fxkit)
                fx = fxkit->getFx(_fxName);

            const StringID& matType         = mat->getType();
            const StringID& matArchetype    = mat->getArchetype();

		    // TEMP : for a smooth transition to feedback effects, old system is still used until all data are correctly set
		    u32 handle = playFeedbackInternal( m_actor->getType()
                                                , m_actor->getArchetype()
                                                , FEEDBACKFX_MANAGER->getDefaultActor()
                                                , _fxName
                                                , matType
                                                , matArchetype
                                                , FEEDBACKFX_MANAGER->getDefaultMaterial()
                                                , _animFriendly
                                                , FEEDBACKFX_MANAGER->getCustomCategory(m_actor->getType()));
		    if ( handle != U32_INVALID )
                return handle;
        }

        StringID fxName = fx.isValid()?fx:_fxName;	
		return playFXInternal(fxName,_animFriendly,-1.f);
    }

    //************************************
    // @name	ITF::FXControllerComponent::stopFX
    // @public 
    // @param	u32 _handle
    // @return	void
    //************************************
    void FXControllerComponent::stopFX(u32 _handle, bbool _onlyStopLooping /*=bfalse*/)
    {
        if (_handle >= m_fxInstances.size())
            return;

        if (m_fxInstances[_handle].m_control != NULL)
        {
            if (m_soundComponent)
            {
                if (m_fxInstances[_handle].m_soundInstance != ITF::SoundHandle::getInvalidHandle())
                    m_soundComponent->stopSound(m_fxInstances[_handle].m_soundInstance,_onlyStopLooping);
                for (u32 i = 0; i < m_fxInstances[_handle].m_soundInstances.size(); ++i)
                {
                    m_soundComponent->stopSound(m_fxInstances[_handle].m_soundInstances[i],_onlyStopLooping);
                }
            }
            if (m_fxBankComponent)
            {
                if (m_fxInstances[_handle].m_particleInstance != U32_INVALID)
                    m_fxBankComponent->stopFx(m_fxInstances[_handle].m_particleInstance);
                for (u32 i = 0; i < m_fxInstances[_handle].m_particleInstances.size(); ++i)
                {
                    m_fxBankComponent->stopFx(m_fxInstances[_handle].m_particleInstances[i]);
                }
            }
        }
    }

    void FXControllerComponent::stopFX( StringID _fxName )
    {
        for (u32 i = 0; i < m_fxInstances.size(); ++i)
        {
            FXInstance& fx = m_fxInstances[i];
            if (fx.m_control && fx.m_control->m_name == _fxName)
            {
                removeFromStopList(i);
                stopFX(i);
            }
        }
    }

    void FXControllerComponent::stopAllFX()
    {
        for (u32 i = 0; i < m_fxInstances.size(); ++i)
        {
            FXInstance& fx = m_fxInstances[i];
            if (fx.m_control)
            {
                stopFX(i);
            }
        }

        for (FxToStopContainer::iterator it = m_fxToStop.begin(); it != m_fxToStop.end(); )
        {
            stopFX(it->handle);

            ITF_VECTOR<StringID>::iterator playOnceIt = find (m_fxPlayOnceList.begin(),m_fxPlayOnceList.end(),it->fxName);
            if ( playOnceIt != m_fxPlayOnceList.end())
                m_fxPlayOnceList.erase(playOnceIt);

            it = m_fxToStop.erase(it);
            continue;
        }
    }


    ITF_ParticleGenerator* FXControllerComponent::getParticleGenerator(u32 _handle, StringID _fxName)
    {
        if (_handle >= m_fxInstances.size())
            return NULL;

        if (m_fxInstances[_handle].m_control != NULL)
        {
            ITF_ParticleGenerator * gen = NULL;
            u32 fxInstanceIndex = U32_INVALID;
            if (m_fxBankComponent)
            {
                fxInstanceIndex = m_fxInstances[_handle].m_particleInstance;

                if (fxInstanceIndex != U32_INVALID &&
                    m_fxBankComponent->getInstanceName(fxInstanceIndex) == _fxName )
                {
                   gen = m_fxBankComponent->getParticleGenerator(fxInstanceIndex);
                    if ( gen != NULL )
                    {
                        return &(*gen);
                    }
                }

                for (u32 i = 0; i < m_fxInstances[_handle].m_particleInstances.size(); ++i)
                {
                    fxInstanceIndex = m_fxInstances[_handle].m_particleInstances[i];

                    if (fxInstanceIndex != U32_INVALID &&
                        m_fxBankComponent->getInstanceName(fxInstanceIndex) == _fxName )
                    {
                        gen = m_fxBankComponent->getParticleGenerator(fxInstanceIndex);
                        if ( gen != NULL )
                        {
                            return &(*gen);
                        }
                    }
                }
            }
        }    

        return NULL;
    }

    FxInstance* getFxInstance( FxBankComponent* m_fxBankComponent, u32 fxInstanceIndex, StringID _fxName )
    {
        if (fxInstanceIndex == U32_INVALID)
        {
            return NULL;
        }

        FxInstance* fxInstance = m_fxBankComponent->getInstance(fxInstanceIndex);

        if (!fxInstance)
        {
            return NULL;
        }

        if (!fxInstance->m_gen)
        {
            return NULL;
        }

        if ( fxInstance->m_state == FxInstance::InstanceState_Unused)
        {
            return NULL;
        }

        if ( fxInstance->m_name != _fxName )
        {
            return NULL;
        }

        return fxInstance;
    }

    void FXControllerComponent::getParticleGenerators(u32 _handle, SafeArray<ITF_ParticleGenerator*>& _generators)
    {
        if (_handle >= m_fxInstances.size())
            return;

        if (m_fxInstances[_handle].m_control != NULL)
        {
            ITF_ParticleGenerator * gen = NULL;
            u32 fxInstanceIndex = U32_INVALID;
            if (m_fxBankComponent)
            {
                fxInstanceIndex = m_fxInstances[_handle].m_particleInstance;

                if (fxInstanceIndex != U32_INVALID)
                {
                    gen = m_fxBankComponent->getParticleGenerator(fxInstanceIndex);
                    if ( gen != NULL )
                    {
                        _generators.push_back(gen);
                    }
                }

                for (u32 i = 0; i < m_fxInstances[_handle].m_particleInstances.size(); ++i)
                {
                    fxInstanceIndex = m_fxInstances[_handle].m_particleInstances[i];

                    if (fxInstanceIndex != U32_INVALID)
                    {
                        gen = m_fxBankComponent->getParticleGenerator(fxInstanceIndex);
                        if ( gen != NULL )
                        {
                            _generators.push_back(gen);
                        }
                    }
                }
            }
        }    
    }

    void FXControllerComponent::setFXPos(u32 _handle, const Vec3d& _pos)
    {
        if (_handle >= m_fxInstances.size())
            return;

        if (m_fxInstances[_handle].m_control != NULL)
        {
            m_fxInstances[_handle].m_playMode = FXPlayMode_Position;
            if (m_soundComponent)
            {
                if (m_fxInstances[_handle].m_soundInstance != ITF::SoundHandle::getInvalidHandle())
                    m_soundComponent->setSoundPos(m_fxInstances[_handle].m_soundInstance,_pos);
                for (u32 i = 0; i < m_fxInstances[_handle].m_soundInstances.size(); ++i)
                {
                    m_soundComponent->setSoundPos(m_fxInstances[_handle].m_soundInstances[i],_pos);
                }
            }
            if (m_fxBankComponent)
            {
                if (m_fxInstances[_handle].m_particleInstance != U32_INVALID)
                    m_fxBankComponent->setFXPos(m_fxInstances[_handle].m_particleInstance, _pos);
                for (u32 i = 0; i < m_fxInstances[_handle].m_particleInstances.size(); ++i)
                {
                    m_fxBankComponent->setFXPos(m_fxInstances[_handle].m_particleInstances[i], _pos);
                }
            }
        }
    }

    void FXControllerComponent::setFXPos(StringID _fxName, const Vec3d& _pos)
    {
        for (u32 i = 0; i < m_fxInstances.size(); ++i)
        {
            FXInstance& fx = m_fxInstances[i];
            if (fx.m_control && fx.m_control->m_name == _fxName)
            {
                setFXPos(i, _pos);
            }
        }
    }

    void FXControllerComponent::setFXAngle(u32 _handle, f32 _angle)
    {
        if (_handle >= m_fxInstances.size())
            return;

        if (m_fxInstances[_handle].m_control != NULL)
        {
            if (m_fxBankComponent)
            {
                if (m_fxInstances[_handle].m_particleInstance != U32_INVALID)
                    m_fxBankComponent->setFXAngle(m_fxInstances[_handle].m_particleInstance, _angle);
                for (u32 i = 0; i < m_fxInstances[_handle].m_particleInstances.size(); ++i)
                {
                    m_fxBankComponent->setFXAngle(m_fxInstances[_handle].m_particleInstances[i], _angle);
                }
            }
        }
    }

    void FXControllerComponent::setFXAngle(StringID _fxName, f32 _angle)
    {
        for (u32 i = 0; i < m_fxInstances.size(); ++i)
        {
            FXInstance& fx = m_fxInstances[i];
            if (fx.m_control && fx.m_control->m_name == _fxName)
            {
                setFXAngle(i, _angle);
            }
        }
    }

    void FXControllerComponent::attachToBone(u32 _handle, u32 _boneIndex)
    {
        if (_handle >= m_fxInstances.size() && _boneIndex == U32_INVALID)
            return;

        if (m_fxInstances[_handle].m_control != NULL)
        {
            m_fxInstances[_handle].m_playMode = FXPlayMode_AttachedToBone;

            if (m_soundComponent)
            {
                if (m_fxInstances[_handle].m_soundInstance != ITF::SoundHandle::getInvalidHandle())
                    m_soundComponent->attachToBone(m_fxInstances[_handle].m_soundInstance,_boneIndex);
                for (u32 i = 0; i < m_fxInstances[_handle].m_soundInstances.size(); ++i)
                {
                    m_soundComponent->attachToBone(m_fxInstances[_handle].m_soundInstances[i],_boneIndex);
                }
            }
            if (m_fxBankComponent)
            {
                if (m_fxInstances[_handle].m_particleInstance != U32_INVALID)
                    m_fxBankComponent->attachToBone(m_fxInstances[_handle].m_particleInstance, _boneIndex);
                for (u32 i = 0; i < m_fxInstances[_handle].m_particleInstances.size(); ++i)
                {
                    m_fxBankComponent->attachToBone(m_fxInstances[_handle].m_particleInstances[i], _boneIndex);
                }
            }
        }
    }

    void FXControllerComponent::attachToBone(StringID _fxName, u32 _boneIndex)
    {
        for (u32 i = 0; i < m_fxInstances.size(); ++i)
        {
            FXInstance& fx = m_fxInstances[i];
            if (fx.m_control && fx.m_control->m_name == _fxName)
            {
                attachToBone(i, _boneIndex);
            }
        }
    }

    //************************************
    // @name	ITF::FXControllerComponent::Update
    // @public 
    // @param	f32 _deltaTime
    // @return	void
    //************************************
    void FXControllerComponent::Update( f32 _deltaTime )
    {

        for (u32 i = 0; i < m_stoppedAnims.size(); ++i)
        {
            for (FxToStopContainer::iterator it = m_fxToStop.begin(); it != m_fxToStop.end(); )
            {
                if (it->animFriendly == m_stoppedAnims[i] )
                {
                    stopFX(it->handle);

                    ITF_VECTOR<StringID>::iterator playOnceIt = find (m_fxPlayOnceList.begin(),m_fxPlayOnceList.end(),it->fxName);
                    if ( playOnceIt != m_fxPlayOnceList.end())
                        m_fxPlayOnceList.erase(playOnceIt);

                    it = m_fxToStop.erase(it);
                    continue;
                }
                else
                {
                    ++it;
                }
            }
        }
        m_stoppedAnims.clear();

        //KILL FX THAT ARE FINISHED

        //m_fxInstances.prefetch();
        m_playingFX = bfalse;

        for (u32 i = 0; i < m_fxInstances.size(); ++i)
        {
            if (m_fxInstances[i].m_control)
            {
                if (!isPlaying(i))
                {
                    removeFromStopList(i);

                    // stopFX is needed to set particles generators in stopping states
                    stopFX(i);

                    // sound and VFX are stopped (no more particles generated), clean instance array
                    releaseFXInstance(i);
                }
            }
        }

        // Special case for triggered FXs
        if ( m_triggerFXisActive && getTemplate()->getTriggerFx().isValid() && (m_triggerFxHandle == U32_INVALID) && !m_dieRequested )
        {
            //ITF_WARNING_CATEGORY(GPP,NULL,0, "Waiting to play Ambiance");
            m_triggerFxHandle = playFXInternal(getTemplate()->getTriggerFx(),StringID::Invalid,-1.0f);
            //ITF_WARNING_CATEGORY(GPP,NULL,m_triggerFxHandle == U32_INVALID, "Playing ambiance !");
        }
    }

    //************************************
    // @name	ITF::FXControllerComponent::isPlaying
    // @public 
    // @param	u32 _handle
    // @return	ITF::bbool
    //************************************
    bbool FXControllerComponent::isPlaying(u32 _handle)
    {
        ITF_ASSERT(_handle < m_fxInstances.size());
        if (_handle >= m_fxInstances.size())
            return false;
        bbool currentIsPlaying = isSoundPlaying(_handle) || isParticlePlaying(_handle);
        m_playingFX = m_playingFX || currentIsPlaying;
        return currentIsPlaying;
    }

    //************************************
    // @name	ITF::FXControllerComponent::isSoundPlaying
    // @public
    // @param	u32 _handle
    // @return	ITF::bbool
    //************************************
    bbool FXControllerComponent::isSoundPlaying(u32 _handle)
    {
        ITF_ASSERT(_handle < m_fxInstances.size());
        if (_handle >= m_fxInstances.size())
            return false;
        if (!m_soundComponent)
            return bfalse;

        if (m_fxInstances[_handle].m_soundInstance != ITF::SoundHandle::getInvalidHandle() 
            && m_soundComponent->isPlaying(m_fxInstances[_handle].m_soundInstance))
            return btrue;

        for (u32 i = 0; i  < m_fxInstances[_handle].m_soundInstances.size(); ++i)
        {
            if (m_fxInstances[_handle].m_soundInstances[i] != ITF::SoundHandle::getInvalidHandle() 
                && m_soundComponent->isPlaying(m_fxInstances[_handle].m_soundInstances[i]))
                return btrue;

        }
        return bfalse;
    }

    //************************************
    // @name	ITF::FXControllerComponent::isParticlePlaying
    // @public 
    // @param	u32 _handle
    // @return	ITF::bbool
    //************************************
    bbool FXControllerComponent::isParticlePlaying(u32 _handle)
    {
        ITF_ASSERT(_handle < m_fxInstances.size());

        if (_handle >= m_fxInstances.size())
            return false;

        if (!m_fxBankComponent)
            return bfalse;

        if (m_fxInstances[_handle].m_particleInstance != U32_INVALID 
            && m_fxBankComponent->isPlaying(m_fxInstances[_handle].m_particleInstance))
            return btrue;

        for (u32 i = 0; i  < m_fxInstances[_handle].m_particleInstances.size(); ++i)
        {
            if (m_fxInstances[_handle].m_particleInstances[i] != U32_INVALID 
                && m_fxBankComponent->isPlaying(m_fxInstances[_handle].m_particleInstances[i]))
                return btrue;

        }
        return bfalse;
    }

    //************************************
    // @name	ITF::FXControllerComponent::getMaterial
    // @protected 
    // @return	ITF::StringID
    //************************************
    const GameMaterial_Template* FXControllerComponent::getMaterial() const
    {
        return World::getGameMaterial(m_material);
    }

    //************************************
    // @name	ITF::FXControllerComponent::onUnloadResources
    // @public 
    // @return	void
    //************************************
    void FXControllerComponent::onUnloadResources()
    {
        m_fxToStop.clear();
        m_fxPlayOnceList.clear();
    }

    //************************************
    // @name	ITF::FXControllerComponent::onBecomeActive
    // @public 
    // @return	void
    //************************************
    void FXControllerComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        startDefaultFx();

		if (m_triggerFXisActive)
		{
			startTriggerFx();
		}
	}

    //************************************
    // @name	ITF::FXControllerComponent::onBecomeInactive
    // @public 
    // @return	void
    //************************************
    void FXControllerComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        stopDefaultFx();
		stopTriggerFx();

        stopAllFX();
    }

    //************************************
    // @name	ITF::FXControllerComponent::startDefaultFx
    // @protected 
    // @return	void
    //************************************
    void FXControllerComponent::startDefaultFx()
    {
        if (m_defaultFxHandle == U32_INVALID && getTemplate()->getDefaultFx().isValid() && !m_dieRequested)
        {
            m_defaultFxHandle = playFXInternal(getTemplate()->getDefaultFx(), StringID::Invalid,-1.0f);
            ITF_WARNING_CATEGORY(Sound,m_actor,m_defaultFxHandle != U32_INVALID,"Could not play defaultFx %s, maybe too many fx already playing",getTemplate()->getDefaultFx().getDebugString());
        }
        else
        {
            m_defaultFxHandle = U32_INVALID;
        }
    }

    //************************************
    // @name	ITF::FXControllerComponent::stopDefaultFx
    // @protected 
    // @return	void
    //************************************
    void FXControllerComponent::stopDefaultFx()
    {
        if (m_defaultFxHandle != U32_INVALID)
        {
            stopFX(m_defaultFxHandle);
            m_defaultFxHandle = U32_INVALID;
        }
    }

    //************************************
    //************************************

	void FXControllerComponent::startTriggerFx()
	{
		if (m_triggerFxHandle == U32_INVALID && getTemplate()->getTriggerFx().isValid() && !m_dieRequested)
		{
			m_triggerFxHandle = playFXInternal(getTemplate()->getTriggerFx(),StringID::Invalid,-1.0f);
		}
	}

	void FXControllerComponent::stopTriggerFx()
	{
		if (m_triggerFxHandle != U32_INVALID)
		{
			stopFX(m_triggerFxHandle);
			m_triggerFxHandle = U32_INVALID;
		}
	}

    //************************************
    // @name	ITF::FXControllerComponent::onResourceReady
    // @public 
    // @return	void
    //************************************
    void FXControllerComponent::onResourceReady()
    {
        if (GetActor()->isActive())
            startDefaultFx();

        if (m_animLightComponent)
        {
            for (ITF_MAP<StringID,FXControl*>::const_iterator it = getTemplate()->getEvents().begin(); it != getTemplate()->getEvents().end(); ++it)
            {
                if ( it->second->m_fxBoneName.isValid() )
                {
                    it->second->m_fxBoneIndex = m_animLightComponent->getBoneIndex(it->second->m_fxBoneName);
                }
            }
        }
    }

    const FXControl* FXControllerComponent::getFXControl( const StringID& _fxName ) const
    {
        const ITF_VECTOR<FXControl*> fxControlList = getTemplate()->getControlList();
        for (ITF_VECTOR<FXControl*>::const_iterator it = fxControlList.begin(); it != fxControlList.end(); ++it)
        {
            const FXControl* fx = *it;
            if (fx->m_name == _fxName)
            {
                return fx;
            }
        }

        return NULL;
    }

    void FXControllerComponent::removeFromStopList( u32 i )
    {
        for (FxToStopContainer::iterator it = m_fxToStop.begin(); it != m_fxToStop.end(); )
        {
            Fx  & fx = *it;
            if (fx.fxName == m_fxInstances[i].m_control->m_name && fx.handle == i)
                it = m_fxToStop.erase(it);
            else
                ++it;
        }
    }

}

