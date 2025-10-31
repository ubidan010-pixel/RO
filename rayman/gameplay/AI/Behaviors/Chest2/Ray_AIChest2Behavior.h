#ifndef _ITF_RAY_AICHEST2BEHAVIOR_H_
#define _ITF_RAY_AICHEST2BEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_TRAJECTORYDATA_H_
#include "gameplay/Components/Trajectory/TrajectoryData.h"
#endif //_ITF_TRAJECTORYDATA_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

namespace ITF {

class AIAction;
class AIAction_Template;
class DummyComponent;
class EventTrigger;
class LinkComponent;
class PlayerDetectorComponent;
class Ray_AIChest2Behavior_Template;
class TrajectoryFollowerComponent;

//------------------------------------------------------------------------------
/// Wrapper around basic link info
class Chest2LinkInfo : public TrajectoryLinkInfo
{
public:
    Chest2LinkInfo(const TrajectoryLinkInfo& _other);
    
    ITF_INLINE bbool getIgnoreCamera() const { return m_ignoreCamera; }
    ITF_INLINE bbool getTakeCamera() const { return m_takeCamera; }
    ITF_INLINE bbool getChangeScale() const { return m_changeScale; }
    ITF_INLINE bbool getBreakable() const { return m_breakable; }
    ITF_INLINE bbool getInvincible() const { return m_invincible; }
    ITF_INLINE f32 getSafeDistance() const { return m_safeDistance; }
    ITF_INLINE f32 getMinSpeed() const { return m_minSpeed; }
    ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }

private:
    Chest2LinkInfo();

    bbool m_ignoreCamera;   // ignore camera on actions that usually take it
    bbool m_takeCamera;    // take camera on actions that usually ignore it 
    bbool m_changeScale;
    bbool m_breakable;
    bbool m_invincible;
    f32 m_safeDistance;
    f32 m_minSpeed;
    f32 m_maxSpeed;
};

//------------------------------------------------------------------------------
/// Chest v2.0, no physics, follows a nodes trajectory
class Ray_AIChest2Behavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIChest2Behavior, AIBehavior,523291102);
    DECLARE_VALIDATE_BEHAVIOR()

public:
    Ray_AIChest2Behavior();
    ~Ray_AIChest2Behavior();

    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void onActorBecomeInactive();
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void update(f32 _dt);
    virtual void onEvent(Event* _event);
    virtual void updateAnimInputs();
    virtual void onActionFinished();

#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

