#include "precompiled_gameplay.h"

#ifndef _ITF_AIJUMPANGLEACTION_H_
#include "gameplay/ai/Actions/AIJumpAngleAction.h"
#endif //_ITF_AIJUMPANGLEACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIJumpAngleAction)
BEGIN_VALIDATE_ACTION(AIJumpAngleAction)
    VALIDATE_ACTION_PARAM("", m_physComponent, "StickToPolylinePhysComponent mandatory");
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
AIJumpAngleAction::AIJumpAngleAction()
    : Super()
    , m_physComponent(NULL)
    , m_justStarted(bfalse)
    , m_jumpAngle(0.0f)
{
}

//------------------------------------------------------------------------------
AIJumpAngleAction::~AIJumpAngleAction()
{
}

//------------------------------------------------------------------------------
void AIJumpAngleAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
}

//------------------------------------------------------------------------------
void AIJumpAngleAction::onActivate()
{
    Super::onActivate();

    m_justStarted = btrue;

    Vec2d force = Vec2d::Up * getTemplate()->getJumpForce();
    m_physComponent->addForce(force.Rotate(m_jumpAngle));
}

//------------------------------------------------------------------------------
void AIJumpAngleAction::update( f32 _delta )
{
    if (m_justStarted)
    {
        m_justStarted = bfalse;
        return;
    }

    if (m_physComponent->getStickedEdge())
    {
        m_aiBehavior->onActionFinished();
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIJumpAngleAction_Template)
BEGIN_SERIALIZATION_CHILD(AIJumpAngleAction_Template)
    SERIALIZE_MEMBER("jumpForce", m_jumpForce);
END_SERIALIZATION()

//------------------------------------------------------------------------------
AIJumpAngleAction_Template::AIJumpAngleAction_Template()
    : Super()
    , m_jumpForce(0.0f)
{
    m_animDefaultAction = ITF_GET_STRINGID_CRC(JUMP,2374255179);
}

//------------------------------------------------------------------------------
AIJumpAngleAction_Template::~AIJumpAngleAction_Template()
{
}

} // namespace ITF

