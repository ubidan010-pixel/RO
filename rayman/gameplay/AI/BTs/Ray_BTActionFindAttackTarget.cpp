
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONFINDATTACKTARGET_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionFindAttackTarget.h"
#endif // _ITF_RAY_BTACTIONFINDATTACKTARGET_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif // _ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionFindAttackTarget);

const f32 Ray_BTActionFindAttackTarget::s_thresholdY = 1.f;

Ray_BTActionFindAttackTarget::Ray_BTActionFindAttackTarget()
: m_physComponent(NULL)
{
}

void Ray_BTActionFindAttackTarget::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_physComponent = _tree->getActor()->GetComponent<StickToPolylinePhysComponent>();
}

bbool Ray_BTActionFindAttackTarget::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_physComponent )
    {
        ITF_WARNING_PARAM(_actor,"stickToPolylinePhysComponent",m_physComponent!=NULL,"%s: stickToPolylinePhysComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getFact().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"fact",getTemplate()->getFact().isValid(),"%s: missing fact to store the target",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTActionFindAttackTarget::decide( f32 _dt )
{
    ActorList allActors;
    Actor* myActor = m_tree->getActor();
    AI_MANAGER->getActorsFromLayer(myActor->getDepth(), allActors);
    Faction myFaction = 0;
    ObjectRef target = ITF_INVALID_OBJREF;
    ObjectRef prevTarget = ITF_INVALID_OBJREF;
    const Vec2d& myPos = myActor->get2DPos();
    f32 radiusSq = getTemplate()->getRadius()*getTemplate()->getRadius();

    m_tree->getBlackBoard().getFact(Ray_AIFact_faction,myFaction);
    m_tree->getBlackBoard().getFact(getTemplate()->getFact(),prevTarget);

    if ( prevTarget != ITF_INVALID_OBJREF )
    {
        Actor* prevActor = AIUtils::getActor(prevTarget);

        if ( prevActor && isValidTarget(prevActor,radiusSq,myPos) )
        {
            target = prevTarget;
        }
    }

    if ( target == ITF_INVALID_OBJREF )
    {
        const u32 allActorCount = allActors.size();

        for (u32 i = 0; i < allActorCount; i++)
        {
            Actor* actor = allActors[i];

            if ( FACTION_MANAGER->hasInteraction(myFaction, AIUtils::getFaction(actor), INTERACTION_TARGET) )
            {
                if ( !isValidTarget(actor,radiusSq,myPos) )
                {
                    continue;
                }

                target = actor->getRef();
                break;
            }
        }
    }

    if ( prevTarget != target )
    {
        if ( target == ITF_INVALID_OBJREF )
        {
            m_tree->getBlackBoard().removeFact(getTemplate()->getFact());
        }
        else
        {
            m_tree->getBlackBoard().setFact(getTemplate()->getFact(),target);
        }
    }

    return bfalse;
}

bbool Ray_BTActionFindAttackTarget::isValidTarget( Actor* _actor, f32 _radiusSq, const Vec2d& _myPos ) const
{
    f32 distToTargetSq = ( _actor->get2DPos() - _myPos ).sqrnorm();

    if ( distToTargetSq > _radiusSq )
    {
        return bfalse;
    }

    EventQueryIsDead query;

    _actor->onEvent(&query);

    if ( query.getIsDead() )
    {
        return bfalse;
    }

    if ( getTemplate()->getFilterByHeight() )
    {
        f32 yHisDist = m_physComponent->getCurrentGravityDir().dot(_actor->get2DPos());
        f32 yMyDist = m_physComponent->getCurrentGravityDir().dot(_myPos);

        if ( f32_Abs(yHisDist-yMyDist) > s_thresholdY )
        {
            return bfalse;
        }
    }

    return btrue;
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionFindAttackTarget_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionFindAttackTarget_Template)

    SERIALIZE_MEMBER("radius",m_radius);
    SERIALIZE_MEMBER("fact",m_fact);
    SERIALIZE_MEMBER("filterByHeight",m_filterByHeight);

END_SERIALIZATION()

Ray_BTActionFindAttackTarget_Template::Ray_BTActionFindAttackTarget_Template()
: m_radius(10.f)
, m_filterByHeight(btrue)
{
}

}