private:
    enum State_t
    {
        State_NA,
        State_Stand,
        State_Run,
        State_Jump,
        State_ToFly,
        State_Fly,
        State_Land,        
        State_Skid,
        State_Wait,
        State_Taunt,
        State_Dive,
        State_Swim,
        State_ThinkDetect,
        State_ThinkHide,
        State_Breaks,
        State_Broken,
    };

    ITF_INLINE const Ray_AIChest2Behavior_Template * getTemplate() const;
    
    bbool isBreakable() const;
    void processTrigger(const EventTrigger* _triggerEvt);
    void processHit(const HitStim* _hitStim);
    void processInteractionQuery(EventInteractionQuery* _query);
    void processCrushed(const EventCrushed* _eventCrushed);
    void processAnimEvent(const AnimGameplayEvent* _animEvent);
    void spawnBreakFx();
    void spawnRelic();
    void sendBreakEvents();
    void updateClosestPlayerRef();
    void updateCamera(const Chest2LinkInfo& _linkInfo);
    void computeEyeFromTargetPosition(const Vec2d &_pos, f32 _radiusScale, f32 &_animInterpX, f32 &_animInterpY);
    void updateEyePos(Actor *_player);
    void changeState(State_t _newState);
    void computeTargetSpeed(const Chest2LinkInfo& _linkInfo);
    void updateSafeDistance(const Chest2LinkInfo& _linkInfo);
    void updateScale(const Chest2LinkInfo& _linkInfo, f32 _linkCursor);
    void updateState(const Chest2LinkInfo& _linkInfo, f32 _linkCursor, f32 _dt);

    // quicksort helper class
    // NB: the higher the dot, the straighter the path, thus >
    struct SortedNode
    {
        TrajectoryNodeComponent* m_node;
        f32 m_dot;
        bool operator <(const SortedNode& _other) const
        {
            return m_dot > _other.m_dot; 
        }
    };

    // thinking
    enum EThinkNode
    {
        ThinkNode_Invalid = -1,
        ThinkNode_Detect = 0,
        ThinkNode_DetectExtern = 1,
        ThinkNode_Hide = 2,
    };
    void startThink();
    void stopThink(bbool _straightOnly = bfalse);
    TrajectoryNodeComponent* getNode(const ActorRef& _ref) const;
    PlayerDetectorComponent* getPlayerDetector(const ActorRef& _ref, EThinkNode _thinkNodeType) const;
    EThinkNode getThinkNodeType(const ActorRef& _ref) const;
    bbool isInvincibleThinkNode(const ActorRef& _ref) const;
    bbool findPath(
        TrajectoryNodeComponent* _previous,
        TrajectoryNodeComponent* _from,
        TrajectoryNodeComponent::NodeList& _path,
        TrajectoryNodeComponent::NodeList& _visited,
        u32 _recursionCount,
        bbool _straightOnly);
    void checkReverse(f32 _dt);

    bbool m_activated;
    bbool m_breakable;
    bbool m_invincible;
    bbool m_thinking;
    bbool m_upsideDownJump;
    State_t m_state;
    f32 m_minDot;
    f32 m_targetSpeed;
    f32 m_skidStartSpeed;
    u32 m_tauntFlag;
    u32 m_tauntStopCount;
    f32 m_safeDistance;
    bbool m_changingScale;
    bbool m_atReducedScale;
    ActorRef m_closestPlayerRef;
    SpawneeGenerator m_breakFxGenerator;
    SpawneeGenerator m_relicGenerator;
    SafeArray<Event*> m_breakEvents;
    ActorRef m_initialPreThinkNode;
    ActorRef m_preThinkNode;
    ActorRef m_thinkNode;
    EThinkNode m_thinkNodeType;
    f32 m_hideCountdown;
    f32 m_detectCountdown;
    f32 m_reverseCountdown;
    AIAction* m_standAction;
    AIAction* m_runAction;
    AIAction* m_jumpAction;
    AIAction* m_toFlyAction;
    AIAction* m_flyAction;
    AIAction* m_landAction;
    AIAction* m_skidAction;
    AIAction* m_waitAction;
    AIAction* m_tauntAction;
    AIAction* m_diveAction;
    AIAction* m_swimAction;
    AIAction* m_thinkDetectAction;
    AIAction* m_thinkHideAction;
    AIAction* m_breaksAction;
    AIAction* m_brokenAction;
    TrajectoryFollowerComponent* m_trajectoryFollowerComponent;
    LinkComponent* m_linkComponent;
};

//------------------------------------------------------------------------------
class Ray_AIChest2Behavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIChest2Behavior_Template, TemplateAIBehavior,860147291);
    DECLARE_SERIALIZE();
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIChest2Behavior);

