
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTDECIDERTARGETINRANGETOATTACK_H_
#include "rayman/gameplay/ai/BTs/Ray_BTDeciderTargetInRangeToAttack.h"
#endif // _ITF_RAY_BTDECIDERTARGETINRANGETOATTACK_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTDeciderTargetInRangeToAttack);

Ray_BTDeciderTargetInRangeToAttack::Ray_BTDeciderTargetInRangeToAttack()
: m_phantomComponent(NULL)
{
}

void Ray_BTDeciderTargetInRangeToAttack::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_phantomComponent = _tree->getActor()->GetComponent<PhantomComponent>();
}

bbool Ray_BTDeciderTargetInRangeToAttack::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_phantomComponent )
    {
        ITF_WARNING_PARAM(_actor,"PhantomComponent",m_phantomComponent!=NULL,"%s: phantom component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTDeciderTargetInRangeToAttack::decide( f32 _dt )
{
    if ( m_tree->getBlackBoard().factExists(Ray_AIFact_attackInProgress) )
    {
        return btrue;
    }

    ObjectRef target = ITF_INVALID_OBJREF;

    m_tree->getBlackBoard().getFact(getTemplate()->getFact(),target);

    if ( target == ITF_INVALID_OBJREF )
    {
        return bfalse;
    }

    EventQueryPhysicsData queryPhysEvent;
    Actor* myActor = m_tree->getActor();
    PhysContactsContainer contacts;
    PHYSWORLD->collidePhantoms(m_phantomComponent->getShapePos(), m_phantomComponent->getShapePos(), myActor->getAngle(),
        m_phantomComponent->getShape(), myActor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
        contacts);

    const u32 contactCount = contacts.size();
    bbool touchingTarget = bfalse;

    for ( u32 i = 0; i < contactCount; i++ )
    {
        const SCollidableContact& contact = contacts[i];

        if ( contact.m_collidableUserData == target )
        {
            touchingTarget = btrue;
            break;
        }
    }

    return touchingTarget;
}

IMPLEMENT_OBJECT_RTTI(Ray_BTDeciderTargetInRangeToAttack_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTDeciderTargetInRangeToAttack_Template)

    SERIALIZE_MEMBER("fact",m_fact);

END_SERIALIZATION()

Ray_BTDeciderTargetInRangeToAttack_Template::Ray_BTDeciderTargetInRangeToAttack_Template()
{
}

}