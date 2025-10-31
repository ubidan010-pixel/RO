#ifndef _ITF_RAY_DISPENSERCOMPONENT_H_
#define _ITF_RAY_DISPENSERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_RAY_SOFTCOLLISION_H_
#include "rayman/gameplay/AI/Utils/Ray_SoftCollision.h"
#endif //_ITF_RAY_SOFTCOLLISION_H_

namespace ITF
{

class AnimatedComponent;
class Event;
class EventTrigger;
class PhantomComponent;
class PlayerDetectorComponent;
class Ray_ChildLaunchComponent;
class Ray_DispenserComponent_Template;

//------------------------------------------------------------------------------
/// Dispense a given number of goods then shrinks
class Ray_DispenserComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DispenserComponent, ActorComponent,2759118564)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    typedef SafeArray<Angle> EjectionAngleList;

    Ray_DispenserComponent();
    virtual ~Ray_DispenserComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onCheckpointLoaded();
    virtual void onEvent(Event* _event);
    virtual void Update(f32 _dt);
    virtual void updateAnimInput();

#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

private:
    enum State_t {State_NA,State_Idle,State_Wiggle,State_Dispense,State_Shrink,State_Done};

    ITF_INLINE const class Ray_DispenserComponent_Template * getTemplate() const;

    void processEventTrigger( EventTrigger* _trigger );
    void processPunchStim( PunchStim* _stimEvent );
    void changeState(State_t _newState, bbool _force = bfalse);
    void updateState(f32 _dt);
    bbool dispense( const ActorRef & _sender, bbool _all );
    void dispenseOne(const ActorRef & _sender);
    bbool hasLowLifeGoodsRemaining() const;
    bbool hasGoodsRemaining() const;

    State_t m_state;
    u32 m_goodsRemaining;
    u32 m_lowLifeGoodsRemaining;
    f32 m_dispenseCountdown;
    f32 m_shakeCountdown;
    f32 m_shakeFactor;
    EjectionAngleList m_ejectionAngleListShuffled;
    u32 m_ejectionAngleIndex;
    Event* m_dispenseEvent;
    Event* m_lowLifeEvent;
    Ray_SoftCollision m_softCollision;
    u32 m_playersInsideCount;
    AnimatedComponent* m_animatedComponent;
    PhantomComponent* m_phantomComponent;
    Ray_ChildLaunchComponent* m_childLaunchComponent;
    PlayerDetectorComponent* m_playerDetector;
};

//------------------------------------------------------------------------------
class Ray_DispenserComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DispenserComponent_Template,TemplateActorComponent,3725050653);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DispenserComponent);

public:
    Ray_DispenserComponent_Template();
    ~Ray_DispenserComponent_Template();

    ITF_INLINE u32 getGoodsCount() const { return m_goodsCount; }
    ITF_INLINE u32 getLowLifeGoodsMax() const { return m_lowLifeGoodsMax; }
    ITF_INLINE f32 getLowLifeThreshold() const { return m_lowLifeThreshold; }
    ITF_INLINE f32 getDispenseDelay() const { return m_dispenseDelay; }
    ITF_INLINE f32 getShakeDelay() const { return m_shakeDelay; }
    ITF_INLINE const Vec3d& getSpawnOffset() const { return m_spawnOffset; }
    ITF_INLINE const Ray_DispenserComponent::EjectionAngleList& getEjectionAngleList() const { return m_ejectionAngleList; }
    ITF_INLINE bbool getChildLauncher() const { return m_isChildLauncher; }
    ITF_INLINE const StringID& getAnimIdle() const { return m_animIdle; }
    ITF_INLINE bbool getCanWiggle() const { return m_canWiggle; }
    ITF_INLINE const StringID& getPadRumbleWiggle() const { return m_padRumbleWiggle; }
    ITF_INLINE const StringID& getAnimWiggle() const { return m_animWiggle; }
    ITF_INLINE const StringID& getAnimDispense() const { return m_animDispense; }
    ITF_INLINE const StringID& getAnimShrink() const { return m_animShrink; }
    ITF_INLINE const StringID& getAnimDone() const { return m_animDone; }
    ITF_INLINE const Event* getDispenseEvent() const { return m_dispenseEvent; }
    ITF_INLINE const Event* getLowLifeEvent() const { return m_lowLifeEvent; }
    ITF_INLINE bbool getSoftCollisionDisabled() const { return m_softCollisionDisabled; }
    ITF_INLINE const Ray_SoftCollision_Template& getSoftCollision() const { return m_softCollision; }
    ITF_INLINE bbool getProcessStim() const { return m_processStim; }    
    ITF_INLINE bbool getAllowNonPlayerHits() const { return m_allowNonPlayerHits; }    
    ITF_INLINE bbool getDisableWhenDone() const { return m_disableWhenDone; }
    

private:
    u32 m_goodsCount;
    u32 m_lowLifeGoodsMax;
    f32 m_lowLifeThreshold;
    f32 m_dispenseDelay;
    f32 m_shakeDelay;
    Vec3d m_spawnOffset;
    Ray_DispenserComponent::EjectionAngleList m_ejectionAngleList;
    bbool m_isChildLauncher;
    StringID m_animIdle;
    bbool m_canWiggle;                // wiggle when player passing by?
    StringID m_padRumbleWiggle;          // pad rumble to start when wiggling
    StringID m_animWiggle;
    StringID m_animDispense;
    StringID m_animShrink;
    StringID m_animDone;
    Event* m_dispenseEvent;
    Event* m_lowLifeEvent;
    bbool m_softCollisionDisabled;
    Ray_SoftCollision_Template m_softCollision;
    bbool m_processStim;
    bbool m_allowNonPlayerHits;
    bbool m_disableWhenDone;
};

//------------------------------------------------------------------------------
ITF_INLINE
const class Ray_DispenserComponent_Template*
Ray_DispenserComponent::getTemplate() const
{
    return static_cast<const Ray_DispenserComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_DISPENSERCOMPONENT_H_