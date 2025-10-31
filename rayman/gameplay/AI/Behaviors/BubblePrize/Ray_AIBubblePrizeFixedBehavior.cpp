#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBUBBLEPRIZEFIXEDBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/BubblePrize/Ray_AIBubblePrizeFixedBehavior.h"
#endif //_ITF_RAY_AIBUBBLEPRIZEFIXEDBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_


namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIBubblePrizeFixedBehavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AIBubblePrizeFixedBehavior)
    m_bubblePrize.validate(_isBehaviorValidated);
    VALIDATE_BEHAVIOR_PARAM("", m_playerDetectorComponent, "PlayerDetectorComponent mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AIBubblePrizeFixedBehavior::Ray_AIBubblePrizeFixedBehavior()
    : Super()
    , m_bubblePrize()
    , m_waterCheckOffset(0)
    , m_playerDetectorComponent(NULL)
{
    static u32 serialNumber = 0;
    m_waterCheckOffset = serialNumber++;
}

//------------------------------------------------------------------------------
Ray_AIBubblePrizeFixedBehavior::~Ray_AIBubblePrizeFixedBehavior()
{
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeFixedBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
   
    // mandatory, validated
    m_playerDetectorComponent = m_actor->GetComponent<PlayerDetectorComponent>();

    // init bubble prize
    m_bubblePrize.init(m_actor, &getTemplate()->getBubblePrize());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSetBubblePrizeRewardNumber,2143348728),m_aiComponent);
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeFixedBehavior::update(f32 _dt)
{
    Super::update(_dt);
    
    // update bubble prize
    m_bubblePrize.update(_dt);

    // update content visual
    BubblePrizeBankState bankState = m_bubblePrize.getBankState();
    if (bankState != BubblePrizeBankState_Invalid)
    {
        m_animComponent->setBankState(bankState);
    }
    
    // pop on first player detected
    if ( m_playerDetectorComponent->getActorsInside().size() > 0 )
    {
        ActorRef receiverRef = m_playerDetectorComponent->getActorsInside().back();
        m_bubblePrize.pop(receiverRef);

        // done, to death behavior
        m_aiComponent->setHealth(0);
    }

    // change detector's radius if water detected (RO-16573)
    if (((CURRENTFRAME+m_waterCheckOffset) & 31)==0)
    {
        updatePlayerDetectorRadius();
    }
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeFixedBehavior::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( Ray_EventSetBubblePrizeRewardNumber* evt = _event->DynamicCast<Ray_EventSetBubblePrizeRewardNumber>(ITF_GET_STRINGID_CRC(Ray_EventSetBubblePrizeRewardNumber,2143348728)) )
    {
        m_bubblePrize.changeRewardNumber( evt );
    }
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeFixedBehavior::onActionFinished()
{
    Super::onActionFinished();
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeFixedBehavior::updatePlayerDetectorRadius()
{
    PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());
    m_playerDetectorComponent->setScaleMultiplier(waterPoly?getTemplate()->getPlayerDetectorMultiplierInWater():1.f);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIBubblePrizeFixedBehavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIBubblePrizeFixedBehavior_Template)
    SERIALIZE_OBJECT("bubblePrize",m_bubblePrize);
    SERIALIZE_MEMBER("playerDetectorMultiplierInWater", m_playerDetectorMultiplierInWater);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIBubblePrizeFixedBehavior_Template::Ray_AIBubblePrizeFixedBehavior_Template()
    : Super()
    , m_bubblePrize()
    , m_playerDetectorMultiplierInWater(1.5f)
{
}

//------------------------------------------------------------------------------
Ray_AIBubblePrizeFixedBehavior_Template::~Ray_AIBubblePrizeFixedBehavior_Template()
{
}

} // namespace ITF
