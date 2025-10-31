#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    BEGIN_SERIALIZATION_SUBCLASS(Ray_PlayerControllerComponent, StateDeadSoul_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("phantomShape", m_phantomShape, PHYSWORLD->getShapesFactory());
        SERIALIZE_MEMBER("softCollisionRadiusMultiplier", m_softCollisionRadiusMultiplier);
        SERIALIZE_MEMBER("ignoreCollisions", m_ignoreCollisions);

        SERIALIZE_MEMBER("minLocalSpeed", m_minLocalSpeed);
        SERIALIZE_MEMBER("maxLocalSpeed", m_maxLocalSpeed);
        SERIALIZE_MEMBER("maxLocalSpeedWithBoost", m_maxLocalSpeedWithBoost);
        SERIALIZE_MEMBER("inputInertia", m_inputInertia);
        SERIALIZE_MEMBER("camSpeedInfluence", m_camSpeedInfluence);

        SERIALIZE_MEMBER("ejectFromBorderForce", m_ejectFromBorderForce);
        SERIALIZE_MEMBER("ejectMargin", m_ejectMargin);

        //SERIALIZE_MEMBER("minSpeedForRotation", m_minSpeedForRotation);
        SERIALIZE_MEMBER("minInputForRotation", m_minInputForRotation);
        SERIALIZE_MEMBER("maxAngle", m_maxAngle);
        SERIALIZE_MEMBER("angularSmoothMinSpeed", m_angularSmoothMinSpeed);
        SERIALIZE_MEMBER("angularSmoothMaxSpeed", m_angularSmoothMaxSpeed);
        SERIALIZE_MEMBER("angularSmoothMinValue", m_angularSmoothMinValue);
        SERIALIZE_MEMBER("angularSmoothMaxValue", m_angularSmoothMaxValue);

        SERIALIZE_MEMBER("useTrail", m_useTrail);
    END_SERIALIZATION()

    Ray_PlayerControllerComponent::StateDeadSoul_Template::StateDeadSoul_Template()
        : m_phantomShape(NULL)
        , m_softCollisionRadiusMultiplier(1.f)
        , m_ignoreCollisions(btrue)

        , m_minLocalSpeed(0)
        , m_maxLocalSpeed(10)
        , m_maxLocalSpeedWithBoost(15)
        , m_inputInertia(0.03f)
        , m_camSpeedInfluence(0.5f)

        , m_ejectFromBorderForce(100)
        , m_ejectMargin(Vec2d::One)
        //, m_minSpeedForRotation(1.f)
        , m_minInputForRotation(0.1f)
        , m_maxAngle(btrue, 30)
        , m_angularSmoothMinSpeed(0.f)
        , m_angularSmoothMaxSpeed(10.f)
        , m_angularSmoothMinValue(0.1f)
        , m_angularSmoothMaxValue(0.4f)
        , m_useTrail(bfalse)
    {
    }

    Ray_PlayerControllerComponent::StateDeadSoul_Template::~StateDeadSoul_Template()
    {
        SF_DEL(m_phantomShape);
    }

    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateDeadSoul)

    Ray_PlayerControllerComponent::StateDeadSoul::StateDeadSoul()
        : m_moveToPointTargetCurrent(Vec2d::Zero)
        , m_doingMoveToPoint(bfalse)
        , m_moveInput(Vec2d::Zero)
        , m_currentAngle(0.f)
        , m_targetAngle(0.f)
        , m_revivePlayer(ITF_INVALID_OBJREF)
        , m_reviveDir(Vec2d::Zero)
        , m_revivePos(Vec2d::Zero)
        , m_inactiveTime(0.f)
    {
    }

    Ray_PlayerControllerComponent::StateDeadSoul::~StateDeadSoul()
    {
    }

    const Ray_PlayerControllerComponent::StateDeadSoul_Template* Ray_PlayerControllerComponent::StateDeadSoul::getTemplate() const
    {
        return &m_parent->getTemplate()->getStateDeadSoul();
    }


    void Ray_PlayerControllerComponent::StateDeadSoul::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::onEnter()
    {
        Super::onEnter();

        // Set faction : player -> ghost
        m_parent->setFaction(m_parent->getTemplate()->getDeadSoulFaction());
        
        m_parent->startDeadSoulFx();

        m_animComponent->enableDraw(btrue);

        if (getTemplate()->getUseTrail())
        {
            EventTrail showTrail(btrue);
            m_actor->onEvent(&showTrail);
        }

        // HACK: we don't control the flip of the dead soul (+curve cursor only works when not flipped :s)
        m_parent->setLookRight(btrue);

        m_parent->setCurrentPhantomShape(getTemplate()->getPhantomShape());

        m_parent->m_softCollisionRadiusMultiplier = getTemplate()->getSoftCollisionRadiusMultiplier();


        m_parent->setGravityMode(GRAVITYMODE_DEADSOUL,0.f);
        m_characterPhys->setWindMultiplier(0.f);

        if (getTemplate()->getIgnoreCollisions())
        {
            m_characterPhys->setProcessContacts(bfalse);
            m_characterPhys->unstick();
        }
        m_characterPhys->setUpdateRotation(bfalse);
        m_characterPhys->setSpeed(Vec2d::Zero);


        m_moveInput = Vec2d::Zero;
        m_currentAngle = 0;
        m_targetAngle = 0;

        m_revivePlayer.invalidate();
        m_reviveDir = Vec2d::Zero;
        m_inactiveTime = 0.f;
        
        m_parent->startDeathDropLum();

		// reset bubble_mode timer//
		m_parent->m_stateDeadSoulTimer = 0.f;

        // Since we can trigger the bubble state from the cheats too, keep this here
        CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());

        if ( m_parent->m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_parent->m_playerIndex));

            player->setWasHitInLevel(btrue);
        }

        // TODO: go to screen edge
