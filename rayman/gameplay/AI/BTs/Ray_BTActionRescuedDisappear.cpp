#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONRESCUEDDISAPPEAR_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionRescuedDisappear.h"
#endif // _ITF_RAY_BTACTIONRESCUEDDISAPPEAR_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionRescuedDisappear);

Ray_BTActionRescuedDisappear::Ray_BTActionRescuedDisappear()
: m_animComponent(NULL)
, m_finished(bfalse)
, m_firstUpdate(btrue)
{
}

void Ray_BTActionRescuedDisappear::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_animComponent = _tree->getActor()->GetComponent<AnimLightComponent>();

    if ( !getTemplate()->getSpawnFX().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(_tree->getActor(), &m_fxGenerator, getTemplate()->getSpawnFX());
    }
}

void Ray_BTActionRescuedDisappear::onActivate()
{
    Super::onActivate();

    Actor* myActor = m_tree->getActor();

    StickToPolylinePhysComponent* physComponent = myActor->GetComponent<StickToPolylinePhysComponent>();

    if ( physComponent )
    {
        physComponent->unstick();
    }

    m_animComponent->setAnim(getTemplate()->getHappyAnim());

    if ( !getTemplate()->getSpawnFX().isEmpty() )
    {
        m_fxGenerator.getSpawnee(myActor->getScene(), myActor->getPos());
    }

    m_firstUpdate = btrue;
}

bbool Ray_BTActionRescuedDisappear::update( f32 _dt )
{
    Super::update(_dt);

    if ( !m_firstUpdate && !m_finished && m_animComponent->isSubAnimFinished() )
    {
        Actor* myActor = m_tree->getActor();
        myActor->disable();
        m_finished = btrue;
    }

    m_firstUpdate = bfalse;

    return btrue;
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionRescuedDisappear_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionRescuedDisappear_Template)

    SERIALIZE_MEMBER("happyAnim",m_happyAnim);
    SERIALIZE_MEMBER("spawnFX",m_spawnFX);

END_SERIALIZATION()

Ray_BTActionRescuedDisappear_Template::Ray_BTActionRescuedDisappear_Template()
{
}

}