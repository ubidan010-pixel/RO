#ifndef _ITF_RAY_AIBUBBLEPRIZEFIXEDBEHAVIOR_H_
#define _ITF_RAY_AIBUBBLEPRIZEFIXEDBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_BUBBLEPRIZE_H_
#include "rayman/gameplay/AI/Behaviors/BubblePrize/Ray_BubblePrize.h"
#endif //_ITF_RAY_BUBBLEPRIZE_H_

namespace ITF
{
class PlayerDetectorComponent;
class Ray_AIBubblePrizeFixedBehavior_Template;

//------------------------------------------------------------------------------
class Ray_AIBubblePrizeFixedBehavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIBubblePrizeFixedBehavior, AIBehavior,1770720676);
    DECLARE_VALIDATE_BEHAVIOR()

public:
    Ray_AIBubblePrizeFixedBehavior();
    ~Ray_AIBubblePrizeFixedBehavior();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate() {}
    virtual void update(f32 _dt);
    virtual void onEvent(Event * _event);
    virtual void onActionFinished();

private:
    ITF_INLINE const Ray_AIBubblePrizeFixedBehavior_Template * getTemplate() const;
    
    void updatePlayerDetectorRadius();

    Ray_BubblePrize m_bubblePrize;
    
    u32 m_waterCheckOffset;
    PlayerDetectorComponent* m_playerDetectorComponent;
};

//------------------------------------------------------------------------------
class Ray_AIBubblePrizeFixedBehavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIBubblePrizeFixedBehavior_Template,TemplateAIBehavior,4089474871);
    DECLARE_SERIALIZE();
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIBubblePrizeFixedBehavior);

public:
    Ray_AIBubblePrizeFixedBehavior_Template();
    ~Ray_AIBubblePrizeFixedBehavior_Template();

    ITF_INLINE const Ray_BubblePrize_Template& getBubblePrize() const { return m_bubblePrize; }
    ITF_INLINE f32 getPlayerDetectorMultiplierInWater() const { return m_playerDetectorMultiplierInWater; }

private:
    Ray_BubblePrize_Template m_bubblePrize;
    f32 m_playerDetectorMultiplierInWater;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AIBubblePrizeFixedBehavior_Template* Ray_AIBubblePrizeFixedBehavior::getTemplate() const
{
    return static_cast<const Ray_AIBubblePrizeFixedBehavior_Template *>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_AIBUBBLEPRIZEFIXEDBEHAVIOR_H_
