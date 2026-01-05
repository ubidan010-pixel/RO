#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BUBBLEPRIZE_H_
#include "rayman/gameplay/AI/Behaviors/BubblePrize/Ray_BubblePrize.h"
#endif //_ITF_RAY_BUBBLEPRIZE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/ActorsManager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AIScoreLumBehavior.h"
#endif //_ITF_RAY_AISCORELUMSBEHAVIOR_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

namespace ITF {

//------------------------------------------------------------------------------
BubblePrizeContent::BubblePrizeContent()
    : m_popEvent(NULL)
    , m_popSpawneeGenerator()
{
}

//------------------------------------------------------------------------------
BubblePrizeContent::~BubblePrizeContent()
{
    SF_DEL(m_popEvent);
}

//------------------------------------------------------------------------------
BEGIN_SERIALIZATION(BubblePrizeContent_Template)
    SERIALIZE_OBJECT_WITH_FACTORY("popEvent", m_popEvent, ACTORSMANAGER->getEventFactory());
    SERIALIZE_MEMBER("popSpawn", m_popSpawn);
    SERIALIZE_ENUM_BEGIN("bankState", m_bankState);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_Invalid);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_Darktoon);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_Heart);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_Lum);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_LumKing);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_SuperPunch_basic);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_SuperPunch_seeking);
		SERIALIZE_ENUM_VAR(BubblePrizeBankState_SwarmRepeller);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_RedLum);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_Lum_x10);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_RedLum_x10);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_Lum_x5);
        SERIALIZE_ENUM_VAR(BubblePrizeBankState_RedLum_x5);
    SERIALIZE_ENUM_END();
    SERIALIZE_MEMBER("rewardNumberChangeEnable", m_rewardNumberChangeEnable );
END_SERIALIZATION()

//------------------------------------------------------------------------------
BubblePrizeContent_Template::BubblePrizeContent_Template()
    : m_popEvent(NULL)
    , m_popSpawn()
    , m_bankState(BubblePrizeBankState_Invalid)
    , m_rewardNumberChangeEnable( bfalse )
{
}

//------------------------------------------------------------------------------
BubblePrizeContent_Template::~BubblePrizeContent_Template()
{
    SF_DEL(m_popEvent);
}

//------------------------------------------------------------------------------
BEGIN_VALIDATE_OBJECT(Ray_BubblePrize)
    VALIDATE_PARAM("contentList", !getTemplate()->getContentList().empty(), "At least one BubblePrizeContent mandatory");
    VALIDATE_PARAM("contentChangeDelay", getTemplate()->getContentChangeDelay()>0.0f, "Positive duration mandatory");
END_VALIDATE_OBJECT()

//------------------------------------------------------------------------------
Ray_BubblePrize::Ray_BubblePrize()
    : m_actorRef()
    , m_contentIndex(0)
    , m_contentChangeTimer(0.0f)
    , m_contentList()
    , m_template(NULL)
{
}

//------------------------------------------------------------------------------
Ray_BubblePrize::~Ray_BubblePrize()
{
}

//------------------------------------------------------------------------------
void Ray_BubblePrize::init(Actor* _actor, const Ray_BubblePrize_Template* _template)
{
    // store actor ref
    ITF_ASSERT(_actor);
    m_actorRef = _actor->getRef();
    
    // store template pointer
    ITF_ASSERT(_template);
    m_template = _template;

    // initialize content list
    m_contentList.resize(getTemplate()->getContentList().size());
    for (u32 i=0; i<m_contentList.size(); ++i)
    {
        // clone pop event
        Event* clonedPopEvent = NULL;
        if (getTemplate()->getContentList()[i].getPopEvent())
        {
            clonedPopEvent = getTemplate()->getContentList()[i].getPopEvent()->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
            BinaryClone(const_cast<Event*>(getTemplate()->getContentList()[i].getPopEvent()), clonedPopEvent);
        }
        m_contentList[i].setPopEvent(clonedPopEvent);

        // initialize spawnee generators
        if (!getTemplate()->getContentList()[i].getPopSpawn().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(_actor,
                &m_contentList[i].getPopSpawneeGenerator(),
                getTemplate()->getContentList()[i].getPopSpawn());
        }
    }
}

//------------------------------------------------------------------------------
void Ray_BubblePrize::update(f32 _dt)
{
    // change content once in a while if there's more than one of them
    u32 count = m_contentList.size();
    if (!getTemplate()->isHeart() && count > 1)
    {
        if ((m_contentChangeTimer += _dt) > getTemplate()->getContentChangeDelay())
        {
            m_contentIndex = (m_contentIndex + 1) % count;
            m_contentChangeTimer -= getTemplate()->getContentChangeDelay();
        }
    }
}

