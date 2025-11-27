#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SUPERPUNCHGAUGE_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_SuperPunchGauge.h"
#endif //_ITF_RAY_SUPERPUNCHGAUGE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

//#define RAY_SUPERPUNCHGAUGE_DEBUG_LOG

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SuperPunchGauge)
BEGIN_VALIDATE_OBJECT_CHILD(Ray_SuperPunchGauge)
    VALIDATE_PARAM("fxControl", getTemplate()->getFxControl().isValid(), "Name mandatory");
    VALIDATE_PARAM("particleGenerator", getTemplate()->getParticleGenerator().isValid(), "Name mandatory");
    VALIDATE_PARAM("punchActor", !getTemplate()->getPunchActor().isEmpty(), "Path mandatory");
    VALIDATE_PARAM("visibleAmmo", getTemplate()->getVisibleAmmo()>0, "Positive value mandatory");
    VALIDATE_PARAM("", m_fxController, "FXControllerComponent mandatory");
END_VALIDATE_OBJECT()

//------------------------------------------------------------------------------
Ray_SuperPunchGauge::Ray_SuperPunchGauge()
    : Super()
    , m_initialized(bfalse)
    , m_fxHandle(U32_INVALID)
    , m_particleList()
    , m_spawneeGenerator()
    , m_fxController(NULL)
{
}

//------------------------------------------------------------------------------
Ray_SuperPunchGauge::~Ray_SuperPunchGauge()
{
}

//------------------------------------------------------------------------------
void Ray_SuperPunchGauge::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_fxController = m_actor->GetComponent<FXControllerComponent>();

    // declare actor spawnees
    if (!getTemplate()->getPunchActor().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_spawneeGenerator, getTemplate()->getPunchActor());
    }

    // register events
    PlayerControllerComponent * playerController = m_actor->GetComponent<PlayerControllerComponent>();
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629), playerController);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSuperPunchLaunch,1975677917), playerController);
}

//------------------------------------------------------------------------------
void Ray_SuperPunchGauge::onEvent(class Event * _event)
{
    Super::onEvent(_event);

    // process events
    if (EventReset* reset = _event->DynamicCast<EventReset>(ITF_GET_STRINGID_CRC(EventReset,2551104629)))
    {
        // generators are invalidated on EventReset, cleanup
        if (isFxPlaying())
        {
            cleanup();
        }
    }
    else if (Ray_EventSuperPunchLaunch* launch = _event->DynamicCast<Ray_EventSuperPunchLaunch>(ITF_GET_STRINGID_CRC(Ray_EventSuperPunchLaunch,1975677917)))
    {
        processLaunch(launch);
    }
}

//------------------------------------------------------------------------------
void Ray_SuperPunchGauge::update(f32 _dt)
{
    Super::update(_dt);
    
    // play FX is power-up is enabled
    if (isEnabled())
    {
        // power up just enabled or ammo changed
        if (getAmmo() != m_particleList.size())
        {
            if (isFxPlaying())
            {
                // was playing, stop before playing again
                stopFx();
            }
            if (!playFx(getTemplate()->getId(), getAmmo()))
            {
                // something went wrong, disable the power up
                setEnabled(bfalse);
            }
        }
    }
    // power up disabled
    else if (isFxPlaying())
    {
        stopFx();
    }
    
    // no delta-time, no-op
    if (_dt <= 0.0f)
    {
        return;
    }
    
    // update FX
    if (isFxPlaying())
    {
        updateFx(_dt);
    }
}

//------------------------------------------------------------------------------
void Ray_SuperPunchGauge::processLaunch(const Ray_EventSuperPunchLaunch* _launch)
{
    // skip if fx not playing
    if (!isFxPlaying())
    {
        return;
    }

#ifdef RAY_SUPERPUNCHGAUGE_DEBUG_LOG
    LOG("[%d] launch %s", CURRENTFRAME, getTemplate()->getParticleGenerator().getDebugString());
#endif

    // retrieve particle generator    
    ITF_ParticleGenerator* particleGenerator = getParticleGenerator();
    if (!particleGenerator)
    {
        cleanup();
        return;
    }

    // hide first particle
    particleGenerator->changeManualParticleState(m_particleList.front().m_index, bfalse);
    m_particleList.removeAt(0);

    // recompute alphas (RO-11297 + RO-11300)
    for (u32 i=0; i<m_particleList.size(); ++i)
    {
        const u32 visibleAmmo = getTemplate()->getVisibleAmmo();
        ITF_ASSERT(visibleAmmo>0);
        if (visibleAmmo>0 && i<visibleAmmo)
        {
            m_particleList[i].m_alpha = u8((255*(visibleAmmo-i))/visibleAmmo);
        }
        else
        {
            m_particleList[i].m_alpha = 0;
        }
    }
    
    // compute launch position
    f32 launchDistance = _launch->getReducedHit()
        ? getTemplate()->getReducedLaunchDistance()
        : getTemplate()->getLaunchDistance();
    Vec2d launchOffset = launchDistance * _launch->getDirection();
    launchOffset.m_y += _launch->getReducedHit()
        ? getTemplate()->getReducedLaunchYOffset()
        : getTemplate()->getLaunchYOffset();
    Vec3d launchPos = m_actor->getPos() + launchOffset.to3d();
    
    // spawn super punch actor
    Actor* superPunch = m_spawneeGenerator.getSpawnee(
        CURRENTWORLD->getRootScene(), launchPos, _launch->getDirection().getAngle());
    ITF_WARNING(m_actor, superPunch, "Can't spawn super punch");
    if (superPunch)
    {
        // enable + send to the launch direction
        superPunch->enable();
        EventSetDirection setDirection(_launch->getDirection());
        setDirection.setSender(m_actor->getRef());
        superPunch->onEvent(&setDirection);
    }
    
    // signal that some ammo was used to the power up manager
    useAmmo();
    
    // stop FX if no more ammo
    if (m_particleList.empty())
    {
        ITF_ASSERT(!isEnabled()); // should be disabled now
        stopFx();
    }
}