public:
    Ray_AIChest2Behavior_Template();
    ~Ray_AIChest2Behavior_Template();
    
    // movement
    ITF_INLINE f32 getSafeDistance() const { return m_safeDistance; }
    ITF_INLINE f32 getMinSpeed() const { return m_minSpeed; }
    ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }
    ITF_INLINE f32 getSwimMinSpeed() const { return m_swimMinSpeed; }
    ITF_INLINE f32 getSwimMaxSpeed() const { return m_swimMaxSpeed; }
    ITF_INLINE f32 getWaitSpeed() const { return m_waitSpeed; }
    ITF_INLINE f32 getUpsideDownFloorSpeed() const { return m_upsideDownFloorSpeed; }
    ITF_INLINE f32 getUpsideDownCeilingSpeed() const { return m_upsideDownCeilingSpeed; }
    ITF_INLINE f32 getBreakableSpeed() const { return m_breakableSpeed; }
    ITF_INLINE f32 getReducedScale() const { return m_reducedScale; }

    // eye following
    ITF_INLINE f32 getEyeMinX() const { return m_eyeMinX; }
    ITF_INLINE f32 getEyeMaxX() const { return m_eyeMaxX; }
    ITF_INLINE f32 getEyeMinY() const { return m_eyeMinY; }
    ITF_INLINE f32 getEyeMaxY() const { return m_eyeMaxY; }
    ITF_INLINE const Vec2d& getRelativeNeutralEyePos() const { return m_relativeNeutralEyePos; }
    ITF_INLINE f32 getDistanceToEyeFactor() const { return m_distanceToEyeFactor; }
    ITF_INLINE f32 getTypicalPlayerHeadY() const { return m_typicalPlayerHeadY; }

    // break fx
    ITF_INLINE const Path& getBreakFx() const { return m_breakFx; }

    // relic spawn
    ITF_INLINE i32 getRelicIndex() const { return m_relicIndex; }
    ITF_INLINE const Path& getRelicActor() const { return m_relicActor; }

    // break events
    ITF_INLINE bbool getSendBreakEventsDirectly() const { return m_sendBreakEventsDirectly; }
    ITF_INLINE bbool getDisableAfterBreakEvents() const { return m_disableAfterBreakEvents; }
    ITF_INLINE bbool getSnapBreakSequencePlayer() const { return m_snapBreakSequencePlayer; }
    ITF_INLINE const SafeArray<Event*>& getBreakEvents() const { return m_breakEvents; }

    // thinking
    ITF_INLINE f32 getDetectDelay() const { return m_detectDelay; }
    ITF_INLINE f32 getHideTimeout() const { return m_hideTimeout; }
    ITF_INLINE bbool getCanReverse() const { return m_canReverse; }
    ITF_INLINE f32 getReverseInterval() const { return m_reverseInterval; }

    // actions
    ITF_INLINE AIAction_Template* getStandAction() const { return m_standAction; }
    ITF_INLINE AIAction_Template* getRunAction() const { return m_runAction; }
    ITF_INLINE AIAction_Template* getJumpAction() const { return m_jumpAction; }
    ITF_INLINE AIAction_Template* getToFlyAction() const { return m_toFlyAction; }
    ITF_INLINE AIAction_Template* getFlyAction() const { return m_flyAction; }
    ITF_INLINE AIAction_Template* getLandAction() const { return m_landAction; }
    ITF_INLINE AIAction_Template* getSkidAction() const { return m_skidAction; }
    ITF_INLINE AIAction_Template* getWaitAction() const { return m_waitAction; }
    ITF_INLINE AIAction_Template* getTauntAction() const { return m_tauntAction; }
    ITF_INLINE AIAction_Template* getDiveAction() const { return m_diveAction; }
    ITF_INLINE AIAction_Template* getSwimAction() const { return m_swimAction; }
    ITF_INLINE AIAction_Template* getThinkDetectAction() const { return m_thinkDetectAction; }
    ITF_INLINE AIAction_Template* getThinkHideAction() const { return m_thinkHideAction; }
    ITF_INLINE AIAction_Template* getBreaksAction() const { return m_breaksAction; }
    ITF_INLINE AIAction_Template* getBrokenAction() const { return m_brokenAction; }

private:
    // movement
    f32 m_safeDistance;
    f32 m_minSpeed;
    f32 m_maxSpeed;
    f32 m_swimMinSpeed;
    f32 m_swimMaxSpeed;
    f32 m_waitSpeed;
    f32 m_upsideDownFloorSpeed;
    f32 m_upsideDownCeilingSpeed;
    f32 m_breakableSpeed;
    f32 m_reducedScale;

    // eye following
    f32 m_eyeMinX;
    f32 m_eyeMaxX;
    f32 m_eyeMinY;
    f32 m_eyeMaxY;
    Vec2d m_relativeNeutralEyePos;
    f32 m_distanceToEyeFactor;
    f32 m_typicalPlayerHeadY;

    // break fx
    Path m_breakFx;

    // relic spawn
    i32 m_relicIndex;
    Path m_relicActor;

    // break events
    bbool m_sendBreakEventsDirectly;
    bbool m_disableAfterBreakEvents;
    bbool m_snapBreakSequencePlayer;
    SafeArray<Event*> m_breakEvents;

    // thinking
    f32 m_detectDelay;
    f32 m_hideTimeout;
    bbool m_canReverse;
    f32 m_reverseInterval;

    // actions
    AIAction_Template* m_standAction;
    AIAction_Template* m_runAction;
    AIAction_Template* m_jumpAction;
    AIAction_Template* m_toFlyAction;
    AIAction_Template* m_flyAction;
    AIAction_Template* m_landAction;
    AIAction_Template* m_skidAction;
    AIAction_Template* m_waitAction;
    AIAction_Template* m_tauntAction;
    AIAction_Template* m_diveAction;
    AIAction_Template* m_swimAction;
    AIAction_Template* m_thinkDetectAction;
    AIAction_Template* m_thinkHideAction;
    AIAction_Template* m_breaksAction;
    AIAction_Template* m_brokenAction;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AIChest2Behavior_Template*
Ray_AIChest2Behavior::getTemplate() const
{
    return static_cast<const Ray_AIChest2Behavior_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_Ray_AIChest2Behavior_H_