//------------------------------------------------------------------------------
BubblePrizeBankState Ray_BubblePrize::getBankState() const
{
    BubblePrizeBankState bankState = getTemplate()->getContentList()[m_contentIndex].getBankState();
    
    // replace lums by red lums when needed
    Ray_AIScoreLumBehavior* scoreLumBehavior = Ray_AIUtils::fetchScoreLumBehavior();
    if (scoreLumBehavior &&
        scoreLumBehavior->getGlobalLumColor() == Ray_AIScoreLumBehavior::Color_Red)
    {
        if(bankState == BubblePrizeBankState_Lum)
            bankState = BubblePrizeBankState_RedLum;
        else if (bankState == BubblePrizeBankState_Lum_x10)
            bankState = BubblePrizeBankState_RedLum_x10;
        else if (bankState == BubblePrizeBankState_Lum_x5)
            bankState = BubblePrizeBankState_RedLum_x5;
    }
    
    return bankState;
}

//------------------------------------------------------------------------------
void Ray_BubblePrize::pop(const ActorRef& _receiverRef)
{
    // get actor pointer
    Actor* actor = m_actorRef.getActor();
    ITF_ASSERT(actor);
    if (!actor)
    {
        return;
    }

    // pop event
    Event* popEvent = m_contentList[m_contentIndex].getPopEvent();
    if (popEvent)
    {
        popEvent->setSender(actor->getRef());

        // rewards spawn handled by the game manager
        if (Ray_EventSpawnReward* spawnReward = popEvent->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727)))
        {
            spawnReward->clear();
            spawnReward->setup(actor);
            spawnReward->setRewardReceiver(_receiverRef);
            GAMEMANAGER->onEvent(spawnReward);
        }
        // power ups handled by the player controller
        else if (Ray_EventPowerUp* powerUp = popEvent->DynamicCast<Ray_EventPowerUp>(ITF_GET_STRINGID_CRC(Ray_EventPowerUp,875814764)))
        {
            Actor* receiver = _receiverRef.getActor();
            ITF_WARNING(actor, receiver, "Can't access receiver actor");
            if (receiver)
            {
                receiver->onEvent(powerUp);
            }
        }
        // not implemented: who should we send this event to?
        else
        {
            ITF_WARNING(actor, bfalse, "Unhandled event type");
        }
    }

    // pop spawn
    SpawneeGenerator& generator = m_contentList[m_contentIndex].getPopSpawneeGenerator();
    if (generator.isInitialized())
    {
        Actor* popSpawnee = generator.getSpawnee(
            actor->getScene(), actor->getPos(), actor->getAngle());
        ITF_WARNING(actor, popSpawnee, "Can't spawn actor");

        BubblePrizeBankState bankState=getBankState();
        bbool isSkullCoin = getTemplate()->isSkullCoin();

        if (popSpawnee && (bankState == BubblePrizeBankState_LumKing || isSkullCoin) )
        {
            // lumking/skullcoin specific: make sure the it's always picked up (RO-6920)
            Ray_EventRewardPickedUp pickedUp;
            pickedUp.setSender(_receiverRef);
            pickedUp.setActivator(_receiverRef);
            if (isSkullCoin)
            {
                pickedUp.setExplodesImmediately(btrue);
            }
            popSpawnee->onEvent(&pickedUp);
        }
    }
    
}
//------------------------------------------------------------------------------
void Ray_BubblePrize::processQueryLumCount(Ray_EventQueryLumCount *_queryLumCount)
{
    const BubblePrizeContent_TemplateList &contentList=getTemplate()->getContentList();
    for (u32 i=0; i<m_contentList.size(); i++)
    {
        const BubblePrizeContent_Template &content=contentList[i];
        switch(content.getBankState())
        {
            case BubblePrizeBankState_Lum:
                _queryLumCount->addNormalLums(1);
                break;
            case BubblePrizeBankState_LumKing:
                _queryLumCount->addLumKings(1);
                break;

        }
    }
}

//------------------------------------------------------------------------------
void Ray_BubblePrize::changeRewardNumber( const Ray_EventSetBubblePrizeRewardNumber * _evt )
{
    ITF_ASSERT( _evt != NULL );
    ITF_ASSERT( m_contentIndex < getTemplate()->getContentList().size() );
    
    if ( getTemplate()->getContentList()[m_contentIndex].isRewardNumberChangeEnable() )
    {
         Event* popEvent = m_contentList[m_contentIndex].getPopEvent();
         if ( popEvent != NULL )
         {
             if (Ray_EventSpawnReward* spawnReward = popEvent->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727)))
             {
                 spawnReward->setNumRewards( _evt->getRewardNumber() );
             }
         }
    }   
}

//------------------------------------------------------------------------------
bbool Ray_BubblePrize::isSkullCoin()
{
    return getTemplate()->isSkullCoin();
}
//------------------------------------------------------------------------------
BEGIN_SERIALIZATION(Ray_BubblePrize_Template)
    SERIALIZE_CONTAINER_OBJECT("contentList", m_contentList);
    SERIALIZE_MEMBER("contentChangeDelay", m_contentChangeDelay);
    SERIALIZE_MEMBER("isHeart", m_isHeart);
    SERIALIZE_MEMBER("isSkullCoin", m_isSkullCoin);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_BubblePrize_Template::Ray_BubblePrize_Template()
    : m_contentList()
    , m_contentChangeDelay(0.5f)
    , m_isHeart(bfalse)
    , m_isSkullCoin(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_BubblePrize_Template::~Ray_BubblePrize_Template()
{
}

} // namespace ITF
