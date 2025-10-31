#include "precompiled_gameplay.h"

#ifndef _ITF_AIBALLISTICSAPEXACTION_H_
#include "gameplay/ai/Actions/Ballistics/AIBallisticsApexAction.h"
#endif //_ITF_AIBALLISTICSAPEXACTION_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIBallisticsApexAction)
BEGIN_VALIDATE_ACTION(AIBallisticsApexAction)
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
AIBallisticsApexAction::AIBallisticsApexAction()
    : Super()
{
}

//------------------------------------------------------------------------------
AIBallisticsApexAction::~AIBallisticsApexAction()
{
}

//------------------------------------------------------------------------------
void AIBallisticsApexAction::setupTrajectory()
{
    // compute impulse and "fake" gravity
    m_origin = m_actor->getPos();
    Vec3d toTarget = m_target - m_origin;
    m_impulse.m_x = toTarget.m_x / getDuration();
    m_impulse.m_y = 2.0f * toTarget.m_y / getDuration();
    m_impulse.m_z = toTarget.m_z / getDuration();
    m_gravity = -m_impulse.m_y / getDuration();
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIBallisticsApexAction_Template)
BEGIN_SERIALIZATION_CHILD(AIBallisticsApexAction_Template)
END_SERIALIZATION()

//------------------------------------------------------------------------------
AIBallisticsApexAction_Template::AIBallisticsApexAction_Template()
    : Super()
{
}

//------------------------------------------------------------------------------
AIBallisticsApexAction_Template::~AIBallisticsApexAction_Template()
{
}

} // namespace ITF
