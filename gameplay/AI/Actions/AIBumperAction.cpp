#include "precompiled_gameplay.h"

#ifndef _ITF_AIBUMPERACTION_H_
#include "gameplay/ai/Actions/AIBumperAction.h"
#endif //_ITF_AIBUMPERACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIBumperAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIBumperAction_Template)
        SERIALIZE_MEMBER("jumpForce", m_jumpForce);
    END_SERIALIZATION()

        AIBumperAction_Template::AIBumperAction_Template()
        : m_jumpForce(500.f)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(BOUNCE,421809396);
    }

    IMPLEMENT_OBJECT_RTTI(AIBumperAction)

    BEGIN_VALIDATE_ACTION(AIBumperAction)
        VALIDATE_ACTION_PARAM("", m_physComponent, "AIBumperAction requires a StickToPolylinePhysComponent");
    END_VALIDATE_ACTION()


    AIBumperAction::AIBumperAction()
        : m_physComponent(NULL)
        , m_justStarted(bfalse)
    {
    }

    AIBumperAction::~AIBumperAction()
    {
    }

    void AIBumperAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    }

    void AIBumperAction::onActivate()
    {
        Super::onActivate();

        m_justStarted = btrue;

        Vec2d jumpDir = m_direction;
        jumpDir *= getTemplate()->getJumpForce();
        m_physComponent->setSpeed(Vec2d::Zero);
        m_physComponent->addForce(jumpDir);
    }

    void AIBumperAction::update( f32 _delta )
    {
        Super::update(_delta);
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