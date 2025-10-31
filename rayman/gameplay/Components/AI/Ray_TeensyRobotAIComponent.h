
#ifndef _ITF_RAY_TEENSYROBOTAICOMPONENT_H_
#define _ITF_RAY_TEENSYROBOTAICOMPONENT_H_

#ifndef _ITF_BTAICOMPONENT_H_
#include "gameplay/Components/AI/BTAIComponent.h"
#endif //_ITF_BTAICOMPONENT_H_

namespace ITF
{

class Ray_TeensyRobotAIComponent : public BTAIComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TeensyRobotAIComponent,BTAIComponent,47198029)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_TeensyRobotAIComponent();
    virtual ~Ray_TeensyRobotAIComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        Update( f32 _dt );
    virtual void                        onEvent( Event * _event);
    virtual void                        updateAnimInput();

    virtual void                        onBecomeActive();
    virtual void                        onBecomeInactive();
    virtual void                        onDepthChanged( f32 _oldZ, f32 _newZ );

protected:

    const class Ray_TeensyRobotAIComponent_Template* getTemplate() const;

    class StickToPolylinePhysComponent* getPhys() const { return static_cast<StickToPolylinePhysComponent*>(m_physComponent); }

    void                                cleanFacts();
    void                                checkSquash();
    void                                triggerDeath();

    void                                receiveWaypoint( class Ray_EventAINetworkWaypoint* _waypoint );
    void                                receivePunch( class PunchStim* _punch );
    void                                receiveCrush( class EventCrushed* _crushed );
    void                                receiveInteractionQuery(EventInteractionQuery * _event);

    void                                registerObjects();
    void                                unregisterObjects();

    class Ray_GroundAIControllerComponent*  m_aiController;

    bbool                               m_registered;

    // ai network
    ActorRef                            m_aiNetworkCurrentWaypoint;
};

class Ray_TeensyRobotAIComponent_Template : public BTAIComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TeensyRobotAIComponent_Template,BTAIComponent_Template,842322961);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_TeensyRobotAIComponent);

public:

    Ray_TeensyRobotAIComponent_Template();
    ~Ray_TeensyRobotAIComponent_Template();

private:

};

ITF_INLINE const class Ray_TeensyRobotAIComponent_Template* Ray_TeensyRobotAIComponent::getTemplate() const
{
    return static_cast<const class Ray_TeensyRobotAIComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_TEENSYROBOTAICOMPONENT_H_

