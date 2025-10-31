#ifndef _ITF_BTACTIONWALKTOTARGET_H_
#define _ITF_BTACTIONWALKTOTARGET_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

class BTActionWalkToTarget : public BTAction
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionWalkToTarget,BTAction,2931928196);

public:

    BTActionWalkToTarget();
    ~BTActionWalkToTarget() {}

    virtual void                                init( BehaviorTree* _tree, const BTNode_Template* _template );
    virtual bbool                               validate( Actor* _actor ) const;
    virtual bbool                               decide( f32 _dt );
    virtual void                                onActivate();
    virtual bbool                               update( f32 _dt );
    virtual void                                updateAnimInput();

protected:

    enum WalkState
    {
        WALKSTATE_UNKNOWN,
        WALKSTATE_GROUND,
        WALKSTATE_FALL,
        WALKSTATE_JUMP,
        WALKSTATE_SWIM,
    };

    // Ground
    bbool                                       updateGround( f32 _dt );
    bbool                                       updateCheckContactsBlocking( const Vec2d& _moveDir );

    // Fall
    bbool                                       updateFall( f32 _dt );

    // Jump
    bbool                                       updateJump( f32 _dt );

    // Swim
    bbool                                       updateSwim( f32 _dt );
    bbool                                       swimHandleInsideOutside( f32 _dt, const PolyLineEdge* _closestEdge );
    bbool                                       swimAdjustMoveToCollisions( f32 _dt, Vec2d& _moveDir );

    bbool                                       checkStateChange();
    class PolyLine*                             getSwimPolyline() const;
    void                                        setState( WalkState _state );

    void                                        resetUTurn();

    Vec2d                                       getTargetPoint( const class PolyLineEdge* _currentEdge ) const;
    Vec2d                                       getCurrentPoint( const class PolyLineEdge* _currentEdge ) const;
    const class BTActionWalkToTarget_Template*  getTemplate() const;

    class GroundAIControllerComponent*          m_aiController;
    class StickToPolylinePhysComponent*         m_physComponent;
    class AnimLightComponent*                   m_animComponent;
    AIUtils::PolylineRegionList                 m_currentRegions;
    WalkState                                   m_walkState;
    f32                                         m_jumpOutOfWaterCounter;
    ObjectRef                                   m_swimPolyline;
    u32                                         m_swimingSurfaceEdge;
    f32                                         m_swimingSurfaceT;
    u32                                         m_swimingClosestEdge;
    f32                                         m_swimingClosestEdgeT;
    bbool                                       m_swimingIsOnSurface;
    bbool                                       m_swimingIsInside;
    bbool                                       m_doingUturn;
};

class BTActionWalkToTarget_Template : public BTAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionWalkToTarget_Template,BTAction_Template,2552012380);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTActionWalkToTarget);

public:

    BTActionWalkToTarget_Template();
    ~BTActionWalkToTarget_Template() {}

    ITF_INLINE const StringID& getWalkAnim() const { return m_walkAnim; }
    ITF_INLINE const StringID& getSwimAnim() const { return m_swimAnim; }
    ITF_INLINE const StringID& getFallAnim() const { return m_fallAnim; }
    ITF_INLINE const StringID& getJumpAnim() const { return m_jumpAnim; }
    ITF_INLINE const StringID& getFactTargetActor() const { return m_factTargetActor; }
    ITF_INLINE const StringID& getFactTargetPos() const { return m_factTargetPos; }
    ITF_INLINE bbool           getCanPerformTurn() const { return m_performTurn; }
    ITF_INLINE bbool           getWallRun() const { return m_wallRun; }
    ITF_INLINE bbool           getForceSprint() const { return m_forceSprint; }

private:

    StringID                                m_walkAnim;
    StringID                                m_swimAnim;
    StringID                                m_fallAnim;
    StringID                                m_jumpAnim;
    StringID                                m_factTargetActor;
    StringID                                m_factTargetPos;
    bbool                                   m_performTurn;
    bbool                                   m_wallRun;
    bbool                                   m_forceSprint;
};

ITF_INLINE const BTActionWalkToTarget_Template*  BTActionWalkToTarget::getTemplate() const {return static_cast<const BTActionWalkToTarget_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONWALKTOTARGET_H_