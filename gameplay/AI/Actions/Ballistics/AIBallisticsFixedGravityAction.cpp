#include "precompiled_gameplay.h"

#ifndef _ITF_AIBALLISTICSFIXEDGRAVITYACTION_H_
#include "gameplay/ai/Actions/Ballistics/AIBallisticsFixedGravityAction.h"
#endif //_ITF_AIBALLISTICSFIXEDGRAVITYACTION_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIBallisticsFixedGravityAction)
BEGIN_VALIDATE_ACTION(AIBallisticsFixedGravityAction)
    VALIDATE_ACTION_PARAM("usePhysicsGravity", !(getTemplate()->getUsePhysicsGravity() && !m_physComponent), "PhysComponent mandatory to use physics gravity");
    VALIDATE_ACTION_PARAM("fixedGravity", (getTemplate()->getUsePhysicsGravity() || getTemplate()->getFixedGravity()<0.0f), "Needs a (negative) fixed gravity if not using physics gravity");
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
AIBallisticsFixedGravityAction::AIBallisticsFixedGravityAction()
    : Super()
{
}

//------------------------------------------------------------------------------
AIBallisticsFixedGravityAction::~AIBallisticsFixedGravityAction()
{
}

//------------------------------------------------------------------------------
void AIBallisticsFixedGravityAction::setupTrajectory()
{
    // set gravity
    if (getTemplate()->getUsePhysicsGravity())
    {
        if (m_physComponent)
        {
            m_gravity = PHYSWORLD->getGravity(m_actor->get2DPos(), m_actor->getDepth()).m_y;
            m_gravity *= m_physComponent->getGravityMultiplier();
        }
    }
    else
    {
        m_gravity = getTemplate()->getFixedGravity();
    }

    // compute impulse
    m_origin = m_actor->getPos();
    Vec3d toTarget = m_target - m_origin;
    m_impulse.m_x = toTarget.m_x / getDuration();
    m_impulse.m_y = (toTarget.m_y / getDuration() ) - 0.5f * m_gravity * getDuration();
    m_impulse.m_z = toTarget.m_z / getDuration();
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIBallisticsFixedGravityAction_Template)
BEGIN_SERIALIZATION_CHILD(AIBallisticsFixedGravityAction_Template)
    SERIALIZE_MEMBER("usePhysicsGravity", m_usePhysicsGravity);
    SERIALIZE_MEMBER("fixedGravity", m_fixedGravity);
END_SERIALIZATION()

//------------------------------------------------------------------------------
AIBallisticsFixedGravityAction_Template::AIBallisticsFixedGravityAction_Template()
    : Super()
    , m_usePhysicsGravity(bfalse)
    , m_fixedGravity(0.0f)
{
}

//------------------------------------------------------------------------------
AIBallisticsFixedGravityAction_Template::~AIBallisticsFixedGravityAction_Template()
{
}

} // namespace ITF
