

#include "precompiled_gameplay.h"

#ifndef _ITF_AIJUMPINDIRACTION_H_
#include "gameplay/ai/Actions/AIJumpInDirAction.h"
#endif //_ITF_AIJUMPINDIRACTION_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(AIJumpInDirAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIJumpInDirAction_Template)

        SERIALIZE_MEMBER("jumpForce",m_jumpForce);
        SERIALIZE_MEMBER("flipDirection",m_flipDirection);

    END_SERIALIZATION()

        AIJumpInDirAction_Template::AIJumpInDirAction_Template()
        : m_jumpForce(Vec2d::Zero)
        , m_flipDirection(btrue)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(JUMP,2374255179);
    }

    IMPLEMENT_OBJECT_RTTI(AIJumpInDirAction)
    AIJumpInDirAction::AIJumpInDirAction()
        : m_physComponent(NULL)
        , m_justStarted(bfalse)
    {
    }

    AIJumpInDirAction::~AIJumpInDirAction()
    {

    }

    void AIJumpInDirAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

        ITF_ASSERT(m_physComponent);

    }

    void AIJumpInDirAction::onActivate()
    {
        Super::onActivate();

        m_justStarted = btrue;

        Vec2d force = getTemplate()->getJumpForce();
        if (getTemplate()->getFlipDirection())
        {
            if (m_actor->isFlipped())
                force.m_x = -force.m_x;
        }

        m_physComponent->addForce(force);
    }

    void AIJumpInDirAction::update( f32 _delta )
    {
        if ( m_justStarted )
        {
            m_justStarted = bfalse;
            return;
        }

        if ( m_physComponent->getStickedEdge() )
        {
            m_aiBehavior->onActionFinished();
        }
    }
}

