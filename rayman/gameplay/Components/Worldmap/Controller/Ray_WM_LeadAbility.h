#ifndef _ITF_RAY_WM_LEADABILITY_H_
#define _ITF_RAY_WM_LEADABILITY_H_

#ifndef _ITF_RAY_WM_BASEABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_BaseAbility.h"
#endif //_ITF_RAY_WM_BASEABILITY_H_

#ifndef _ITF_TRAJECTORYDATA_H_
#include "gameplay/Components/Trajectory/TrajectoryData.h"
#endif //_ITF_TRAJECTORYDATA_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

#ifndef _ITF_RAY_WM_CONTROLLERSTATE_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_ControllerState.h"
#endif //_ITF_RAY_WM_CONTROLLERSTATE_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

namespace ITF {

class Ray_WM_LeadAbility_Template;
class Ray_WM_SpotComponent;

//------------------------------------------------------------------------------
struct Ray_WM_MoveRecord
{
    Ray_WM_MoveRecord()
        : m_valid(bfalse)
        , m_pos(Vec3d::Zero)
        , m_adjustFactor(0.0f)
        , m_isSprinting(bfalse)
        , m_state(Ray_WM_ControllerState_NA)
    {}

    void clear()
    {
        m_valid = bfalse;
        m_pos = Vec3d::Zero;
        m_adjustFactor = 0.0f;
        m_isSprinting = bfalse;
        m_state = Ray_WM_ControllerState_NA;
    }

    bbool m_valid;
    Vec3d m_pos;
    f32 m_adjustFactor;
    bbool m_isSprinting;
    Ray_WM_ControllerState m_state;
};

//------------------------------------------------------------------------------
/// Moves the lead player according to joypad input
class Ray_WM_LeadAbility : public Ray_WM_BaseAbility, public IInputListener
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_LeadAbility, Ray_WM_BaseAbility,2634161728)
    DECLARE_VALIDATE_OBJECT()

public:
    Ray_WM_LeadAbility();
    virtual ~Ray_WM_LeadAbility();

    virtual void onActorLoaded(
        Pickable::HotReloadType _hotReload,
        const Ray_WM_BaseAbility_Template* _template,
        Ray_WM_ControllerComponent* _controller,
        Actor* _actor);
    virtual void onBecomeActive(u32 _playerIndex);
    virtual void onPlayerIndexChange(u32 _newIndex);
    virtual void onBecomeInactive();
    virtual void update(f32 _dt);
    
#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit(class ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif

    void signalFollowing(Actor* _actor, u32 _playerIndex, bbool _following);
    Ray_WM_MoveRecord getMoveRecord(u32 _playerIndex) const;

    void teleport(Actor* _to);

    Ray_WM_SpotComponent* getStandSpot() const;

    virtual void Receive( u32 deviceID /* player */, f32 axis, const StringID& action ) ;

private:
    const Ray_WM_LeadAbility_Template* getTemplate() const;

    u32 getFollowerIndex(u32 _playerIndex) const;

    Ray_WM_SpotComponent* getSpot(const ActorRef& _ref) const;
    TrajectoryNodeComponent* getNode(const ActorRef& _ref) const;

    ActorRef getCurrentNodeRef() const;

    void applyInput();
    bbool findStandNodeRef();
    void updateMove(f32 _dt);
    f32 computeAdjustFactor() const;
    u32 computeFrameOffset(u32 _followerIndex) const;
    bbool findMove(const Vec2d& _dir, const ActorRef& _nodeRef);
    void reverseMove(const Vec2d& _dir);
    void stepMove(u32 _index);
    
    void changeStateInternal(Ray_WM_ControllerState _newState, const ActorRef& _newStandNodeRef = ActorRef::InvalidRef);
    
    void startTeleportToLevelSelection();
    void startTeleportToWorldSelection();

    bbool m_inLevelSelection;
    bbool m_changedLayer;   // set to true when player change from worldmap to levelmap and the opposite. used to trigger specific behavior on first node
    bbool m_wordlMapUnlockSequencePlayed;
    
    TrajectoryData m_trajectoryData;
    ActorRef m_standNodeRef;
    ActorRefList m_moveNodeRefs;
    u32 m_moveNodeIndex;
    bbool m_reverseForbidden;

    f32 m_speed;
    f32 m_targetSpeed;

    // following
    static const u32 s_framesPerFollower = 20; // 20 frames per follower
    static const u32 s_maxFollowers = s_maxPlayers - 1;
    static const u32 s_moveRecordsCount = s_framesPerFollower * s_maxFollowers;
    FixedArray<bbool, s_maxFollowers> m_followers;
    FixedArray<u32, s_maxFollowers> m_followersFrameOffset;
    FixedArray<Ray_WM_MoveRecord, s_moveRecordsCount> m_moveRecords;
    u32 m_frame;

    //
    //--
    void clearListener();
    bbool m_listenerCheatUnlock;
    bbool m_listenerCheatActive;
    bbool m_listenerEnter;
    bbool m_listenerBack;
    bbool m_listenerHome;
    Vec2d m_listenerMove;
    bbool m_listenerSprint;

    bbool m_listenerRegistered;
};

//------------------------------------------------------------------------------
class Ray_WM_LeadAbility_Template : public Ray_WM_BaseAbility_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_WM_LeadAbility_Template, Ray_WM_BaseAbility_Template,1909722303);
    DECLARE_SERIALIZE();

public:
    Ray_WM_LeadAbility_Template();
    ~Ray_WM_LeadAbility_Template();
    
    ITF_INLINE const ITF_STDSTRING& getStartNode() const { return m_startNode; }
    ITF_INLINE Angle getFindMoveAngle() const { return m_findMoveAngle; }
    ITF_INLINE Angle getReverseMoveAngle() const { return m_reverseMoveAngle; }
    ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }
    ITF_INLINE f32 getTargetSpeedBlendFactor() const { return m_targetSpeedBlendFactor; }
    ITF_INLINE f32 getSpeedBlendFactor() const { return m_speedBlendFactor; }
    ITF_INLINE f32 getSprintMaxSpeed() const { return m_sprintMaxSpeed; }
    ITF_INLINE f32 getSprintTargetSpeedBlendFactor() const { return m_sprintTargetSpeedBlendFactor; }
    ITF_INLINE f32 getSprintSpeedBlendFactor() const { return m_sprintSpeedBlendFactor; }
    ITF_INLINE f32 getZOffset() const { return m_zOffset; }
    ITF_INLINE const ITF_VECTOR<Vec3d>& getFollowersAdjustOffsets() const { return m_followersAdjustOffsets; }
    
private:
    ITF_STDSTRING m_startNode;
    Angle m_findMoveAngle;
    Angle m_reverseMoveAngle;
    f32 m_maxSpeed;
    f32 m_targetSpeedBlendFactor;
    f32 m_speedBlendFactor;
    f32 m_sprintMaxSpeed;
    f32 m_sprintTargetSpeedBlendFactor;
    f32 m_sprintSpeedBlendFactor;
    f32 m_zOffset;
    ITF_VECTOR<Vec3d> m_followersAdjustOffsets;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_WM_LeadAbility_Template*
Ray_WM_LeadAbility::getTemplate() const
{
    return static_cast<const Ray_WM_LeadAbility_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_WM_LEADABILITY_H_
