
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONDANCEONDOOR_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionDanceOnDoor.h"
#endif // _ITF_RAY_BTACTIONDANCEONDOOR_H_

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

IMPLEMENT_OBJECT_RTTI(Ray_BTActionDanceOnDoor);

Ray_BTActionDanceOnDoor::Ray_BTActionDanceOnDoor()
: m_animComponent(NULL)
, m_danceStone(ITF_INVALID_OBJREF)
, m_waitingForSync(bfalse)
{
}

Ray_BTActionDanceOnDoor::~Ray_BTActionDanceOnDoor()
{
}

void Ray_BTActionDanceOnDoor::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_animComponent = _tree->getActor()->GetComponent<AnimatedComponent>();
}

bbool Ray_BTActionDanceOnDoor::validate( Actor* _actor ) const
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

bbool Ray_BTActionDanceOnDoor::decide( f32 _dt )
{
    return m_tree->getBlackBoard().factExists(Ray_AIFact_danceOnStone);
}

void Ray_BTActionDanceOnDoor::onActivate()
{
    Super::onActivate();

    m_tree->getBlackBoard().getFact(Ray_AIFact_danceOnStone,m_danceStone);
    m_waitingForSync = btrue;

    Actor* currentStone = AIUtils::getActor(m_danceStone);

    if ( currentStone )
    {
        m_tree->getActor()->setFlipped(currentStone->isFlipped());
    }
}

bbool Ray_BTActionDanceOnDoor::update( f32 _dt )
{
    Super::update(_dt);

    bbool allBusy;
    bbool mustWait;


    if ( !checkCanContinue(allBusy, mustWait) )
    {
        if ( allBusy )
        {
            // it's ok to dance here anyway
        }
        else
        {
            return bfalse;
        }
    }

    if (mustWait)
    {
        return btrue;
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

bbool Ray_BTActionDanceOnDoor::checkCanContinue( bbool& _allBusy, bbool& _mustWait )
{
    bbool retVal;
    _mustWait = bfalse;
    Actor* currentStone = AIUtils::getActor(m_danceStone);

    _allBusy = bfalse;

    if ( currentStone )
    {
        Ray_EventCanDanceOnStone canDance(m_tree->getActor()->getRef());

        currentStone->onEvent(&canDance);

        if ( canDance.getCanDance() )
        {

            if (canDance.getMustWaitForSartDance())
            {
                m_tree->getBlackBoard().setFact(Ray_AIFact_gotoWaitForDanceOnStone, currentStone->getPos());
                _mustWait = btrue;
                retVal = btrue;
            }
            else
            {
                retVal = btrue;
            }
        }
        else
        {
            if ( canDance.isOccupied() )
            {
                Ray_EventQueryEmptyStone queryEmpty;
                currentStone->onEvent(&queryEmpty);

                if ( queryEmpty.getEmptyStone() != ITF_INVALID_OBJREF )
                {
                    m_tree->getBlackBoard().setFact(Ray_AIFact_danceOnStone,queryEmpty.getEmptyStone());
                    m_danceStone = queryEmpty.getEmptyStone();
                    retVal = btrue;
                }
                else
                {
                    m_tree->getBlackBoard().setFact(Ray_AIFact_gotoWaitForDanceOnStone, queryEmpty.getWaitZone());
                    m_tree->getBlackBoard().setFact(Ray_AIFact_waitForDanceOnStone, queryEmpty.getDoor());

                    retVal = bfalse;
                    _allBusy = btrue;
                }
            }
            else
            {
                retVal = bfalse;
            }
        }
    }
    else
    {
        retVal = bfalse;
    }

    return retVal;
}

void Ray_BTActionDanceOnDoor::updateAnimInput()
{
    Super::updateAnimInput();

    m_animComponent->setInput(ITF_GET_STRINGID_CRC(WaitingToDance,3871966685), m_waitingForSync);
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionDanceOnDoor_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionDanceOnDoor_Template)

    SERIALIZE_MEMBER("anim", m_anim);
    SERIALIZE_MEMBER("syncRatio", m_syncRatio);
    SERIALIZE_MEMBER("syncOffset", m_syncOffset);

END_SERIALIZATION()

Ray_BTActionDanceOnDoor_Template::Ray_BTActionDanceOnDoor_Template()
: m_syncRatio(1.f)
, m_syncOffset(0.f)
{
}

}