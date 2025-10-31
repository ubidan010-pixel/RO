#ifndef _ITF_RAY_BTACTIONRUNDARKTOONIFIED_H_
#define _ITF_RAY_BTACTIONRUNDARKTOONIFIED_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

class Ray_BTActionRunDarktoonified : public BTAction
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionRunDarktoonified,BTAction,4174691184);

public:

    Ray_BTActionRunDarktoonified();
    ~Ray_BTActionRunDarktoonified() {}

    virtual void                                init( BehaviorTree* _tree, const BTNode_Template* _template );
    virtual bbool                               validate( Actor* _actor ) const;
    virtual void                                onActivate();
    virtual void                                onDeactivate();
    virtual bbool                               update( f32 _dt );
    virtual void                                updateAnimInput();
    virtual void                                onEvent(Event * _event);

protected:

    enum WalkState
    {
        WALKSTATE_UNKNOWN,
        WALKSTATE_GROUND,
        WALKSTATE_FALL,
        WALKSTATE_SWIM,
        WALKSTATE_CRASHED,
        WALKSTATE_FLYFALL,
    };

    // Ground
    void                                        updateGround( f32 _dt );

    // Fall
    void                                        updateFall( f32 _dt );

    // Swim
    void                                        updateSwim( f32 _dt );
    bbool                                       swimHandleInsideOutside( f32 _dt, const PolyLineEdge* _closestEdge );

    // Crashed
    void                                        updateCrashed( f32 _dt );

    // Fly-fall
    void                                        updateFlyFall( f32 _dt );

    bbool                                       checkStateChange();
    class PolyLine*                             getSwimPolyline() const;
    void                                        setState( WalkState _state );

    const class Ray_BTActionRunDarktoonified_Template*  getTemplate() const;

    class Ray_GroundAIControllerComponent*      m_aiController;
    class StickToPolylinePhysComponent*         m_physComponent;
    class AnimatedComponent*                    m_animComponent;
    AIUtils::PolylineRegionList                 m_currentRegions;
    WalkState                                   m_walkState;
    ObjectRef                                   m_swimPolyline;
    u32                                         m_swimingSurfaceEdge;
    f32                                         m_swimingSurfaceT;
    u32                                         m_swimingClosestEdge;
    f32                                         m_swimingClosestEdgeT;
    Vec2d                                       m_moveDir;
    ObjectRef                                   m_blockedPolyline;
    u32                                         m_blockedEdgeIndex;
    f32                                         m_crashedDist;
    f32                                         m_runningTime;
    f32                                         m_launchIdleTimer;
    f32                                         m_stayIdleTimer;
    f32                                         m_flyFallStayInAirTimer;
    bbool                                       m_swimingIsOnSurface;
    bbool                                       m_swimingIsInside;
    bbool                                       m_wasBlocked;
};

class Ray_BTActionRunDarktoonified_Template : public BTAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionRunDarktoonified_Template,BTAction_Template,3833787209);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(Ray_BTActionRunDarktoonified);

public:

    Ray_BTActionRunDarktoonified_Template();
    ~Ray_BTActionRunDarktoonified_Template() {}

    ITF_INLINE const StringID&              getWalkAnim() const { return m_walkAnim; }
    ITF_INLINE const StringID&              getSwimAnim() const { return m_swimAnim; }
    ITF_INLINE const StringID&              getFallAnim() const { return m_fallAnim; }
    ITF_INLINE const StringID&              getCrashAnim() const { return m_crashWallAnim; }
    ITF_INLINE const StringID&              getFlyFallAnim() const { return m_flyfallAnim; }
    ITF_INLINE f32                          getLaunchIdleMinTime() const { return m_launchIdleMinTime; }
    ITF_INLINE f32                          getLaunchIdleMaxTime() const { return m_launchIdleMaxTime; }
    ITF_INLINE f32                          getStayIdleMinTime() const { return m_stayIdleMinTime; }
    ITF_INLINE f32                          getStayIdleMaxTime() const { return m_stayIdleMaxTime; }
    ITF_INLINE f32                          getFlyFallSpeedMultiplier() const { return m_flyfallSpeedMultiplier; }
    ITF_INLINE bbool                        getCanRunInTheAir() const { return m_runInTheAir; }

private:

    StringID                                m_walkAnim;
    StringID                                m_swimAnim;
    StringID                                m_fallAnim;
    StringID                                m_crashWallAnim;
    StringID                                m_flyfallAnim;
    f32                                     m_launchIdleMinTime;
    f32                                     m_launchIdleMaxTime;
    f32                                     m_stayIdleMinTime;
    f32                                     m_stayIdleMaxTime;
    f32                                     m_flyfallSpeedMultiplier;
    bbool                                   m_runInTheAir;
};

ITF_INLINE const Ray_BTActionRunDarktoonified_Template*  Ray_BTActionRunDarktoonified::getTemplate() const {return static_cast<const Ray_BTActionRunDarktoonified_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONRUNDARKTOONIFIED_H_

