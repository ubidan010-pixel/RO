#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTERRECEIVEHITACTION_H_
#include "rayman/gameplay/ai/Actions/Shooter/Ray_AIShooterReceiveHitAction.h"
#endif //_ITF_RAY_AISHOOTERRECEIVEHITACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterReceiveHitAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIShooterReceiveHitAction_Template)
        SERIALIZE_MEMBER("minStunTime", m_minStunTime);
        SERIALIZE_MEMBER("maxStunTime", m_maxStunTime);
        SERIALIZE_MEMBER("loopingAnimDuration", m_loopingAnimDuration);
        SERIALIZE_MEMBER("stunAnimation", m_stunAnimation );
    END_SERIALIZATION()

    Ray_AIShooterReceiveHitAction_Template::Ray_AIShooterReceiveHitAction_Template()
        : m_minStunTime(0.f)
        , m_maxStunTime(0.f)
        , m_loopingAnimDuration( -1.f )
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterReceiveHitAction)

    Ray_AIShooterReceiveHitAction::Ray_AIShooterReceiveHitAction()
        : Super()
        , m_timeCounter(0.f)
        , m_stunned(bfalse)
    {
    }

    Ray_AIShooterReceiveHitAction::~Ray_AIShooterReceiveHitAction()
    {
    }

    void Ray_AIShooterReceiveHitAction::onActivate()
    {
        Super::onActivate();

        m_stunned = bfalse;
        m_timeCounter = Max( getTemplate()->getLoopingAnimDuration(), 0.f );
    }

    void Ray_AIShooterReceiveHitAction::update( f32 _dt )
    {
        Super::update(_dt);

        if ( m_timeCounter && ( m_stunned || ( getTemplate()->getLoopingAnimDuration() > 0.f ) ) )
        {
            m_timeCounter = Max(m_timeCounter-_dt,0.f);

            if ( !m_timeCounter )
            {
                m_aiBehavior->onActionFinished();
            }
        }
        else if ( !m_stunned )
        {
            bbool animLooping = ( m_animComponent != NULL ) ? m_animComponent->isAnimLooped() : bfalse;
            
            if ( !animLooping && isAnimFinished() )
            {
                m_timeCounter = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinStunTime(),getTemplate()->getMaxStunTime());

                if ( m_timeCounter )
                {
                    m_stunned = btrue;
                    if ( getTemplate()->getStunAnimation().isValid() )
                    {
                        playAction( getTemplate()->getStunAnimation() );
                    }
                }
                else
                {
                    m_aiBehavior->onActionFinished();
                }
            }
        }       
    }


    void Ray_AIShooterReceiveHitAction::updateAnimInputs()
    {
        Super::updateAnimInputs();

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Stunned,1059090638),static_cast<u32>(m_stunned));
        }
    }

}