#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ELEVATORWHEELAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_ElevatorWheelAIComponent.h"
#endif //_ITF_RAY_ELEVATORWHEELAICOMPONENT_H_

#ifndef _ITF_RAY_ELEVATORAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_ElevatorAIComponent.h"
#endif //_ITF_Ray_ElevatorAIComponent_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_ElevatorWheelAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ElevatorWheelAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_ElevatorWheelAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_animLightComponent, "Ray_ElevatorAIComponent requires an AnimLightComponent/AnimatedComponent.");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_ElevatorWheelAIComponent::Ray_ElevatorWheelAIComponent()
    :
    m_animLightComponent(NULL),
    m_requestDisable(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_ElevatorWheelAIComponent::~Ray_ElevatorWheelAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_ElevatorWheelAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        
        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796), this);

        // Get Components
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();

    }

    //*****************************************************************************

    void Ray_ElevatorWheelAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if(HitStim* trigger = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
        {
            //only send force if this is a friendly stim (sent by PCs)
            if( AIUtils::getStimFaction(trigger) == RAY_FACTION_PLAYER )
            {
                sendForce(trigger->getLevel());
            }
        }
    }

    //*****************************************************************************

    void Ray_ElevatorWheelAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        const StringID animFadeOut = ITF_GET_STRINGID_CRC(FadeOut,872188705);

        if(!m_requestDisable)
        {
            ActorBind* parentActBind = m_actor->getParentBind();
            if (parentActBind && parentActBind->m_type == ActorBind::BoneName)
            {
                // Its link with a bone ?
                Actor* parentAct = m_actor->getParent().getActor();
                if (parentAct)
                {
                    AnimLightComponent* parentAnimComponent = parentAct->GetComponent<AnimLightComponent>();
                    if (parentAnimComponent)
                    {
                        const StringID boneName(parentActBind->m_typeData);
                        u32 indexBoneBind = parentAnimComponent->getBoneIndex(boneName);

                        if(indexBoneBind != U32_INVALID)
                        {
                            AnimBoneDyn* animBone = parentAnimComponent->getBone(indexBoneBind);

                            if (animBone)
                            {
                                f32 alphaBone = animBone->m_Alpha;

                                if (alphaBone < 1.0f)
                                {
                                    m_requestDisable = btrue;
                                    m_animLightComponent->setAnim(animFadeOut);
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
           if (m_animLightComponent->isSubAnimFinished())
           {
                m_actor->disable(); 
           }
        }
    }

    //*****************************************************************************
    
    void Ray_ElevatorWheelAIComponent::sendForce(u32 _level) const
    {
        Actor* parentAct = m_actor->getParent().getActor();
        if (parentAct)
        {
            Ray_ElevatorAIComponent* elevatorComponent = parentAct->GetComponent<Ray_ElevatorAIComponent>();
            if (elevatorComponent)
            {
                elevatorComponent->addForce(_level);
            }
        }
    }

	//*****************************************************************************
	// Template
	//*****************************************************************************
	
	IMPLEMENT_OBJECT_RTTI(Ray_ElevatorWheelAIComponent_Template)

    BEGIN_SERIALIZATION(Ray_ElevatorWheelAIComponent_Template)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_ElevatorWheelAIComponent_Template::Ray_ElevatorWheelAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************

    Ray_ElevatorWheelAIComponent_Template::~Ray_ElevatorWheelAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************

	
};