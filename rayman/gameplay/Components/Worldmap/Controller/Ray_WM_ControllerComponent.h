#ifndef _ITF_RAY_WM_CONTROLLERCOMPONENT_H_
#define _ITF_RAY_WM_CONTROLLERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_RAY_WM_CONTROLLERSTATE_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_ControllerState.h"
#endif //_ITF_RAY_WM_CONTROLLERSTATE_H_

#ifndef _ITF_RAY_WM_FOLLOWABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_FollowAbility.h"
#endif //_ITF_RAY_WM_FOLLOWABILITY_H_

#ifndef _ITF_RAY_WM_LEADABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_LeadAbility.h"
#endif //_ITF_RAY_WM_LEADABILITY_H_

#ifndef _ITF_RAY_HEARTSHIELDCOMPONENT_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_HeartShield.h"
#endif //_ITF_RAY_HEARTSHIELDCOMPONENT_H_

namespace ITF {

class AnimatedComponent;
class AnimGameplayEvent;
class EventPlayerIndexChange;
class EventPlayerModeChanged;
class EventTeleportToActor;
class Ray_WM_ControllerComponent_Template;
class Ray_WM_FollowAbility;
class Ray_WM_LeadAbility;
class TrajectoryNodeComponent;

//------------------------------------------------------------------------------
class Ray_WM_ControllerComponent : public ActorComponent, public IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_ControllerComponent, ActorComponent,2120802388)
    DECLARE_SERIALIZE();
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_WM_ControllerComponent();
    virtual ~Ray_WM_ControllerComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void onBecomeActive();
    virtual void onBecomeInactive();
    virtual void Update(f32 _dt);
    virtual void updateAnimInput();
    virtual void onEvent(Event* _event);
    
#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(class ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

    bbool isMoving() const;
    ITF_INLINE Ray_WM_ControllerState getState() const { return m_state; }

    ITF_INLINE Ray_WM_LeadAbility& getLeadAbility() { return m_leadAbility; }
    ITF_INLINE Ray_WM_FollowAbility& getFollowAbility() { return m_followAbility; }

private:
    friend class Ray_WM_LeadAbility;
    friend class Ray_WM_FollowAbility;

    const Ray_WM_ControllerComponent_Template* getTemplate() const;

    void init();
    void reset();

    void changeState(Ray_WM_ControllerState _newState);
    void postUpdateAbility(f32 _dt);
    ITF_INLINE bbool isPerformingUTurn() const { return m_performingUTurn != UTURNTYPE_NONE; }

    // events processing
    void processPlayerIndexChange(EventPlayerIndexChange* _playerIndexChanged);
    void processPlayerModeChanged(EventPlayerModeChanged* _eventGameMode);
    void processAnimGameplayEvent(AnimGameplayEvent* _animEvent);
    void processTeleport(EventTeleportToActor* _teleport);

    // u-turns
    void startUTurn( EUturnType _type );
    void resetUTurn();    

    u32 m_playerIndex;
    Ray_WM_ControllerState m_state;
    Vec2d m_previousPos;
    Vec2d m_speed2d;

    // move cursor: used to cycle steps
    f32 m_moveCursor;

    // u-turns
    EUturnType m_performingUTurn; // if the character is performing a u-turn
    bbool m_uTurnFlipped;         // if we already flipped the character on the u-turn
    bbool m_uTurnFinished;        // if the u-turn is finished
    bbool m_flipActor;            // if we need to flip the actor

    // sprint
    bbool m_isSprinting;
    
    Ray_WM_LeadAbility m_leadAbility; 
    Ray_WM_FollowAbility m_followAbility; 
    AnimatedComponent* m_animatedComponent;

    Ray_HeartShield m_heartShield;
};

//------------------------------------------------------------------------------
class Ray_WM_ControllerComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_ControllerComponent_Template, TemplateActorComponent,723916428);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_ControllerComponent);

public:
    Ray_WM_ControllerComponent_Template();
    ~Ray_WM_ControllerComponent_Template();
    
    ITF_INLINE f32 getMoveX() const { return m_moveX; }
    ITF_INLINE f32 getMoveFactorMultiplier() const { return m_moveFactorMultiplier; }
    ITF_INLINE const StringID& getStandAnim() const { return m_standAnim; }
    ITF_INLINE const StringID& getWalkAnim() const { return m_walkAnim; }
    ITF_INLINE const StringID& getJumpAnim() const { return m_jumpAnim; }
    ITF_INLINE const StringID& getSwimAnim() const { return m_swimAnim; }
    ITF_INLINE const StringID& getFlyAnim() const { return m_flyAnim; }
    ITF_INLINE const Ray_WM_LeadAbility_Template& getLeadAbility() const { return m_leadAbility; }
    ITF_INLINE const Ray_WM_FollowAbility_Template& getFollowAbility() const { return m_followAbility; }
    ITF_INLINE const Ray_HeartShield_Template& getHeartShield() const { return m_heartShield; }
    
private:
    f32 m_moveX;
    f32 m_moveFactorMultiplier;
    StringID m_standAnim;
    StringID m_walkAnim;
    StringID m_jumpAnim;
    StringID m_swimAnim;
    StringID m_flyAnim;
    Ray_WM_LeadAbility_Template m_leadAbility; 
    Ray_WM_FollowAbility_Template m_followAbility; 
    Ray_HeartShield_Template m_heartShield;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_WM_ControllerComponent_Template*
Ray_WM_ControllerComponent::getTemplate() const
{
    return static_cast<const Ray_WM_ControllerComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_WM_CONTROLLERCOMPONENT_H_
