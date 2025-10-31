#ifndef _ITF_TRAJECTORYFOLLOWERCOMPONENT_H_
#define _ITF_TRAJECTORYFOLLOWERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_TRAJECTORYDATA_H_
#include "gameplay/Components/Trajectory/TrajectoryData.h"
#endif //_ITF_TRAJECTORYDATA_H_

namespace ITF {

class AnimatedComponent;
class EventTrigger;
class LinkComponent;
class TrajectoryNodeComponent;

//------------------------------------------------------------------------------
class TrajectoryFollowerComponent : public ActorComponent, public IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(TrajectoryFollowerComponent,ActorComponent,2852387072);
    DECLARE_VALIDATE_COMPONENT()

public:
    DECLARE_SERIALIZE()

    TrajectoryFollowerComponent();
    virtual ~TrajectoryFollowerComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }
    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onEvent(Event * _event);
    virtual void Update( f32 _deltaTime );
    virtual void updateAnimInput();
 
    void initFlip();
    void activate();
    void activate(const ActorRefList& _nodeList);
    void activate(TrajectoryNodeComponent* _node);
    void deactivate();
    ITF_INLINE bbool isActive() const { return m_activated; }
    void reverse();

#ifdef ITF_SUPPORT_EDITOR
    virtual void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif

    ITF_INLINE const TrajectoryLinkInfo& getLinkInfo() { return m_data.getCurrentLink(); }
    ITF_INLINE f32 getLinkCursor() { return m_data.getLinkCursor(); }

    ITF_INLINE f32 getSpeed() const { return m_speed; }
    void setRuntimeSpeed(f32 _speed, bbool _smooth);
    
    ITF_INLINE bbool getIsWaiting() const { return m_isWaiting; }
    ITF_INLINE void setIsWaiting(bbool _val) { m_isWaiting = _val; }

    ITF_INLINE const ActorRefList& getNodeList() const { return m_nodeList; }

    ITF_INLINE bbool hasPreviousNode() const { return m_data.getPreviousNodeRef().isValid(); }
    ITF_INLINE const ActorRef& getPreviousNode() const { return m_data.getPreviousNodeRef(); }

    ITF_INLINE const ActorRef& getCurrentNode() const { return m_data.getCurrentNodeRef(); }

    ITF_INLINE bbool hasNextNode() const { return m_data.getNextNodeRef().isValid(); }
    ITF_INLINE const ActorRef& getNextNode() const { return m_data.getNextNodeRef(); }
    
private:
    ITF_INLINE const class TrajectoryFollowerComponent_Template*  getTemplate() const;

    void processTrigger(EventTrigger* _triggerEvt);
    TrajectoryNodeComponent* selectNextNode(TrajectoryNodeComponent* _node);
    TrajectoryNodeComponent* getNode(const ActorRef& _nodeActor);
    void updatePosAndAngle(f32 _dt, const Vec3d & _cameraOffset);

    bbool m_activated;
    ActorRefList m_nodeList;
    TrajectoryData m_data;
    bbool m_linkCursorJustReset;
    
    f32 m_speed;
    f32 m_targetSpeed;
    bbool m_useRuntimeSpeed;
    
    bbool m_isWaiting;
    bbool m_flipDirection;

    Vec2d m_cameraPreviousPos;
    Vec2d m_cameraOffset;

    bbool m_activateNextTime;

    LinkComponent* m_linkComponent;
    AnimatedComponent* m_animatedComponent;
};

//------------------------------------------------------------------------------
class TrajectoryFollowerComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TrajectoryFollowerComponent_Template,TemplateActorComponent,1567319308);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(TrajectoryFollowerComponent);

public:
    TrajectoryFollowerComponent_Template();
    ~TrajectoryFollowerComponent_Template() {}

    ITF_INLINE f32 getSpeed() const { return m_speed; }
    ITF_INLINE f32 getSpeedSmoothFactor() const { return m_speedSmoothFactor; }
    ITF_INLINE bbool getCameraRelative() const { return m_cameraRelative; }
    ITF_INLINE bbool getSkipRetrigger() const { return m_skipRetrigger; }
    ITF_INLINE bbool getDestroyAtEnd() const { return m_destroyAtEnd; }
    ITF_INLINE bbool getUpdatesAnimInput() const { return m_updatesAnimInput; }
    ITF_INLINE bbool getStartsActivated() const {return m_startsActivated;}
    ITF_INLINE bbool getUpdatesAngle() const {return m_updatesAngle;}
    ITF_INLINE bbool getUpdatesZ() const {return m_updatesZ;}

    ITF_INLINE f32 getSafeDistance() const { return m_safeDistance; }
    ITF_INLINE f32 getMinSpeed() const { return m_minSpeed; }
    ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }

private:
    f32 m_speed;
    f32 m_speedSmoothFactor;
    bbool m_cameraRelative;
    bbool m_skipRetrigger;
    bbool m_destroyAtEnd;
    bbool m_updatesAnimInput;
    bbool m_startsActivated;
    bbool m_updatesAngle;
    bbool m_updatesZ;

    // to keep safe distance in a similar fashion than Chest2 AI (RO-13180)
    // NB[LaurentCou]: if safeDistance = 0.0f (default), not used
    f32 m_safeDistance;
    f32 m_minSpeed;
    f32 m_maxSpeed;
};

//------------------------------------------------------------------------------
ITF_INLINE
const TrajectoryFollowerComponent_Template*
TrajectoryFollowerComponent::getTemplate() const
{
    return static_cast<const TrajectoryFollowerComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_TRAJECTORYFOLLOWERCOMPONENT_H_