//------------------------------------------------------------------------------
ITF_ParticleGenerator* Ray_SuperPunchGauge::getParticleGenerator()
{
    // should not happen if fx isn't playing
    ITF_ASSERT(isFxPlaying());
    
    // get the particle generator from the fx controller
    ITF_ParticleGenerator* particleGenerator = m_fxController->getParticleGenerator(
        m_fxHandle, getTemplate()->getParticleGenerator());
    ITF_ASSERT_MSG(
        particleGenerator, "Can't retrieve particle generator %s",
        getTemplate()->getParticleGenerator().getDebugString()
        );
    return particleGenerator;
}

//------------------------------------------------------------------------------
void Ray_SuperPunchGauge::cleanup()
{
    // clear local fx infos
    m_fxHandle = U32_INVALID;
    m_particleList.clear();
}

//------------------------------------------------------------------------------
bbool Ray_SuperPunchGauge::isFxPlaying() const
{
    // safety check: the particle list and the handle should always be in sync
    bbool handleOk = (m_fxHandle != U32_INVALID);
    bbool listOk = !m_particleList.empty();
    ITF_ASSERT((handleOk && listOk) || (!handleOk && !listOk));
    return handleOk;
}

//------------------------------------------------------------------------------
bbool Ray_SuperPunchGauge::playFx(const StringID& _type, u32 _ammo)
{
#ifdef RAY_SUPERPUNCHGAUGE_DEBUG_LOG
    LOG("[%d] play %s", CURRENTFRAME, getTemplate()->getParticleGenerator().getDebugString());
#endif

    // play the FX
    m_fxHandle = m_fxController->playFX(getTemplate()->getFxControl());
    if (m_fxHandle == U32_INVALID)
    {
        ITF_WARNING(
            m_actor, bfalse, "Can't start FX %s",
            getTemplate()->getFxControl().getDebugString()
            );
        return bfalse;
    }

    // initialize particle list
    m_particleList.resize(_ammo);
    for (u32 i=0; i<m_particleList.size(); ++i)
    {
        m_particleList[i].m_index = i;
        m_particleList[i].m_pos = m_actor->getPos();
        m_particleList[i].m_previousTargetPos = m_actor->getPos();

        // fade out to invisible ammo (RO-11297 + RO-11300)
        const u32 visibleAmmo = getTemplate()->getVisibleAmmo();
        ITF_ASSERT(visibleAmmo>0);
        if (visibleAmmo>0 && i<visibleAmmo)
        {
            m_particleList[i].m_alpha = u8((255*(visibleAmmo-i))/visibleAmmo);
        }
        else
        {
            m_particleList[i].m_alpha = 0;
        }
    }
    
    // retrieve the particle generator
    ITF_ParticleGenerator* particleGenerator = getParticleGenerator();
    if (!particleGenerator)
    {
        cleanup();
        return bfalse;
    }
    
    // max particles guard
    ParticleGeneratorParameters* params = particleGenerator->getParameters();
    if (params->getMaxParticles()<_ammo)
    {
        ITF_WARNING(m_actor, bfalse, "Not enough particles");
        cleanup();
        return bfalse;
    }
    
    // setup particle generator
    params->setGeneratorMode(PARGEN_MODE_MANUAL);
    params->setPosition(m_actor->getPos());
    particleGenerator->spawnNParticle(_ammo, 0.0f);

    // play FX ok
    return btrue;
}

//------------------------------------------------------------------------------
void Ray_SuperPunchGauge::stopFx()
{
#ifdef RAY_SUPERPUNCHGAUGE_DEBUG_LOG
    LOG("[%d] stop %s", CURRENTFRAME, getTemplate()->getParticleGenerator().getDebugString());
#endif

    // this will delete the generator eventually
    m_fxController->stopFXFromHandle(m_fxHandle);
    
    // cleanup local info
    cleanup();
}

