
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_Ray_BTActionWaitForDanceOnDoor_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionWaitForDanceOnDoor.h"
#endif // _ITF_RAY_BTACTIONWAITFORDANCEONDOOR_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionWaitForDanceOnDoor);

Ray_BTActionWaitForDanceOnDoor::Ray_BTActionWaitForDanceOnDoor()
: m_animComponent(NULL)
, m_danceDoor(ITF_INVALID_OBJREF)
, m_waitingForSync(bfalse)
{
}

Ray_BTActionWaitForDanceOnDoor::~Ray_BTActionWaitForDanceOnDoor()
{
}

void Ray_BTActionWaitForDanceOnDoor::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_animComponent = _tree->getActor()->GetComponent<AnimatedComponent>();
}

bbool Ray_BTActionWaitForDanceOnDoor::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"AnimatedComponent",m_animComponent!=NULL,"%s: anim component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getAnim().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"anim",getTemplate()->getAnim().isValid(),"%s: there is no anim set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTActionWaitForDanceOnDoor::decide( f32 _dt )
{
    return m_tree->getBlackBoard().factExists(Ray_AIFact_waitForDanceOnStone);
}

void Ray_BTActionWaitForDanceOnDoor::onActivate()
{
    Super::onActivate();

    m_tree->getBlackBoard().getFact(Ray_AIFact_waitForDanceOnStone, m_danceDoor);
    m_waitingForSync = btrue;


}

bbool Ray_BTActionWaitForDanceOnDoor::update( f32 _dt )
{
    Super::update(_dt);

    if (m_danceDoor.isValid())
    {

        Actor* door = AIUtils::getActor(m_danceDoor);

        if (door)
        {
            Ray_EventQueryEmptyStone queryEmpty;
            door->onEvent(&queryEmpty);

            if ( queryEmpty.getEmptyStone() != ITF_INVALID_OBJREF )
            {
                m_tree->getBlackBoard().removeFact(Ray_AIFact_waitForDanceOnStone);
                m_tree->getBlackBoard().setFact(Ray_AIFact_danceOnStone,queryEmpty.getEmptyStone());
            }
        }
    }

    if ( m_animComponent )
    {
        m_animComponent->setAnim(getTemplate()->getAnim());
    }

    if (m_waitingForSync)
    {
        f32 metronome = METRONOME_MANAGER->getBarPercentage(getTemplate()->getSyncOffset(), getTemplate()->getSyncRatio());
        if (metronome < _dt * 2)
        {
            m_waitingForSync = bfalse;
        }
    }

    return btrue;
}

void Ray_BTActionWaitForDanceOnDoor::updateAnimInput()
{
    Super::updateAnimInput();

    m_animComponent->setInput(ITF_GET_STRINGID_CRC(WaitingToDance,3871966685), m_waitingForSync);
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionWaitForDanceOnDoor_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionWaitForDanceOnDoor_Template)

    SERIALIZE_MEMBER("anim", m_anim);
    SERIALIZE_MEMBER("syncRatio", m_syncRatio);
    SERIALIZE_MEMBER("syncOffset", m_syncOffset);

END_SERIALIZATION()

Ray_BTActionWaitForDanceOnDoor_Template::Ray_BTActionWaitForDanceOnDoor_Template()
: m_syncRatio(1.f)
, m_syncOffset(0.f)
{
}

}
