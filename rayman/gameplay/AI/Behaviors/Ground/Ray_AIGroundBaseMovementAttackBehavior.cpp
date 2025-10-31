#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundBaseMovementAttackBehavior_Template)

    BEGIN_SERIALIZATION_ABSTRACT_CHILD(Ray_AIGroundBaseMovementAttackBehavior_Template)
    END_SERIALIZATION()
        Ray_AIGroundBaseMovementAttackBehavior_Template::Ray_AIGroundBaseMovementAttackBehavior_Template()
        : Super()
    {
    }

    Ray_AIGroundBaseMovementAttackBehavior_Template::~Ray_AIGroundBaseMovementAttackBehavior_Template()
    {
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundBaseMovementAttackBehavior)
    Ray_AIGroundBaseMovementAttackBehavior::Ray_AIGroundBaseMovementAttackBehavior()
    : Super()
    {
    }

    Ray_AIGroundBaseMovementAttackBehavior::~Ray_AIGroundBaseMovementAttackBehavior()
    {
    }
}