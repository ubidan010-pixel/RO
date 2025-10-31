
#ifndef _ITF_RAY_FRIENDLYBTAICOMPONENT_H_
#define _ITF_RAY_FRIENDLYBTAICOMPONENT_H_

#ifndef _ITF_BTAICOMPONENT_H_
#include "gameplay/Components/AI/BTAIComponent.h"
#endif //_ITF_BTAICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{

class Ray_FriendlyBTAIComponent : public BTAIComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_FriendlyBTAIComponent,BTAIComponent,896114598)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_FriendlyBTAIComponent();
    virtual ~Ray_FriendlyBTAIComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        onActorClearComponents();
    virtual void                        onCheckpointLoaded();
    virtual void                        Update( f32 _dt );
    virtual void                        onEvent( Event * _event);
    virtual void                        updateAnimInput();

    virtual void                        onBecomeActive();

    virtual void                        onBecomeInactive();

    virtual void                        onDepthChanged( f32 _oldZ, f32 _newZ );

protected:

    const class Ray_FriendlyBTAIComponent_Template* getTemplate() const;

    class StickToPolylinePhysComponent* getPhys() const { return static_cast<StickToPolylinePhysComponent*>(m_physComponent); }

    void                                cleanFacts();
    void                                checkSquash();
    void                                triggerDeath();

    void                                receiveOrder( class Ray_EventAIOrderBT* _order );
    void                                receiveOrderSetTargetWaypoint( class Ray_EventAIOrderBT* _order );
    void                                receiveOrderSetRespawnPoint( class Ray_EventAIOrderBT* _order );
    void                                receiveOrderSetDetectionArea( class Ray_EventAIBTSetDetectionArea* _order );
    void                                receiveDisableOrder( class Ray_EventDisableAIOrderBT* _order );
    void                                receiveWaypoint( class Ray_EventAINetworkWaypoint* _waypoint );
    void                                receiveTeleportUsed( class Ray_EventTeleportUsed* _teleportUsed );
    void                                receiveDance( class Ray_EventDance* _dance );
    void                                receiveActivateStone( class Ray_EventActivateStone* _activateStone );
    void                                receivePunch( class PunchStim* _punch );
    void                                receiveCrush( class EventCrushed* _crushed );
    void                                receiveRestoreDeath( Ray_EventRestoreDeath* _restoreDeath );
    void                                receiveRescued( Ray_EventRescueFriendly* _rescued );
    void                                receiveQueryCanStickOrCollide( EventQueryCanStickOrCollide* _queryCanStick );
    void                                receiveInteractionQuery(EventInteractionQuery * _event);
    void                                registerObjects();
    void                                unregisterObjects();

    void                                releaseDarktoon( Event* _event );
    void                                setRescued();

    class Ray_GroundAIControllerComponent*  m_aiController;

    ObjectRef                           m_prevOrder;
    ObjectRef                           m_usedTeleporter;
    ObjectPath                          m_respawnPoint;
    ObjectPath                          m_targetWaypoint;
    f32                                 m_retaliateTimer;
    SpawneeGenerator                    m_darktoonSpawner;
    bbool                               m_rescued;
    bbool                               m_rescueFinished;
    bbool                               m_registered;

    // ai network
    ActorRef                            m_aiNetworkCurrentWaypoint;
};

class Ray_FriendlyBTAIComponent_Template : public BTAIComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_FriendlyBTAIComponent_Template,BTAIComponent_Template,1728992287);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FriendlyBTAIComponent);

public:

    Ray_FriendlyBTAIComponent_Template();
    ~Ray_FriendlyBTAIComponent_Template();

    f32                             getRetaliationDuration() const { return m_retaliationDuration; }
    f32                             getSquashDeathPenetration() const { return m_squashDeathPenetration; }
    ITF_INLINE const Path &         getDarktoonPath() const {return m_darktoonPath;}
    bbool                           getDisappearOnRescue() const { return m_disappearOnRescue; }

private:

    f32                             m_retaliationDuration;
    f32                             m_squashDeathPenetration;
    Path                            m_darktoonPath;
    bbool                           m_disappearOnRescue;
};

ITF_INLINE const class Ray_FriendlyBTAIComponent_Template* Ray_FriendlyBTAIComponent::getTemplate() const
{
    return static_cast<const class Ray_FriendlyBTAIComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_FRIENDLYBTAICOMPONENT_H_

