#ifndef _ITF_RAY_BUBBLEPRIZE_H_
#define _ITF_RAY_BUBBLEPRIZE_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF {

class Ray_AIScoreLumBehavior;
class Ray_BubblePrize_Template;

//------------------------------------------------------------------------------
enum BubblePrizeBankState
{
    BubblePrizeBankState_Invalid = 0,
    BubblePrizeBankState_Darktoon = 1,
    BubblePrizeBankState_Putrid = 2, // DEPRECATED[LaurentCou]
    BubblePrizeBankState_Heart = 3,
    BubblePrizeBankState_Lum = 4,
    BubblePrizeBankState_LumKing = 5,
    BubblePrizeBankState_SuperPunch_basic = 6,
    BubblePrizeBankState_SuperPunch_seeking = 7,
	BubblePrizeBankState_SwarmRepeller = 8,
    BubblePrizeBankState_RedLum = 9,
    BubblePrizeBankState_Lum_x10 = 10,
    BubblePrizeBankState_RedLum_x10 = 11,
    BubblePrizeBankState_Lum_x5 = 12,
    BubblePrizeBankState_RedLum_x5 = 13,
};

//------------------------------------------------------------------------------
class BubblePrizeContent
{
public:
    BubblePrizeContent();
    ~BubblePrizeContent();

    ITF_INLINE Event* getPopEvent() const { return m_popEvent; }
    ITF_INLINE void setPopEvent(Event* _val) { m_popEvent = _val; }
    ITF_INLINE SpawneeGenerator& getPopSpawneeGenerator() { return m_popSpawneeGenerator; }

private:
    Event* m_popEvent;
    SpawneeGenerator m_popSpawneeGenerator;
};

//------------------------------------------------------------------------------
class BubblePrizeContent_Template
{
    DECLARE_SERIALIZE()

public:
    BubblePrizeContent_Template();
    ~BubblePrizeContent_Template();

    ITF_INLINE const Event* getPopEvent() const { return m_popEvent; }
    ITF_INLINE const Path& getPopSpawn() const { return m_popSpawn; }
    ITF_INLINE BubblePrizeBankState getBankState() const { return m_bankState; }
    ITF_INLINE const bbool isRewardNumberChangeEnable() const { return m_rewardNumberChangeEnable; }

private:
    Event* m_popEvent;
    Path m_popSpawn;
    BubblePrizeBankState m_bankState;
    bbool m_rewardNumberChangeEnable;
};

//------------------------------------------------------------------------------
typedef ITF_VECTOR<BubblePrizeContent> BubblePrizeContentList;
typedef ITF_VECTOR<BubblePrizeContent_Template> BubblePrizeContent_TemplateList;

//------------------------------------------------------------------------------
// Handle content of various size and nature, pop them on demand.
class Ray_BubblePrize
{
public:
    DECLARE_VALIDATE_OBJECT()

    Ray_BubblePrize();
    ~Ray_BubblePrize();

    void init(Actor* _actor, const Ray_BubblePrize_Template* _template);
    void update(f32 _dt);
    
    BubblePrizeBankState getBankState() const;
    void pop(const ActorRef& _receiverRef);
    void changeRewardNumber( const Ray_EventSetBubblePrizeRewardNumber * _evt );
    void processQueryLumCount(Ray_EventQueryLumCount *_queryLumCount);


private:
    ITF_INLINE const Ray_BubblePrize_Template* getTemplate() const;
    
    ActorRef m_actorRef;
    u32 m_contentIndex;
    f32 m_contentChangeTimer;
    BubblePrizeContentList m_contentList;

    const Ray_BubblePrize_Template* m_template;
};

//------------------------------------------------------------------------------
class Ray_BubblePrize_Template
{
    DECLARE_SERIALIZE()

public:
    Ray_BubblePrize_Template();
    ~Ray_BubblePrize_Template();
    
    ITF_INLINE const BubblePrizeContent_TemplateList& getContentList() const { return m_contentList; }
    ITF_INLINE f32 getContentChangeDelay() const { return m_contentChangeDelay; }
    ITF_INLINE bbool isHeart() const { return m_isHeart; }
    ITF_INLINE bbool isSkullCoin() const { return m_isSkullCoin; }

private:
    BubblePrizeContent_TemplateList m_contentList;
    f32 m_contentChangeDelay;
    bbool m_isHeart;
    bbool m_isSkullCoin;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_BubblePrize_Template*
Ray_BubblePrize::getTemplate() const
{
    return static_cast<const Ray_BubblePrize_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_BUBBLEPRIZE_H_
