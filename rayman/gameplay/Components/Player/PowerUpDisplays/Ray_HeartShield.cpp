#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_HEARTSHIELDCOMPONENT_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_HeartShield.h"
#endif //_ITF_RAY_HEARTSHIELDCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_HeartShield)
BEGIN_VALIDATE_OBJECT(Ray_HeartShield)
    VALIDATE_PARAM("heartActor", !getTemplate()->getHeartActor().isEmpty(), "Path mandatory");
    VALIDATE_PARAM("goldenHeartActor", !getTemplate()->getGoldenHeartActor().isEmpty(), "Path mandatory");
    VALIDATE_PARAM("diamondHeartActor", !getTemplate()->getDiamondHeartActor().isEmpty(), "Path mandatory");
END_VALIDATE_OBJECT()

//------------------------------------------------------------------------------
Ray_HeartShield::Ray_HeartShield()
    : Super()
    , m_initialized(bfalse)
    , m_previousHpAtMax(0)
    , m_previousTargetPos(Vec3d::Zero)
    , m_smoothedTargetPosSpeed(Vec3d::Zero)
    , m_heartGenerator()
    , m_goldenHeartGenerator()
    , m_diamondHeartGenerator()
    , m_heartRef()
    , m_currentHeartTier(HeartTier_None)
    , m_waitingForImmunityToSpawnRed(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_HeartShield::~Ray_HeartShield()
{
    // destroy heart
    destroyHeartActor(btrue);
}

//------------------------------------------------------------------------------
void Ray_HeartShield::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // declare actor spawnees
    if (!getTemplate()->getHeartActor().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_heartGenerator, getTemplate()->getHeartActor());
    }

    if (!getTemplate()->getGoldenHeartActor().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_goldenHeartGenerator, getTemplate()->getGoldenHeartActor());
    }

    if (!getTemplate()->getDiamondHeartActor().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_diamondHeartGenerator, getTemplate()->getDiamondHeartActor());
    }
}

//------------------------------------------------------------------------------
void Ray_HeartShield::onBecomeActive()
{
    // restore heart shield
    m_initialized = bfalse;
    m_waitingForImmunityToSpawnRed = bfalse;
}

//------------------------------------------------------------------------------
void Ray_HeartShield::onBecomeInactive()
{
    // fix heart shield remains (RO-13911 & RO-14672)
    destroyHeartActor(btrue);
    m_waitingForImmunityToSpawnRed = bfalse;
}

//------------------------------------------------------------------------------
void Ray_HeartShield::update(f32 _dt)
{
    Super::update(_dt);

    // heart follows player actor
    Actor* heart = m_heartRef.getActor();
    if (heart)
    {
        // current position
        Vec3d newPos = heart->getPos();

        // get target position
        Vec2d offset = getTemplate()->getPlayerFollowOffset();
        if (AIUtils::getFlipped(m_actor))
        {
            offset.m_x = -offset.m_x;
        }
        offset *= m_actor->getScale();
        Vec3d targetPos = m_actor->getPos() + offset.to3d();

        // smooth position depending on target speed
        AIUtils::advancedMichelSmooth(
            newPos,
            m_smoothedTargetPosSpeed,
            m_previousTargetPos,
            targetPos,
            getTemplate()->getSpeedBlend(),
            getTemplate()->getSpeedMin(),getTemplate()->getSpeedMax(),
            getTemplate()->getBlendAtSpeedMin(),
            getTemplate()->getBlendAtSpeedMax(),
            _dt
            );

        // set new pos
        heart->setPos(newPos);
    }

    // get state
    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayerFromActor(m_actor->getRef()));
    bbool hpAtMax = player ? (player->getHitPoints() == player->getMaxHitPoints()) : m_previousHpAtMax;

    EPlayerHeartTier currentTier = getCurrentHeartTier();
    if (heart && m_currentHeartTier != currentTier)
    {
        if (m_currentHeartTier == HeartTier_Golden && currentTier == HeartTier_Red)
        {
            destroyHeartActor(bfalse);
            heart = nullptr;
            m_currentHeartTier = currentTier;
            m_waitingForImmunityToSpawnRed = btrue;
        }
        else if (currentTier == HeartTier_None)
        {
            destroyHeartActor(bfalse);
            heart = nullptr;
            m_currentHeartTier = currentTier;
            m_waitingForImmunityToSpawnRed = bfalse;
        }
        else
        {
            destroyHeartActor(bfalse);
            heart = nullptr;
            m_currentHeartTier = currentTier;
            if (hpAtMax)
            {
                createHeartActor();
            }
        }
    }

    if (m_waitingForImmunityToSpawnRed && hpAtMax && currentTier == HeartTier_Red)
    {
        createHeartActor();
        m_waitingForImmunityToSpawnRed = bfalse;
    }
    else if (m_waitingForImmunityToSpawnRed && currentTier != HeartTier_Red)
    {
        m_waitingForImmunityToSpawnRed = bfalse;
    }

    // hp changed by something else, update power up
    // + set the initial state
    if (!m_initialized || hpAtMax != m_previousHpAtMax)
    {
        if (hpAtMax)
        {
            setEnabled(btrue);
            if (!heart)
            {
                createHeartActor();
            }
        }
        else
        {
            setEnabled(bfalse);
            if (heart)
            {
                destroyHeartActor(bfalse);
            }
        }
        m_initialized = btrue;
    }

    // remember state for next frame
    m_previousHpAtMax = hpAtMax;
}