//------------------------------------------------------------------------------
void Ray_SuperPunchGauge::updateFx(f32 _dt)
{
    // retrieve the particle generator
    ITF_ParticleGenerator* particleGenerator = getParticleGenerator();
    if (!particleGenerator)
    {
        cleanup();
        return;
    }

    // init particles AABB
    AABB aabb(m_actor->get2DPos(), m_actor->get2DPos());

    // update each active super punch particle
    for (u32 i=0; i<m_particleList.size(); ++i)
    {
        // current position
        Vec3d newPos = m_particleList[i].m_pos;

        // target position & speed
        Vec3d targetPos;
        if (i==0)
        {
            Vec2d offset = getTemplate()->getPlayerFollowOffset();
            if (AIUtils::getFlipped(m_actor))
            {
                offset.m_x = -offset.m_x;
            }
            offset *= m_actor->getScale();
            targetPos = m_actor->getPos() + offset.to3d();
        }
        else
        {
            Vec2d offset = getTemplate()->getOthersFollowOffset();
            if (m_particleList[i-1].m_flipped)
            {
                offset.m_x = -offset.m_x;
            }
            offset *= m_actor->getScale();
            targetPos = m_particleList[i-1].m_pos + offset.to3d();
        }
        
        // smooth position depending on target speed
        AIUtils::advancedMichelSmooth(
            newPos,
            m_particleList[i].m_smoothedSpeed,
            m_particleList[i].m_previousTargetPos,
            targetPos,
            getTemplate()->getSpeedBlend(),
            getTemplate()->getSpeedMin(),getTemplate()->getSpeedMax(),
            getTemplate()->getBlendAtSpeedMin(),
            getTemplate()->getBlendAtSpeedMax(),
            _dt
            );

        // compute orientation
        Vec2d toActor = (m_actor->get2DPos() - newPos.truncateTo2D());
        bbool flipped = bfalse;
        f32 newAngle = 0.0f;
        if (!toActor.IsEqual(Vec2d::Zero, MTH_EPSILON))
        {
            Vec2d toActorNormalized = toActor;
            toActor.normalize();
            AIUtils::getAngleAndFlipped(toActor, newAngle, flipped);
        }

        // set position & orientation in generator        
        particleGenerator->changeManualParticleState(m_particleList[i].m_index, btrue);
        particleGenerator->changeManualParticleFlip(m_particleList[i].m_index, flipped);
        particleGenerator->changeManualParticleAlpha(m_particleList[i].m_index, m_particleList[i].m_alpha);
        particleGenerator->updateManualParticle(m_particleList[i].m_index, newPos, Vec3d::Zero, Angle(bfalse, newAngle));

        // grow particles AABB
        aabb.grow(newPos);

        // remember state
        m_particleList[i].m_pos = newPos;
        m_particleList[i].m_flipped = flipped;
    }
    
    // NB[LaurentCou]: this should be updated by the particle generator but
    // there is *no* way to do this properly at the moment; for this to work
    // we need computeAABB = TRUE (sic) set in the ITF_ParticleGenerator
    // template file
    m_actor->growRelativeAABBFromAbsolute(aabb);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SuperPunchGauge_Template)
BEGIN_SERIALIZATION_CHILD(Ray_SuperPunchGauge_Template)
    SERIALIZE_MEMBER("fxControl", m_fxControl);
    SERIALIZE_MEMBER("particleGenerator", m_particleGenerator);
    SERIALIZE_MEMBER("punchActor", m_punchActor);
    SERIALIZE_MEMBER("playerFollowOffset", m_playerFollowOffset);
    SERIALIZE_MEMBER("othersFollowOffset", m_othersFollowOffset);
    SERIALIZE_MEMBER("launchDistance", m_launchDistance);
    SERIALIZE_MEMBER("launchYOffset", m_launchYOffset);
    SERIALIZE_MEMBER("reducedLaunchDistance", m_reducedLaunchDistance);
    SERIALIZE_MEMBER("reducedLaunchYOffset", m_reducedLaunchYOffset);
    SERIALIZE_MEMBER("visibleAmmo", m_visibleAmmo);
    SERIALIZE_MEMBER("speedBlend", m_speedBlend);
    SERIALIZE_MEMBER("speedMin", m_speedMin);
    SERIALIZE_MEMBER("speedMax", m_speedMax);
    SERIALIZE_MEMBER("blendAtSpeedMin", m_blendAtSpeedMin);
    SERIALIZE_MEMBER("blendAtSpeedMax", m_blendAtSpeedMax);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_SuperPunchGauge_Template::Ray_SuperPunchGauge_Template()
    : Super()
    , m_fxControl()
    , m_particleGenerator()
    , m_punchActor()
    , m_playerFollowOffset(Vec2d::Zero)
    , m_othersFollowOffset(Vec2d::Zero)
    , m_launchDistance(0.0f)
    , m_launchYOffset(0.0f)
    , m_reducedLaunchDistance(0.0f)
    , m_reducedLaunchYOffset(0.0f)
    , m_visibleAmmo(3)
    , m_speedBlend(1.0f)
    , m_speedMin(0.0f)
    , m_speedMax(1.0f)
    , m_blendAtSpeedMin(0.0f)
    , m_blendAtSpeedMax(1.0f)
{
}

//------------------------------------------------------------------------------
Ray_SuperPunchGauge_Template::~Ray_SuperPunchGauge_Template()
{
}

} // namespace ITF
