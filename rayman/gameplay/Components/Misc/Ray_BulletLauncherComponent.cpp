#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BULLETLAUNCHERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_BulletLauncherComponent.h"
#endif //_ITF_RAY_BULLETLAUNCHERCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_BulletLauncherComponent)
BEGIN_SERIALIZATION_CHILD(Ray_BulletLauncherComponent)
    SERIALIZE_OBJECT( "timedSpawnerData",   m_timedSpawnerData );
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_BulletLauncherComponent)
    VALIDATE_COMPONENT_PARAM("bullet", !getTemplate()->getBullet().isEmpty(), "bullet path mandatory");
    VALIDATE_COMPONENT_PARAM("standAnim", getTemplate()->getStandAnim().isValid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("standToOpenAnim", getTemplate()->getStandToOpenAnim().isValid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("openAnim", !getTemplate()->useSeparateFire() || getTemplate()->getOpenAnim().isValid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("fireAnim", !getTemplate()->useSeparateFire() || getTemplate()->getFireAnim().isValid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("openToStandAnim", getTemplate()->getOpenToStandAnim().isValid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("useSeparateFire,useTimedSpawnerData", !getTemplate()->useSeparateFire() || !getTemplate()->useTimedSpawnerData(), "Incompatible flags");
    VALIDATE_COMPONENT_PARAM("", m_animLightComponent, "AnimLightComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_BulletLauncherComponent::Ray_BulletLauncherComponent()
    : Super()
    , m_state(State_NA)
    , m_launchBoneIndex(U32_INVALID)
    , m_bulletGenerator()
    , m_bulletRef()
    , m_animLightComponent(NULL)
    , m_tweenComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_BulletLauncherComponent::~Ray_BulletLauncherComponent()
{
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();
    m_tweenComponent = m_actor->GetComponent<TweenComponent>();

    // initialize spawner
    if (!getTemplate()->getBullet().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_bulletGenerator, getTemplate()->getBullet());
    }
    
    // register to activate event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBulletLaunch,1261820746), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

    // Timed spawner init
    m_timedSpawner.initialize( m_actor, &m_timedSpawnerData, NULL );
    m_timedSpawner.setActivate( bfalse );
    m_timedSpawner.warmSpawnOnly( btrue );
    if ( getTemplate()->useTimedSpawnerData() )
    {
        // Disable the tween
        if ( m_tweenComponent != NULL )
        {
            m_tweenComponent->suspendTweening();
        }        
    }
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::onBecomeActive()
{
    Super::onBecomeActive();

    if ( getTemplate()->useTimedSpawnerData() )
    {
        // Disable the tween
        if ( m_tweenComponent != NULL )
        {
            m_tweenComponent->suspendTweening();
        }        
    }

    if (getTemplate()->getStandAnim().isValid())
    {
        changeState(State_Stand);
    }
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::Update(f32 _dt)
{
    Super::Update(_dt);

    // timed spawn
    if ( getTemplate()->useTimedSpawnerData() )
    {
        m_timedSpawner.update( _dt );
        if ( m_timedSpawner.warmIsSpawnNeeded() )
        {
            Ray_EventBulletLaunch launchEvt;
            launchBullet( &launchEvt );
        }
    }
    
    updateState(_dt);
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);
    
    // start launching (i.e. opening)
    if (Ray_EventBulletLaunch* launch = _event->DynamicCast<Ray_EventBulletLaunch>(ITF_GET_STRINGID_CRC(Ray_EventBulletLaunch,1261820746)))
    {
        if (m_state != State_StandToOpen)
        {
            launchBullet(launch);
        }
    }
    else if (EventTrigger* trig = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if (getTemplate()->useTimedSpawnerData())
        {
            m_timedSpawner.setActivate( trig->getActivated() );
        }
        else if (m_state != State_StandToOpen)
        {
            // allow triggering from "official" sequencers
            // NB[LaurentCou]: if allowed (workaround RO-11874)
            if (getTemplate()->launchOnTrigger())
            {
                Ray_EventBulletLaunch bulletLaunchEvt;
                launchBullet(&bulletLaunchEvt);
            }
        }
    }
    else if (EventGeneric* genericEvent = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)))
    {
        if (genericEvent->getId() == ITF_GET_STRINGID_CRC(Open,812432275))
        {
            ITF_WARNING(
                m_actor, getTemplate()->useSeparateFire(),
                "Can't do that without useSeparateFire");
            changeState(State_StandToOpen);
        }
        else if (genericEvent->getId() == ITF_GET_STRINGID_CRC(Close,3883599349))
        {
            ITF_WARNING(
                m_actor, getTemplate()->useSeparateFire(),
                "Can't do that without useSeparateFire");
            changeState(State_OpenToStand);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::launchBullet(Ray_EventBulletLaunch* _launch)
{
    if (getTemplate()->useSeparateFire())
    {
        ITF_WARNING(m_actor, m_state==State_Open, "Firing with no prior opening");
        changeState(State_Fire);
    }
    else
    {
        changeState(State_StandToOpen);
    }

    // place on launch bone + Z offset
    // CAVEAT: we need the bullets to be *behind* friezes during launch, and
    // *in front* of them afterwards (e.g. walking bullets need to appear on
    // top of the floor), thus the Z offset applied only in launch phase
    m_launchBoneIndex = m_animLightComponent->getBoneIndex(getTemplate()->getLaunchBone());
    if (m_launchBoneIndex == U32_INVALID)
    {
        ITF_WARNING(m_actor, bfalse, "Can't find launch bone \"%s\"", getTemplate()->getLaunchBone().getDebugString());
        return;
    }
    Vec2d pos2d = Vec2d::Zero;
    m_animLightComponent->getBonePos(m_launchBoneIndex, pos2d);
    Vec3d pos(pos2d.m_x, pos2d.m_y, m_actor->getPos().m_z + getTemplate()->getLaunchZOffset());

    // actually spawn bullet, in root scene
    Actor* bullet = m_bulletGenerator.getSpawnee(
        getTemplate()->getLaunchInRootScene() ? CURRENTWORLD->getRootScene() : m_actor->getScene(),
        pos);
    if (!bullet)
    {
        ITF_WARNING(m_actor, bfalse, "Can't spawn bullet");
        return;
    }
    
    // flip it if necessary
    bullet->setFlipped(m_actor->isFlipped());

    bullet->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
    
    // disable physics for the launch phase (walking bullets would fall to the
    // floor otherwise)
    PhysComponent* bulletPhysComponent = bullet->GetComponent<PhysComponent>();
    if (bulletPhysComponent)
    {
        bulletPhysComponent->setDisabled(btrue);
    }

    // enable, start bullet's launch behavior and keep ref to move it ourselves
    bullet->enable();
    bullet->onEvent(_launch);
    m_bulletRef = bullet->getRef();
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::updateLaunch()
{
    Actor* bullet = m_bulletRef.getActor();
    if (bullet)
    {
        ITF_ASSERT(m_launchBoneIndex != U32_INVALID);
        Vec2d pos2d = Vec2d::Zero;
        m_animLightComponent->getBonePos(m_launchBoneIndex, pos2d);
        Vec3d pos(pos2d.m_x, pos2d.m_y, m_actor->getPos().m_z + getTemplate()->getLaunchZOffset());
        bullet->setPos(pos);
        f32 angle;
        
        Vec2d lookDir = Vec2d::Right.Rotate(
            m_actor->isFlipped()
            ? m_actor->getAngle() + MTH_PI
            : m_actor->getAngle());
        bbool flip;
        AIUtils::getAngleAndFlipped(lookDir , angle, flip);
        bullet->setFlipped( flip );
        bullet->setAngle( angle );
    }
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::freeBullet()
{
    Actor* bullet = m_bulletRef.getActor();
    if (!bullet)
    {
        return;
    }

    // actually launch the bullet, start its regular behavior
    Vec2d direction = m_actor->isFlipped()
        ? -Vec2d::Right.Rotate(m_actor->getAngle())
        : Vec2d::Right.Rotate(m_actor->getAngle());
    EventSetDirection setDirection(direction);
    setDirection.setSender(m_actor->getRef());
    bullet->onEvent(&setDirection);

    // re-enable physics
    PhysComponent* bulletPhysComponent = bullet->GetComponent<PhysComponent>();
    if (bulletPhysComponent)
    {
        bulletPhysComponent->setDisabled(bfalse);
    }

    // remove Z offset
    Vec3d pos = bullet->getPos();
    pos.m_z -= getTemplate()->getLaunchZOffset();
    bullet->setPos(pos);
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::changeState(State_t _newState)
{
    if (_newState ==  m_state)
    {
        return;
    }

    // finish previous state
    switch (m_state)
    {
    case State_Fire:
    {
        ITF_ASSERT(getTemplate()->useSeparateFire());

        // launch phase done, now the bullet lives free
        freeBullet();
    }
    break;
    }

    // switch to a new state
    switch (m_state = _newState)
    {
    case State_Stand:
    {
        m_animLightComponent->setAnim(getTemplate()->getStandAnim());
    }
    break;
    case State_StandToOpen:
    {
        m_animLightComponent->setAnim(getTemplate()->getStandToOpenAnim());
    }
    break;
    case State_Open:
    {
        m_animLightComponent->setAnim(getTemplate()->getOpenAnim());
    }
    break;
    case State_Fire:
    {
        m_animLightComponent->setAnim(getTemplate()->getFireAnim());
    }
    break;
    case State_OpenToStand:
    {
        m_animLightComponent->setAnim(getTemplate()->getOpenToStandAnim());
        
        // launch phase done, now the bullet lives free
        if (!getTemplate()->useSeparateFire())
        {
            freeBullet();
        }
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_BulletLauncherComponent::updateState(f32 _dt)
{
    switch (m_state)
    {
    case State_Stand: break; // nothing to do
    case State_StandToOpen:
    {
        if (getTemplate()->useSeparateFire())
        {
            // stand to open -> open
            if (m_animLightComponent->getCurSubAnim(0)->getFriendlyName() == getTemplate()->getStandToOpenAnim()
                && m_animLightComponent->isSubAnimFinished())
            {
                changeState(State_Open);
            }
        }
        else
        {
            // stand to open -> open to stand
            if (m_animLightComponent->getCurSubAnim(0)->getFriendlyName() == getTemplate()->getStandToOpenAnim()
                && m_animLightComponent->isSubAnimFinished())
            {
                changeState(State_OpenToStand);
            }
            // bullet follows our launch bone in launch phase
            else
            {
                updateLaunch();
            }
        }
    }
    break;
    case State_Open:
    {
        ITF_ASSERT(getTemplate()->useSeparateFire());
    }
    break;
    case State_Fire:
    {
        ITF_ASSERT(getTemplate()->useSeparateFire());

        // fire -> open
        if (m_animLightComponent->getCurSubAnim(0)->getFriendlyName() == getTemplate()->getFireAnim()
            && m_animLightComponent->isSubAnimFinished())
        {
            changeState(State_Open);
        }
        // bullet follows our launch bone in launch phase
        else
        {
            updateLaunch();
        }
    }
    break;
    case State_OpenToStand:
    {
        // open to stand -> stand
        if (m_animLightComponent->getCurSubAnim(0)->getFriendlyName() == getTemplate()->getOpenToStandAnim()
            && m_animLightComponent->isSubAnimFinished())
        {
            changeState(State_Stand);
        }
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_BulletLauncherComponent_Template)
BEGIN_SERIALIZATION(Ray_BulletLauncherComponent_Template)
    SERIALIZE_MEMBER("bullet", m_bullet);
    SERIALIZE_MEMBER("launchBone", m_launchBone);
    SERIALIZE_MEMBER("launchZOffset", m_launchZOffset);
    SERIALIZE_MEMBER("launchInRootScene", m_launchInRootScene);
    SERIALIZE_MEMBER("standAnim", m_standAnim);
    SERIALIZE_MEMBER("standToOpenAnim", m_standToOpenAnim);
    SERIALIZE_MEMBER("openAnim", m_openAnim);
    SERIALIZE_MEMBER("fireAnim", m_fireAnim);
    SERIALIZE_MEMBER("openToStandAnim", m_openToStandAnim);
    SERIALIZE_MEMBER("useSeparateFire", m_useSeparateFire);
    SERIALIZE_MEMBER("useTimedSpawnerData", m_useTimedSpawnerData );
    SERIALIZE_MEMBER("launchOnTrigger", m_launchOnTrigger);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_BulletLauncherComponent_Template::Ray_BulletLauncherComponent_Template()
    : Super()
    , m_bullet()
    , m_launchBone()
    , m_launchZOffset(0.0f)
    , m_launchInRootScene(btrue)
    , m_standAnim()
    , m_standToOpenAnim()
    , m_openAnim()
    , m_fireAnim()
    , m_openToStandAnim()
    , m_useSeparateFire(bfalse)
    , m_useTimedSpawnerData(bfalse)
    , m_launchOnTrigger(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_BulletLauncherComponent_Template::~Ray_BulletLauncherComponent_Template()
{
}

} // namespace ITF