//------------------------------------------------------------------------------
void Ray_HeartShield::reset()
{
    // NB[LaurentCou]: for some reason, heart shields disappear with the shooter
    // controller on checkpoint reload, this is a works around for this
    m_initialized = bfalse;
    m_waitingForImmunityToSpawnRed = bfalse;
}

//------------------------------------------------------------------------------
EPlayerHeartTier Ray_HeartShield::getCurrentHeartTier() const
{
    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayerFromActor(m_actor->getRef()));
    return player ? player->getHeartTier() : HeartTier_None;
}

//------------------------------------------------------------------------------
void Ray_HeartShield::createHeartActor()
{
    // guard
    if (!CURRENTWORLD)
    {
        return;
    }


    EPlayerHeartTier currentTier = getCurrentHeartTier();
    m_currentHeartTier = currentTier;

    SpawneeGenerator* generator = nullptr;
    switch (currentTier)
    {
        case HeartTier_Golden:
            ITF_WARNING(m_actor, !getTemplate()->getGoldenHeartActor().isEmpty(), "Golden Heart actor path not configured");
            generator = &m_goldenHeartGenerator;
            break;

        case HeartTier_Diamond:
            ITF_WARNING(m_actor, !getTemplate()->getDiamondHeartActor().isEmpty(), "Diamond Heart actor path not configured");
            generator = &m_diamondHeartGenerator;
            break;

        case HeartTier_Red:
        default:
            ITF_WARNING(m_actor, !getTemplate()->getHeartActor().isEmpty(), "Red Heart actor path not configured");
            generator = &m_heartGenerator;
            break;
    }

    Actor* heart = generator->getSpawnee(CURRENTWORLD->getRootScene(), m_actor->getPos());
    ITF_WARNING(m_actor, heart, "Can't spawn heart shield");
    if (heart)
    {
        heart->enable();
        m_heartRef = heart->getRef();
        m_actor->getBinding()->bindChild(m_heartRef);

        // store target pos
        m_previousTargetPos = m_actor->getPos();
    }
}
//------------------------------------------------------------------------------
void Ray_HeartShield::show( bbool _enable )
{
    Actor* heart = m_heartRef.getActor();
    if ( heart != NULL )
    {
        if ( _enable )
        {
            heart->enable();
        }
        else
        {
            heart->disable();
        }
    }
}

//------------------------------------------------------------------------------
void Ray_HeartShield::destroyHeartActor(bbool _force)
{
    // already destroyed? nothing to do
    if (!m_heartRef.isValid())
    {
        return;
    }

    // get heart
    Actor* heart = m_heartRef.getActor();

    // invalidate ref
    m_actor->getBinding()->unbindChild(m_heartRef);
    m_heartRef.invalidate();

    // actor's gone? nothing more to do
    if (!heart)
    {
        return;
    }

    // forced => direct destroy
    if (_force)
    {
        heart->disable();
        heart->requestDestruction();
    }
    // "natural" => play death behavior, will request the actor's destruction
    else
    {
        EventChangeBehaviourQuery bhvEvt;
        bhvEvt.setWantedBehaviourName(getTemplate()->getHeartDeathBhvName());
        heart->onEvent(&bhvEvt);
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_HeartShield_Template)
BEGIN_SERIALIZATION_CHILD(Ray_HeartShield_Template)
    SERIALIZE_MEMBER("heartActor", m_heartActor);
    SERIALIZE_MEMBER("goldenHeartActor", m_goldenHeartActor);
    SERIALIZE_MEMBER("diamondHeartActor", m_diamondHeartActor);
    SERIALIZE_MEMBER("heartDeathBhvName", m_heartDeathBhvName);
    SERIALIZE_MEMBER("playerFollowOffset", m_playerFollowOffset);
    SERIALIZE_MEMBER("speedBlend", m_speedBlend);
    SERIALIZE_MEMBER("speedMin", m_speedMin);
    SERIALIZE_MEMBER("speedMax", m_speedMax);
    SERIALIZE_MEMBER("blendAtSpeedMin", m_blendAtSpeedMin);
    SERIALIZE_MEMBER("blendAtSpeedMax", m_blendAtSpeedMax);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_HeartShield_Template::Ray_HeartShield_Template()
    : Super()
    , m_heartActor()
    , m_goldenHeartActor()
    , m_diamondHeartActor()
    , m_heartDeathBhvName()
    , m_playerFollowOffset(Vec2d::Zero)
    , m_speedBlend(1.0f)
    , m_speedMin(0.0f)
    , m_speedMax(1.0f)
    , m_blendAtSpeedMin(0.0f)
    , m_blendAtSpeedMax(1.0f)
{
}

//------------------------------------------------------------------------------
Ray_HeartShield_Template::~Ray_HeartShield_Template()
{
}

} // namespace ITF