/*
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
            if (player && player->getActive() &&
                player->getHitPoints() &&
                player->getIndex() != m_parent->getPlayerIndex())
            {
                if ( Actor* otherPlayerActor = player->getActor() )
                {
                    setMoveToPoint(otherPlayerActor->get2DPos());
                    break;
                }
            }
        }
*/
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::onExit()
    {
        Super::onExit();

        // Set faction : ghost -> player
        m_parent->setFaction(m_parent->getTemplate()->getFaction());

        m_parent->stopDeadSoulFx();

        if (getTemplate()->getUseTrail())
        {
            EventTrail hideTrail(bfalse);
            m_actor->onEvent(&hideTrail);
        }

        m_parent->setPhantomShape();
        m_parent->m_softCollisionRadiusMultiplier = m_parent->getTemplate()->getSoftCollisionRadiusMultiplier();


        m_parent->resetGravityMode(GRAVITYMODE_DEADSOUL);
        m_characterPhys->setWindMultiplier(1.f);
        m_characterPhys->setProcessContacts(btrue);
        m_characterPhys->setUpdateRotation(btrue);

        if ( m_parent->m_nextState != &m_parent->m_stateDeath && m_parent->m_nextState != &m_parent->m_stateRevive )
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_parent->m_playerIndex));
            player->setHitPoints(RAY_GAMEMANAGER->getStartHitPointsAfterDeath());

            if ( m_actor->isEnabled() )
            {
                CAMERACONTROLLERMANAGER->registerSubject(m_actor->getRef());
            }
        }
    }

    bbool Ray_PlayerControllerComponent::StateDeadSoul::checkStateChange( f32 _dt )
    {
        if (m_revivePlayer.isValid())
        {
            m_parent->m_stateRevive.setData(m_revivePlayer, m_reviveDir, m_revivePos);
            m_parent->changeState(&m_parent->m_stateRevive);

            return btrue;
        }

        if ( m_inactiveTime >= m_parent->getTemplate()->getDeadSoulInactiveOptOutTime() )
        {
            if ( !RAY_GAMEMANAGER->isSequencePlaying() && CINEMATICMANAGER->getLockPlayers() == 0 )
            {
                triggerDeath();
                return btrue;
            }
        }

        return bfalse;
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::triggerDeath()
    {
        m_parent->m_stateDeath.setExplodeDirectly(btrue);
        m_parent->m_stateDeath.setDeactivateOnFinish();
        m_parent->changeState(&m_parent->m_stateDeath);
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::processPunch( const PunchStim* _punch )
    {
        Actor* otherActor = AIUtils::getActor(_punch->getSender());

        if ( otherActor && isActorGoodForRevive(otherActor) )
        {
            bbool receiveDamage;
            bbool receiveHit = AIUtils::isEnemyHit(_punch, m_parent->getFaction(), &receiveDamage);
            bbool isFriendlyHit = receiveHit && !receiveDamage;

            if ( isFriendlyHit )
            {
                startRevive( _punch->getOriginalSender().isValid() ? _punch->getOriginalSender() : _punch->getSender(), _punch->getDirection() * (f32)(_punch->getLevel() + 1));
            }
        }
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::processCrush( EventCrushed* _crushed )
    {
        Actor* otherActor = AIUtils::getActor(_crushed->getSender());
        if ( otherActor && isActorGoodForRevive(otherActor) )
        {
            startRevive(_crushed->getSender(), _crushed->getDirection());
        }
    }

    bbool Ray_PlayerControllerComponent::StateDeadSoul::isActorGoodForRevive( Actor* _actor ) const
    {
        f32 initialScale = _actor->getWorldInitialScale().m_x;
        f32 currentScale = _actor->getScale().m_x;
        f32 scale = currentScale / initialScale;

        return scale == 1.f || scale == 0.5f;
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::startRevive( ActorRef _otherPlayer, const Vec2d& _dir )
    {
        m_revivePlayer = _otherPlayer;
        m_reviveDir = _dir;
        m_revivePos = _otherPlayer.getActor()->get2DPos();
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::reviveOnFirstLivePlayer()
    {
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            if (i == m_parent->m_playerIndex)
            {
                continue;
            }

            Player* otherPlayer = GAMEMANAGER->getPlayer(i);
            if (!otherPlayer ||
                !otherPlayer->getActive() ||
                otherPlayer->isDead())
            {
                continue;
            }

            Actor* otherActor = otherPlayer->getActor();
            if (otherActor && isActorGoodForRevive(otherActor))
            {
                startRevive(otherActor->getRef(), Vec2d::Right);
                return;
            }
        }
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::update( f32 _dt )
    {
        if (!m_parent->m_moveToPoint)
        {
            m_parent->updateSoftCollisions(_dt);
            move(_dt);
            clamp(_dt);
        }
        else
        {
            if ( !m_doingMoveToPoint )
            {
                m_moveToPointTargetCurrent = m_actor->get2DPos();
            }
            moveToPoint();
        }

        m_parent->updateDeathDropLum(_dt);

        m_doingMoveToPoint = m_parent->m_moveToPoint;
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::move( f32 _dt )
    {
        const StateDeadSoul_Template& stateTemplate = m_parent->getTemplate()->getStateDeadSoul();

        const Vec2d& currentVelocity = m_characterPhys->getSpeed();
        const Vec2d& camVelocity = CAMERACONTROLLERMANAGER->getCamSpeed().truncateTo2D();
 
        // input
        const Vec2d& targetInput = m_parent->getMoveDataAnalog().getMoveReal();
        m_moveInput += (targetInput - m_moveInput) * getTemplate()->getInputInertia();
        const f32 inputForce = m_moveInput.norm();
        const Vec2d inputDir = inputForce ? m_moveInput / inputForce : Vec2d::Zero;

        const bbool boosting = m_parent->m_listenerSprint;
        const f32 maxLocalSpeed = boosting ? stateTemplate.getMaxLocalSpeedWithBoost() : stateTemplate.getMaxLocalSpeed();
        const f32 targetSpeed = Interpolate(stateTemplate.getMinLocalSpeed(), maxLocalSpeed, inputForce);
        Vec2d targetVelocity = targetSpeed * inputDir + camVelocity * inputForce * getTemplate()->getCamSpeedInfluence();


        const Vec2d deltaVelocity = (targetVelocity - currentVelocity);
        m_characterPhys->addForce(deltaVelocity * maxLocalSpeed);

        const f32 speed = currentVelocity.norm();
        const f32 speedRatio = InverseInterpolate(stateTemplate.getAngularSmoothMinSpeed(), stateTemplate.getAngularSmoothMaxSpeed(), Clamp(speed, stateTemplate.getAngularSmoothMinSpeed(), stateTemplate.getAngularSmoothMaxSpeed()));
        const f32 angularSmoothFactor = Interpolate(stateTemplate.getAngularSmoothMinValue(), stateTemplate.getAngularSmoothMaxValue(), speedRatio);

        //const bbool ignoreInput = speed < stateTemplate.getMinSpeedForRotation();
        const bbool ignoreInput = inputForce < stateTemplate.getMinInputForRotation();

        if ( !ignoreInput )
        {
            //const Vec2d currentDir = currentVelocity / speed;
            const Vec2d currentDir = inputDir;
            f32 finalTargetAngle = -currentDir.dot(Vec2d::Right) * getTemplate()->getMaxAngle();
            m_targetAngle += getShortestAngleDelta(m_targetAngle, finalTargetAngle) * angularSmoothFactor;
        }
        else
        {
            f32 finalTargetAngle = 0;
            m_targetAngle += getShortestAngleDelta(m_targetAngle, finalTargetAngle) * 0.1f;
        }

        m_currentAngle += getShortestAngleDelta(m_currentAngle, m_targetAngle) * angularSmoothFactor;
        m_actor->setAngle( m_currentAngle );

        if ( m_parent->m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() || !targetInput.IsEqual(Vec2d::Zero,MTH_EPSILON) )
        {
            m_inactiveTime = 0.f;
        }
        else
        {
            m_inactiveTime += APPLOOPDT;
        }

#ifdef DEVELOPER_BEN
/*
        String msg;
        msg.setTextFormat("input %.0f%%", inputForce * 100);
        GFX_ADAPTER->drawDBGText(msg);
        //msg.setTextFormat("force %.2f", localForce.norm());
        //GFX_ADAPTER->drawDBGText(msg);
        msg.setTextFormat("speed %.2f", m_characterPhys->getSpeed().norm());
        GFX_ADAPTER->drawDBGText(msg);
        msg.setTextFormat("angle (target / current ): %.0f /  %.0f", m_targetAngle * MTH_RADTODEG, m_currentAngle * MTH_RADTODEG);
        GFX_ADAPTER->drawDBGText(msg);

        //GFX_ADAPTER->drawDBGCircle(m_actor->getPos(), 0.1f);
        //GFX_ADAPTER->drawDBG3DLine(m_actor->getPos(), m_actor->getPos() + moveInput.to3d());
*/
#endif
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::clamp( f32 _dt )
    {
        const StateDeadSoul_Template& stateTemplate = m_parent->getTemplate()->getStateDeadSoul();

        AABB screenAABB;
        if ( !CAMERACONTROLLERMANAGER->getAABB(m_actor->getDepth(), screenAABB) )
        {
            return;
        }

        screenAABB.setMin( screenAABB.getMin() + stateTemplate.getEjectMargin() );
        screenAABB.setMax( screenAABB.getMax() - stateTemplate.getEjectMargin() );
        const Vec2d& min = screenAABB.getMin();
        const Vec2d& max = screenAABB.getMax();

        Vec2d pos = m_actor->get2DPos();
        Vec2d penetration(Vec2d::Zero);
        if (pos.m_x < min.m_x)
            penetration.m_x = pos.m_x - min.m_x;
        else if (pos.m_x > max.m_x)
            penetration.m_x = pos.m_x - max.m_x;
        if (pos.m_y < min.m_y)
            penetration.m_y = pos.m_y - min.m_y;
        else if (pos.m_y > max.m_y)
            penetration.m_y = pos.m_y - max.m_y;

        // apply a force to move away from the screen border
        m_characterPhys->addForce(-penetration * stateTemplate.getEjectFromBorderForce());

#ifdef DEVELOPER_BEN
        //GFX_ADAPTER->drawDBGLine(m_actor->get2DPos(), m_actor->get2DPos() - penetration, Color::red());
        //GFX_ADAPTER->drawDBGAABB(CAMERACONTROLLERMANAGER->getAABB());
#endif
    }

    void Ray_PlayerControllerComponent::StateDeadSoul::moveToPoint()
    {
        Vec2d pos = m_actor->get2DPos();

        m_moveToPointTargetCurrent += (m_parent->m_moveToPointTarget - m_moveToPointTargetCurrent) * 0.1f;
        pos += (m_moveToPointTargetCurrent - pos) * 0.1f;
        m_actor->set2DPos(pos);

        f32 sqrDist = (m_parent->m_moveToPointTarget - pos).sqrnorm();
        if ( sqrDist < sqr(0.5f) )
        {
            m_parent->m_moveToPointFinished = btrue;
        }

        m_characterPhys->setSpeed(Vec2d::Zero);
    }

    Actor* Ray_PlayerControllerComponent::StateDeadSoul::getClosestRangePlayer()
    {
        const f32 myZ = m_actor->getDepth();

        // get the active player that has the closest Z
        Actor* closestPlayer = NULL;
        f32 closestDeltaZ = F32_INFINITY;

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            if (i == m_parent->getPlayerIndex())
            {
                continue;
            }

            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
            if ( !player || !player->getActive() || player->isDead() )
            {
                continue;
            }

            Actor* otherPlayerActor = player->getActor();
            if ( !otherPlayerActor )
            {
                continue;
            }

            const f32 deltaZ = f32_Abs(otherPlayerActor->getDepth() - myZ);
            if (deltaZ < closestDeltaZ)
            {
                closestDeltaZ = deltaZ;
                closestPlayer = otherPlayerActor;
            }
        }

        return closestPlayer;
    }
}
