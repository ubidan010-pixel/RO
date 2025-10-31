#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SWINGROPECOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_SwingRopeComponent.h"
#endif //_ITF_RAY_SWINGROPECOMPONENT_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SWINGSOFTPLATFORM_H_
#include "gameplay/components/Environment/SwingSoftPlatform.h"
#endif //_ITF_SWINGSOFTPLATFORM_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_SwingRopeComponent)

BEGIN_SERIALIZATION_CHILD(Ray_SwingRopeComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("initialAngle",m_initialAngle);
    END_CONDITION_BLOCK()

    SERIALIZE_FUNCTION(postChangeProperties,ESerialize_PropertyEdit_Load);

END_SERIALIZATION()

Ray_SwingRopeComponent::Ray_SwingRopeComponent()
: m_initialAngle(bfalse,0.f)
{
}

Ray_SwingRopeComponent::~Ray_SwingRopeComponent()
{
}

void Ray_SwingRopeComponent::createSoftPlatform()
{
    SwingSoftPlatform* swingSoft = newAlloc(mId_Gameplay,SwingSoftPlatform);
    swingSoft->setMaxSwingAngle(getTemplate()->getMaxSwingAngle());
    swingSoft->setNonStiffGravityShift(m_initialAngle.ToRadians());
    swingSoft->setStiffGravityMultiplier(getTemplate()->getStiffGravityMultiplier());
    swingSoft->setStiffImpulseMultiplier(getTemplate()->getStiffImpulseMultiplier());
    swingSoft->setStiffSwingSpeedFriction(getTemplate()->getStiffSwingSpeedFriction());
    swingSoft->setNonStiffSwingSpeedFriction(getTemplate()->getNonStiffSwingSpeedFriction());
    swingSoft->setLandDragMultiplier(getTemplate()->getLandDragMultiplier());
    swingSoft->setLandDragDuration(getTemplate()->getLandDragDuration());
    swingSoft->setCanUseSmall(getTemplate()->getCanUseSmall());
    swingSoft->setCanUseNormal(getTemplate()->getCanUseNormal());
    swingSoft->setRepositionWithBones(getTemplate()->getRepositionWithBones());
    m_softPlatform = swingSoft;
}

void Ray_SwingRopeComponent::postChangeProperties()
{
    if ( m_softPlatform )
    {
        static_cast<SwingSoftPlatform*>(m_softPlatform)->setNonStiffGravityShift(m_initialAngle.ToRadians());
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_SwingRopeComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_SwingRopeComponent_Template)

    SERIALIZE_MEMBER("maxSwingAngle",m_maxSwingAngle);
    SERIALIZE_MEMBER("stiffGravityMultiplier",m_stiffGravityMultiplier);
    SERIALIZE_MEMBER("stiffImpulseMultiplier",m_stiffImpulseMultiplier);
    SERIALIZE_MEMBER("stiffSwingSpeedFriction",m_stiffSwingSpeedFriction);
    SERIALIZE_MEMBER("nonStiffSwingSpeedFriction",m_nonStiffSwingSpeedFriction);
    SERIALIZE_MEMBER("landDragMultiplier",m_landDragMultiplier);
    SERIALIZE_MEMBER("landDragDuration",m_landDragDuration);
    SERIALIZE_MEMBER("canUseSmall",m_canUseSmall);
    SERIALIZE_MEMBER("canUseNormal",m_canUseNormal);
    SERIALIZE_MEMBER("repositionWithBones",m_repositionWithBones);

END_SERIALIZATION()

Ray_SwingRopeComponent_Template::Ray_SwingRopeComponent_Template()
: m_maxSwingAngle(bfalse,MTH_PIBY2)
, m_stiffGravityMultiplier(5.f)
, m_stiffImpulseMultiplier(2.5f)
, m_stiffSwingSpeedFriction(0.95f)
, m_nonStiffSwingSpeedFriction(0.95f)
, m_landDragMultiplier(10.f)
, m_landDragDuration(0.5f)
, m_canUseNormal(btrue)
, m_canUseSmall(btrue)
, m_repositionWithBones(btrue)
{
}

}

