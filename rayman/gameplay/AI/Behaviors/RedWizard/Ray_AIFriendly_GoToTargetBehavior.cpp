#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIFriendly_GoToTargetBehavior.h"
#endif //_ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIFriendly_GoToTargetBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIFriendly_GoToTargetBehavior_Template)
    END_SERIALIZATION()

    Ray_AIFriendly_GoToTargetBehavior_Template::Ray_AIFriendly_GoToTargetBehavior_Template()
    {
    }

    Ray_AIFriendly_GoToTargetBehavior_Template::~Ray_AIFriendly_GoToTargetBehavior_Template()
    {
    }


    IMPLEMENT_OBJECT_RTTI(Ray_AIFriendly_GoToTargetBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIFriendly_GoToTargetBehavior)
    END_VALIDATE_BEHAVIOR()

    Ray_AIFriendly_GoToTargetBehavior::Ray_AIFriendly_GoToTargetBehavior()
        : m_targetReachedRadius(0.f)
        //, m_targetFlipped(bfalse)
    {
    }

    Ray_AIFriendly_GoToTargetBehavior::~Ray_AIFriendly_GoToTargetBehavior()
    {
    }


    void Ray_AIFriendly_GoToTargetBehavior::setData( Event* _event )
    {
        Ray_EventAIOrder* order = _event->DynamicCast<Ray_EventAIOrder>(ITF_GET_STRINGID_CRC(Ray_EventAIOrder,1055366852));
        if (order)
        {
            m_target = _event->getSender();
            m_targetReachedRadius = order->getTargetReachedRadius();
            //m_targetBone = StringID::InvalidId; // TODO: get target bone from order
            //m_targetFlipped = bfalse;   // TODO: get flip from order
        }
        else
        {
            ITF_ASSERT_MSG(0, "wrong order type!");
            m_target = _event->getSender();
            m_targetReachedRadius = 1.f;
        }
    }

    void Ray_AIFriendly_GoToTargetBehavior::onActivate()
    {
        Super::onActivate();

        ITF_ASSERT(m_target.isValid());
        ITF_ASSERT(m_targetReachedRadius > 0.f);
    }

    void Ray_AIFriendly_GoToTargetBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_target.invalidate();
        m_targetReachedRadius = 0.f;
        //m_targetBone.invalidate();
        //m_targetFlipped = bfalse;
    }

    bbool Ray_AIFriendly_GoToTargetBehavior::checkReachedTarget( const Vec2d& _target ) const
    {
        if ( m_physComponent->getStickedEdgeIndex() == U32_INVALID )
        {
            return bfalse;
        }

        const PolyLineEdge* edge = m_physComponent->getStickedEdge();

        if ( !edge )
        {
            return bfalse;
        }

        Vec2d vecToPos = _target-edge->getPos();
        f32 distTargetOnGround = edge->m_normalizedVector.dot(vecToPos);
        vecToPos = m_actor->get2DPos()-edge->getPos();
        f32 distActorOnGround = edge->m_normalizedVector.dot(vecToPos);
        f32 distSqr = f32_Abs(distTargetOnGround-distActorOnGround);

        if ( distSqr > (m_targetReachedRadius*m_targetReachedRadius) )
        {
            return bfalse;
        }

        return btrue;
    }

    void Ray_AIFriendly_GoToTargetBehavior::update( f32 _dt )
    {
        if ( Actor* target = m_target.getActor() )
        {
            Vec2d vecToTarget = target->get2DPos() - m_actor->get2DPos();

            m_targetDirection = vecToTarget;
            m_targetDirection.normalize();

            if ( m_currentAction == m_move )
            {
                if ( checkReachedTarget(target->get2DPos()) )
                {
                    // target reached
                    m_aiComponent->onBehaviorFinished();
                    return;
                }
            }
        }
        else
        {
            m_aiComponent->onBehaviorFinished();
            return;
        }

        Super::update(_dt);
    }

}