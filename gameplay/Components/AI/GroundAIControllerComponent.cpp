#include "precompiled_gameplay.h"

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(GroundAIControllerComponent)
BEGIN_SERIALIZATION_CHILD(GroundAIControllerComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(GroundAIControllerComponent)

    VALIDATE_COMPONENT_PARAM("", m_phys && m_phys->DynamicCast<StickToPolylinePhysComponent>(ITF_GET_STRINGID_CRC(StickToPolylinePhysComponent,3089159513)), "GroundAIControllerComponent requires a StickToPolylinePhysComponent");
    VALIDATE_COMPONENT_PARAM("", m_animComponent, "GroundAIControllerComponent requires a AnimLightcomponent");
    VALIDATE_COMPONENT_PARAM("", m_phantomComponent, "GroundAIControllerComponent requires a PhantomComponent");

END_VALIDATE_COMPONENT()

GroundAIControllerComponent::GroundAIControllerComponent()
: m_animComponent(NULL)
, m_phantomComponent(NULL)
, m_moveTargetBlendTimer(0.f)
, m_currentMoveTargetMultiplier(0.f)
, m_mustAvoid(bfalse)
, m_avoidanceCheckTimer(0.f)
, m_avoidanceSpeedFactor(1.f)
, m_currentNavMode(NAVMODE_NORMAL)
, m_nextNavMode(NAVMODE_NONE)
, m_swimPolyline(ITF_INVALID_OBJREF)
, m_waterResistance(0.f)
, m_swimingSurfaceEdge(U32_INVALID)
, m_swimingSurfaceT(0.f)
, m_swimingInertia(Vec2d::Zero)
, m_swimingTargetRotation(0.f)
, m_swimingCurrentAngularSpeed(0.f)
, m_swimingIsOnSurface(bfalse)
, m_swimingIsOnJumpRange(bfalse)
, m_controlledJumpStart(Vec3d::Zero)
, m_controlledJumpEnd(Vec3d::Zero)
, m_controlledJumpTimer(0.f)
, m_controlledJumpTimeTotal(0.f)
, m_sprintMaxSpeedBlendTimer(0.f)
, m_jump(bfalse)
, m_forceJump(bfalse)
, m_disablePhysics(bfalse)
, m_wallRun(bfalse)
, m_sprint(bfalse)
{
}

GroundAIControllerComponent::~GroundAIControllerComponent()
{
}

void GroundAIControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();
    m_phantomComponent = m_actor->GetComponent<PhantomComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsSprinting,2326390223),this);
}

void GroundAIControllerComponent::Update( f32 _dt )
{
    m_moveDirThisFrame = m_moveDir;

    if ( m_nextNavMode != NAVMODE_NONE )
    {
        setNavMode();
    }

    m_avoidanceCheckTimer -= _dt;

    if ( m_avoidanceCheckTimer <= _dt )
    {
        m_avoidanceCheckTimer = Seeder::getSharedSeeder().GetFloat(0.5f,0.8f);
        checkAvoidance();
    }

    bbool disabledPhysics = m_disablePhysics;

    switch(m_currentNavMode)
    {
        case NAVMODE_NORMAL:
            if ( getPhys()->getStickedEdge() )
            {
                updateMoveGround(_dt);
            }
            else
            {
                updateMoveAir(_dt);
            }
            break;
        case NAVMODE_SWIM:
            updateMoveSwim(_dt);
            break;
        case NAVMODE_CONTROLLEDJUMP:
            updateControlledJump(_dt);
            disabledPhysics = btrue;
            break;
    }

    clearFrame();

    m_phys->setDisabled(disabledPhysics);
    getPhys()->setStickOnWalls(m_wallRun);
}

void GroundAIControllerComponent::jumpToTarget( const Vec3d& _pos )
{
    if ( m_currentNavMode == NAVMODE_CONTROLLEDJUMP )
    {
        m_controlledJumpEnd = _pos;
    }
    else
    {
        m_controlledJumpPrevPos = m_actor->get2DPos();
        m_controlledJumpStart = m_actor->getPos();
        m_controlledJumpEnd = _pos;
        m_controlledJumpTimer = 0.f;
        m_nextNavMode = NAVMODE_CONTROLLEDJUMP;

        f32 dist = ( m_controlledJumpStart - m_controlledJumpEnd ).norm();

        m_controlledJumpTimeTotal = Max(dist / getTemplate()->getJumpToActorSpeed(),getTemplate()->getJumpToActorMinTime());
    }
}

void GroundAIControllerComponent::resetJumpToTarget()
{
    if ( m_currentNavMode == NAVMODE_CONTROLLEDJUMP )
    {
        m_nextNavMode = NAVMODE_NORMAL;
    }
}

void GroundAIControllerComponent::setSwimingPolylineData( ObjectRef _ref, u32 _surfaceEdge, f32 _surfaceEdgeT, bbool _isOnSurface )
{
    if ( _ref == ITF_INVALID_OBJREF )
    {
        m_nextNavMode = NAVMODE_NORMAL;
    }
    else
    {
        if ( m_currentNavMode != NAVMODE_SWIM )
        {
            m_nextNavMode = NAVMODE_SWIM;
        }
    }

    m_swimPolyline = _ref;
    m_swimingSurfaceEdge = _surfaceEdge;
    m_swimingSurfaceT = _surfaceEdgeT;
    m_swimingIsOnSurface = _isOnSurface;
}

void GroundAIControllerComponent::updateControlledJump( f32 _dt )
{
    if ( m_controlledJumpTimer == m_controlledJumpTimeTotal )
    {
        m_nextNavMode = NAVMODE_NORMAL;
        setNavMode();
    }

    m_controlledJumpTimer = Min(m_controlledJumpTimer+_dt,m_controlledJumpTimeTotal); 

    f32 t = m_controlledJumpTimer / m_controlledJumpTimeTotal;

    Vec2d p0XZ(m_controlledJumpStart.m_x,m_controlledJumpStart.m_z);
    Vec2d p3XZ(m_controlledJumpEnd.m_x,m_controlledJumpEnd.m_z);
    Vec2d dirXZ = p3XZ - p0XZ;
    Vec2d p1XZ = p0XZ + (dirXZ*getTemplate()->getJumpToActorXZFuncPoint0T());
    Vec2d p2XZ = p0XZ + (dirXZ*getTemplate()->getJumpToActorXZFuncPoint1T());
    Vec2d pXZ = getBezierPosition(p0XZ,p1XZ,p2XZ,p3XZ,t);

    f32 p0Y = m_controlledJumpStart.m_y;
    f32 p3Y = m_controlledJumpEnd.m_y;
    f32 p1Y = p0Y + getTemplate()->getJumpToActorYFuncPoint0Dist();
    f32 p2Y = p3Y + getTemplate()->getJumpToActorYFuncPoint1Dist();
    f32 pY = getBezierPosition(p0Y,p1Y,p2Y,p3Y,t);

    Vec3d newPos = Vec3d(pXZ.m_x,pY,pXZ.m_y);

    m_actor->setPos(newPos);

    Vec2d newP2D = newPos.truncateTo2D();
    Vec2d dif = newP2D - m_controlledJumpPrevPos;

    m_phys->setSpeed(dif/LOGICDT);

    m_controlledJumpPrevPos = newP2D;
}

void GroundAIControllerComponent::updateMoveGround( f32 _dt )
{
    bbool wantsToMove = m_moveDir != Vec2d::Zero;

    updateMoveTargetMultiplier(_dt,wantsToMove);

    if ( wantsToMove )
    {
        const Vec2d& groundDir = getPhys()->getCurrentGroundDir();
        Vec2d walkForce = groundDir * getTemplate()->getWalkForce();
        f32 xPos = groundDir.dot(m_moveDir);
        bbool flipped = xPos < 0.f;

        if ( flipped )
        {
            walkForce *= -1.f;
        }

        f32 efficiency = calculateEfficiency();

        Vec2d force = walkForce;
        // multiplied by engine force
        force *= m_currentMoveTargetMultiplier;
        // multiplied by efficiency
        force *= efficiency;

        getPhys()->addForce(force);
    }

    if ( m_jump )
    {
        performJump();
    }
}

f32 GroundAIControllerComponent::calculateEfficiency() const
{
    f32 efficiency = AIUtils::walkCalculateEfficiency(getPhys()->getSpeed().norm(),
        getWalkMaxSpeed(),
        getPhys()->getFrictionCoef(),
        getPhys()->getCurrentGravity().norm(),
        getPhys()->getCurrentGravityDir(),
        m_actor->getAngle(),
        getTemplate()->getWalkMinGroundAngleEfficiencyAngle().ToRadians(),
        getTemplate()->getWalkMaxGroundAngleEfficiencyAngle().ToRadians(),
        getTemplate()->getWalkMinGroundAngleEfficiencyMult(),
        getTemplate()->getWalkMaxGroundAngleEfficiencyMult(),
        !m_wallRun);

    return efficiency;
}

f32 GroundAIControllerComponent::getWalkMaxSpeed() const
{
    f32 speed = getTemplate()->getWalkEfficiencyMaxSpeed();

    if ( m_mustAvoid )
    {
        speed *= m_avoidanceSpeedFactor;
    }

    if ( getTemplate()->getSprintMaxSpeedBlendDuration() && m_sprintMaxSpeedBlendTimer )
    {
        f32 t = m_sprintMaxSpeedBlendTimer/getTemplate()->getSprintMaxSpeedBlendDuration();
        speed *= Interpolate(1.f,getTemplate()->getSprintEfficiencyMultiplier(),t);
    }

    return speed;
}

void GroundAIControllerComponent::updateMoveTargetMultiplier( f32 _deltaTime, bbool _move )
{
    bbool sprint = m_sprint;

    if ( _move )
    {
        m_moveTargetBlendTimer = Min(m_moveTargetBlendTimer+_deltaTime,getTemplate()->getWalkMoveTargetMultiplierBlendTime());
    }
    else
    {
        m_moveTargetBlendTimer = Max(m_moveTargetBlendTimer-_deltaTime,0.f);
        sprint = bfalse;
    }

    if ( sprint )
    {
        m_sprintMaxSpeedBlendTimer = Min(m_sprintMaxSpeedBlendTimer+_deltaTime,getTemplate()->getSprintMaxSpeedBlendDuration());
    }
    else
    {
        m_sprintMaxSpeedBlendTimer = Max(m_sprintMaxSpeedBlendTimer-_deltaTime,0.f);
    }

    if ( getTemplate()->getWalkMoveTargetMultiplierBlendTime() )
    {
        m_currentMoveTargetMultiplier = Interpolate(getTemplate()->getWalkMoveTargetMultiplierMin(),getTemplate()->getWalkMoveTargetMultiplierMax(),m_moveTargetBlendTimer/getTemplate()->getWalkMoveTargetMultiplierBlendTime());
    }
    else
    {
        m_currentMoveTargetMultiplier = getTemplate()->getWalkMoveTargetMultiplierMax();
    }
}

void GroundAIControllerComponent::updateMoveAir( f32 _dt )
{
    bbool wantsToMove = m_moveDir != Vec2d::Zero;

    updateMoveTargetMultiplier(_dt,wantsToMove);

    if ( wantsToMove )
    {
        Vec2d gravNormal = getPhys()->getCurrentGravityDir().getPerpendicular();
        Vec2d airForce = gravNormal * getTemplate()->getAirForce();
        f32 xMove = gravNormal.dot(m_moveDir);

        Vec2d move = gravNormal*xMove;
        bbool sameDir = move.dot(getPhys()->getSpeed()) >= 0.f;
        f32 currentSpeed = Min(f32_Abs(gravNormal.dot(getPhys()->getSpeed())),getTemplate()->getAirTopSpeed());
        Vec2d force;

        if ( sameDir )
        {
            force = move * (airForce*(1.f-(currentSpeed/getTemplate()->getAirTopSpeed())));
        }
        // If we are trying to move in the opposite direction then we apply the force normally
        else
        {
            force = move * airForce;
        }

        getPhys()->addForce(force);
    }
    else
    {
        Vec2d gravNormal = getPhys()->getCurrentGravityDir().getPerpendicular();
        f32 xSpeed = gravNormal.dot(getPhys()->getSpeed());

        if ( xSpeed )
        {
            Vec2d force = gravNormal*-xSpeed*getTemplate()->getAirFriction();
            getPhys()->addForce(force);
        }
    }

    if ( m_jump )
    {
        if ( m_forceJump )
        {
            performJump();
        }
        else
        {
            // Apply suspension
            f32 ySpeed = -getPhys()->getCurrentGravityDir().dot(getPhys()->getSpeed());

            if ( ySpeed > 0.f && !getPhys()->getStickedPolyline() && ySpeed < getTemplate()->getAirSuspensionMaxYSpeed() )
            {
                Vec2d gravity = -getPhys()->getCurrentGravity();

                ySpeed = Clamp(ySpeed,getTemplate()->getAirSuspensionMinYSpeed(),getTemplate()->getAirSuspensionMaxYSpeed());
                f32 t = ( ySpeed - getTemplate()->getAirSuspensionMinYSpeed() ) / ( getTemplate()->getAirSuspensionMaxYSpeed() - getTemplate()->getAirSuspensionMinYSpeed() );
                f32 multiplier = Interpolate(getTemplate()->getAirSuspensionMinMultiplier(),getTemplate()->getAirSuspensionMaxMultiplier(),t);
                getPhys()->addForce(gravity*multiplier);
            }
        }
    }
}

void GroundAIControllerComponent::updateMoveSwim( f32 _dt )
{
    PolyLine* poly = AIUtils::getPolyLine(m_swimPolyline);

    if ( poly )
    {
        Vec2d moveDir = m_moveDir;
        f32 moveSize = moveDir.norm();
        Vec2d swimForce = Vec2d::Zero;
        Vec2d floatForce = Vec2d::Zero;

        updateMoveTargetMultiplier(_dt,moveSize!=0.f);
        swimingUpdateAirFriction(poly);
        swimingUpdatePolyData(poly);
        swimUpdateDive(_dt,poly,moveDir,moveSize,swimForce,floatForce);
        swimApplyForces(_dt,swimForce,floatForce);
        swimUpdateRotation(_dt);
    }
}

void GroundAIControllerComponent::swimingUpdateAirFriction( PolyLine* _poly )
{
    const GameMaterial_Template* mat = World::getGameMaterial(_poly->getEdgeAt(0).getGameMaterial());

    if ( mat )
    {
        m_waterResistance = mat->getWaterResistance();
    }
    else
    {
        m_waterResistance = 0.f;
    }

    f32 waterResist = m_waterResistance;

    f32 speedSq = getPhys()->getSpeed().sqrnorm();

    if ( speedSq > (getTemplate()->getSwimingWaterResistMinSpeed()*getTemplate()->getSwimingWaterResistMinSpeed()) )
    {
        f32 speed = f32_Sqrt(speedSq);

        speed = Clamp(speed,getTemplate()->getSwimingWaterResistMinSpeed(),getTemplate()->getSwimingWaterResistMaxSpeed());

        f32 t = ( speed - getTemplate()->getSwimingWaterResistMinSpeed() ) / ( getTemplate()->getSwimingWaterResistMaxSpeed() - getTemplate()->getSwimingWaterResistMinSpeed() );
        f32 multiplier = Interpolate(1.f,getTemplate()->getSwimingWaterResistMaxMult(),t);

        waterResist *= multiplier;
    }

    getPhys()->setAirFrictionMultiplier(waterResist);
}

void GroundAIControllerComponent::swimingUpdatePolyData( PolyLine* _poly )
{
    m_swimingIsOnJumpRange = bfalse;

    if ( m_swimingSurfaceEdge != U32_INVALID )
    {
        const PolyLineEdge& edge = _poly->getEdgeAt(m_swimingSurfaceEdge);

        Vec2d surfacePoint = edge.getPos() + (edge.m_vector * m_swimingSurfaceT);
        f32 distToSurface = ( m_actor->get2DPos() - surfacePoint ).sqrnorm();

        if ( distToSurface <= ( getTemplate()->getSwimingJumpRange() * getTemplate()->getSwimingJumpRange() ) )
        {
            Vec2d checkPos = surfacePoint + Vec2d::Up;
            const RegionsManager::RegionDataList* regionList = REGIONS_MANAGER->getRegions(m_actor->getDepth());

            if ( regionList )
            {
                for ( u32 i = 0; i < regionList->size(); i++ )
                {
                    const RegionsManager::RegionData& regionData = (*regionList)[i];

                    if ( regionData.m_id != s_RegionName_Swim )
                    {
                        continue;
                    }

                    PolyLine* poly = AIUtils::getPolyLine(regionData.m_ref);

                    if ( !poly || poly == _poly )
                    {
                        continue;
                    }

                    if ( !poly->isPointInside(checkPos) )
                    {
                        continue;
                    }

                    return;
                }
            }

            m_swimingIsOnJumpRange = btrue;
        }
    }
}

void GroundAIControllerComponent::swimUpdateDive( f32 _dt, PolyLine* _poly, Vec2d& _moveDir, f32& _moveSize, Vec2d& _swimForce, Vec2d& _floatForce )
{
    const PolyLineEdge& surfaceEdge = _poly->getEdgeAt(m_swimingSurfaceEdge);

    Vec2d startPos = m_actor->get2DPos();
    Vec2d currentMoveDir = _moveDir;
    f32 moveSize = _moveSize;

    if ( m_swimingIsOnSurface )
    {
        swimGetFloatForce(surfaceEdge,_floatForce);

        f32 xMove = getPhys()->getCurrentGravityDir().getPerpendicular().dot(currentMoveDir);
        f32 yMove = getPhys()->getCurrentGravityDir().dot(currentMoveDir);

        if ( xMove != 0.f )
        {
            // While in the surface we move in the direction of the polyline to follow its shape
            currentMoveDir = surfaceEdge.m_normalizedVector;
            currentMoveDir *= moveSize;

            if ( xMove < 0.f )
            {
                currentMoveDir *= -1.f;
            }
        }
        else if ( yMove < 0.f )
        {
            moveSize = 0.f;
            currentMoveDir = Vec2d::Zero;
        }
    }

    swimGetSwimForce(_dt,currentMoveDir,moveSize,_swimForce);

    if ( currentMoveDir != Vec2d::Zero )
    {
        f32 targetRotation = getVec2Angle(currentMoveDir);
        setSwimTargetRotation(targetRotation);
    }
    else
    {
        resetSwimTargetRotation();
    }
}

void GroundAIControllerComponent::swimApplyForces( f32 _dt, const Vec2d& _swimForce, const Vec2d& _floatForce )
{
    bbool floating = _floatForce != Vec2d::Zero;
    Vec2d swimForce = _swimForce;

    if ( floating && !m_swimingIsOnSurface )
    {
        Vec2d rightDir = getPhys()->getCurrentGravityDir().getPerpendicular();

        swimForce = rightDir * rightDir.dot(swimForce);
    }

    getPhys()->addForce(swimForce);

    if ( floating )
    {
        getPhys()->addForce(_floatForce);
    }
}

void GroundAIControllerComponent::swimUpdateRotation( f32 _dt )
{
    bbool lookRight = !m_actor->isFlipped();

    // First we transform the angle to "world" space, removing the flipping
    f32 currentAngle = m_actor->getAngle();
    bbool piAdded = bfalse;

    if ( !lookRight )
    {
        currentAngle += MTH_PI;
        piAdded = btrue;
    }

    // Then we calculate the current angular speed
    f32 angleDif = getShortestAngleDelta(currentAngle,m_swimingTargetRotation);
    f32 force = (angleDif*getTemplate()->getSwimingTurnDistMult()) - (m_swimingCurrentAngularSpeed*getTemplate()->getSwimingTurnSpeedMult());

    m_swimingCurrentAngularSpeed += force*_dt;

    currentAngle += m_swimingCurrentAngularSpeed * _dt;

    NormalizeAnglePI(currentAngle);

    if ( piAdded )
    {
        currentAngle -= MTH_PI;
    }

    m_actor->updateComponentPosAngle(m_actor->getPos(),currentAngle,this);
}

void GroundAIControllerComponent::swimGetFloatForce( const PolyLineEdge& _surfaceEdge, Vec2d& _floatForce )
{
    Vec2d surfacePoint = _surfaceEdge.getPos() + (_surfaceEdge.m_vector * m_swimingSurfaceT);

    // Pull down the surface point a bit to make sure we are inside the water
    const Vec2d& currentGravityDir = getPhys()->getCurrentGravityDir();
    surfacePoint += currentGravityDir * 0.1f;

    // Apply the force to be pulled towards the surface
    f32 speedProj = currentGravityDir.dot(getPhys()->getSpeed());
    Vec2d ySpeed = currentGravityDir * speedProj;
    Vec2d dist = surfacePoint - m_actor->get2DPos();

    Vec2d surfaceForce = ( dist * 20.f ) - ( ySpeed * 5.f );

    _floatForce = surfaceForce;

    Vec2d forceDir = surfaceForce;

    forceDir.normalize();

    f32 floatSign = forceDir.dot(currentGravityDir) > 0.f ? 1.f : -1.f;
    _floatForce += currentGravityDir*m_waterResistance*floatSign;
}

void GroundAIControllerComponent::swimGetSwimForce( f32 _dt, const Vec2d& _moveDir, f32 _moveSize, Vec2d& _swimForce )
{
    Vec2d targetMove;
    f32 topSpeed = getTemplate()->getSwimingTopSpeed();

    if ( _moveSize && _moveDir != Vec2d::Zero )
    {
        // Set the swiming force
        targetMove = _moveDir*_moveSize*topSpeed;
        targetMove *= m_currentMoveTargetMultiplier;
    }
    else
    {
        targetMove = Vec2d::Zero;
    }

    Vec2d dif = targetMove - m_swimingInertia;
    f32 difSq = dif.sqrnorm();
    f32 inertiaSpeed = getTemplate()->getSwimingInertiaSpeed() * _dt;

    if ( difSq > (inertiaSpeed*inertiaSpeed) )
    {
        dif.normalize();
        dif *= inertiaSpeed;
    }

    m_swimingInertia += dif;

    dif = m_swimingInertia - getPhys()->getSpeed();
    difSq = dif.sqrnorm();

    if ( difSq > (topSpeed*topSpeed) )
    {
        dif.normalize();
        dif *= topSpeed;
    }

    _swimForce = dif / LOGICDT;
}

void GroundAIControllerComponent::setSwimTargetRotation( f32 _target )
{
    m_swimingTargetRotation = _target;
    NormalizeAnglePI(m_swimingTargetRotation);
}

void GroundAIControllerComponent::resetSwimTargetRotation()
{
    f32 targetRotation = getVec2Angle(getPhys()->getCurrentGravityDir().getPerpendicular());
    f32 anglePi = m_actor->getAngle();
    bbool lookingRight = !m_actor->isFlipped();

    if ( !lookingRight )
    {
        anglePi += MTH_PI;
    }

    NormalizeAnglePI(anglePi);

    anglePi = f32_Abs(anglePi);

    bbool upsideDown = ( anglePi <= (MTH_PIBY2*0.5f) && !lookingRight ) ||
        ( anglePi >= (MTH_PI*0.75f) && lookingRight );

    if ( upsideDown )
    {
        if ( lookingRight )
        {
            targetRotation += MTH_PI;
        }
    }
    else
    {
        if ( !lookingRight )
        {
            targetRotation += MTH_PI;
        }
    }

    m_swimingTargetRotation = targetRotation;
}

void GroundAIControllerComponent::performJump()
{
    Vec2d gravNormal = getPhys()->getCurrentGravityDir().getPerpendicular();
    f32 xSpeed = gravNormal.dot(getPhys()->getSpeed());
    Vec2d newSpeed = gravNormal*xSpeed;

    getPhys()->setSpeed(newSpeed);

    Vec2d force = (-getPhys()->getCurrentGravityDir())*getTemplate()->getJumpForce();

    getPhys()->addForce(force);

    m_jump = bfalse;
}

void GroundAIControllerComponent::clearFrame()
{
    m_moveDir = Vec2d::Zero;
    m_jump = bfalse;
    m_forceJump = bfalse;
    m_nextNavMode = NAVMODE_NONE;
}

void GroundAIControllerComponent::setNavMode()
{
    if ( m_currentNavMode != m_nextNavMode )
    {
        switch(m_currentNavMode)
        {
            case NAVMODE_SWIM:
                {
                    getPhys()->setGravityMultiplier(1.f);
                    getPhys()->setAirFrictionMultiplier(0.f);
                    getPhys()->setUpdateRotation(btrue);
                    getPhys()->setStickingDisabled(bfalse);

                    if ( m_jump )
                    {
                        performJump();
                    }
                }
                break;
            case NAVMODE_CONTROLLEDJUMP:
                {
                    getPhys()->setDisabled(m_disablePhysics);
                }
                break;
            default:
                break;
        }

        m_currentNavMode = m_nextNavMode;

        switch(m_currentNavMode)
        {
            case NAVMODE_SWIM:
                {
                    getPhys()->setGravityMultiplier(0.f);
                    getPhys()->setUpdateRotation(bfalse);
                    getPhys()->setStickingDisabled(btrue);
                    getPhys()->unstick();
                    m_swimingInertia = getPhys()->getSpeed();
                }
                break;
            case NAVMODE_CONTROLLEDJUMP:
                {
                    getPhys()->setSpeed(Vec2d::Zero);
                    getPhys()->setDisabled(btrue);
                }
                break;
            default:
                break;
        }
    }
}

void GroundAIControllerComponent::setAvoidance( bbool _val )
{
    if ( m_mustAvoid != _val )
    {
        m_mustAvoid = _val;

        if ( m_mustAvoid )
        {
            m_avoidanceSpeedFactor = Seeder::getSharedSeeder().GetFloat(getTemplate()->getAvoidanceMinVariation(),getTemplate()->getAvoidanceMaxVariation());
        }
    }
}

void GroundAIControllerComponent::checkAvoidance()
{
    bbool mustAvoid = bfalse;

    if ( m_moveDir != Vec2d::Zero && !m_phys->getSpeed().IsEqual(Vec2d::Zero,MTH_EPSILON) )
    {
        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(m_phantomComponent->getShapePos(), m_phantomComponent->getShapePos(), m_actor->getAngle(),
            m_phantomComponent->getShape(), m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
            contacts);

        const u32 contactCount = contacts.size();
        EventQueryPhysicsData physData;
        Vec2d mySpeed = m_phys->getSpeed();
        Vec2d mySpeedDir = mySpeed;
        Vec2d hisSpeedDir;

        mySpeedDir.normalize();

        f32 myDist = mySpeedDir.dot(m_actor->get2DPos());

        for ( u32 i = 0; i < contactCount; i++ )
        {
            const SCollidableContact& contact = contacts[i];

            if ( contact.m_collidableUserData == m_actor->getRef() )
            {
                continue;
            }

            Actor* actor = AIUtils::getActor(contact.m_collidableUserData);

            if ( !actor )
            {
                continue;
            }

            f32 hisDist = mySpeedDir.dot(actor->get2DPos());

            if ( myDist > hisDist )
            {
                continue;
            }

            if ( f32_Abs(myDist-hisDist) > getTemplate()->getAvoidanceRadius() )
            {
                continue;
            }

            actor->onEvent(&physData);

            if ( physData.getStickyEdgePolyline() == U32_INVALID )
            {
                continue;
            }

            if ( physData.getSpeed().IsEqual(Vec2d::Zero,MTH_EPSILON) )
            {
                continue;
            }

            hisSpeedDir = physData.getSpeed();
            hisSpeedDir.normalize();

            if ( hisSpeedDir.dot(mySpeedDir) <= 0.f )
            {
                continue;
            }

            mustAvoid = btrue;
            break;
        }
    }

    setAvoidance(mustAvoid);
}

void GroundAIControllerComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( EventQueryIsSprinting* isInSprint = _event->DynamicCast<EventQueryIsSprinting>(ITF_GET_STRINGID_CRC(EventQueryIsSprinting,2326390223)))
    {
        isInSprint->setIsSprinting(isSprinting());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(GroundAIControllerComponent_Template)
BEGIN_SERIALIZATION(GroundAIControllerComponent_Template)

    SERIALIZE_MEMBER("walkForce",m_walkForce);
    SERIALIZE_MEMBER("walkEfficiencyMaxSpeed",m_walkEfficiencyMaxSpeed);
    SERIALIZE_MEMBER("walkEfficiencyMinGroundAngle",m_walkMinGroundAngleEfficiencyAngle);
    SERIALIZE_MEMBER("walkEfficiencyMaxGroundAngle",m_walkMaxGroundAngleEfficiencyAngle);
    SERIALIZE_MEMBER("walkEfficiencyMinAngleMultiplier",m_walkMinGroundAngleEfficiencyMult);
    SERIALIZE_MEMBER("walkEfficiencyMaxAngleMultiplier",m_walkMaxGroundAngleEfficiencyMult);
    SERIALIZE_MEMBER("walkMoveTargetMultiplierMin",m_walkMoveTargetMultiplierMin);
    SERIALIZE_MEMBER("walkMoveTargetMultiplierMax",m_walkMoveTargetMultiplierMax);
    SERIALIZE_MEMBER("walkMoveTargetBlendTime",m_walkMoveTargetMultiplierBlendTime);

    SERIALIZE_MEMBER("airForce",m_airForce);
    SERIALIZE_MEMBER("airTopSpeed",m_airTopSpeed);
    SERIALIZE_MEMBER("airFriction",m_airFriction);
    SERIALIZE_MEMBER("airSuspensionMinYSpeed",m_airSuspensionMinYSpeed);
    SERIALIZE_MEMBER("airSuspensionMaxYSpeed",m_airSuspensionMaxYSpeed);
    SERIALIZE_MEMBER("airSuspensionMinMultiplier",m_airSuspensionMinMultiplier);
    SERIALIZE_MEMBER("airSuspensionMaxMultiplier",m_airSuspensionMaxMultiplier);

    SERIALIZE_MEMBER("swimWaterResistMinSpeed",m_swimingWaterResistMinSpeed);
    SERIALIZE_MEMBER("swimWaterResistMaxSpeed",m_swimingWaterResistMaxSpeed);
    SERIALIZE_MEMBER("swimWaterResistMaxMultiplier",m_swimingWaterResistMaxMult);
    SERIALIZE_MEMBER("swimJumpRange",m_swimingJumpRange);
    SERIALIZE_MEMBER("swimInertiaSpeed",m_swimingInertiaSpeed);
    SERIALIZE_MEMBER("swimTopSpeed",m_swimingTopSpeed);
    SERIALIZE_MEMBER("swimTurnDistanceMultiplier",m_swimingTurnDistMult);
    SERIALIZE_MEMBER("swimTurnSpeedMultiplier",m_swimingTurnSpeedMult);

    SERIALIZE_MEMBER("jumpForce",m_jumpForce);

    SERIALIZE_MEMBER("jumpToActorYFuncPoint0Dist",m_jumpToActorYFuncPoint0Dist);
    SERIALIZE_MEMBER("jumpToActorYFuncPoint1Dist",m_jumpToActorYFuncPoint1Dist);
    SERIALIZE_MEMBER("jumpToActorXZFuncPoint0T",m_jumpToActorXZFuncPoint0T);
    SERIALIZE_MEMBER("jumpToActorXZFuncPoint1T",m_jumpToActorXZFuncPoint1T);
    SERIALIZE_MEMBER("jumpToActorSpeed",m_jumpToActorSpeed);
    SERIALIZE_MEMBER("jumpToActorMinTime",m_jumpToActorMinTime);

    SERIALIZE_MEMBER("avoidanceRadius",m_avoidanceRadius);
    SERIALIZE_MEMBER("minVariation",m_avoidanceMinVariation);
    SERIALIZE_MEMBER("maxVariation",m_avoidanceMaxVariation);

    SERIALIZE_MEMBER("sprintWalkForceMultiplier",m_sprintForceMultiplier);
    SERIALIZE_MEMBER("sprintEfficiencyMaxSpeedMultiplier",m_sprintEfficiencyMultiplier);
    SERIALIZE_MEMBER("sprintAccelerationDuration",m_sprintMaxSpeedBlendDuration);

END_SERIALIZATION()

GroundAIControllerComponent_Template::GroundAIControllerComponent_Template()
: m_walkForce(500.f)
, m_walkEfficiencyMaxSpeed(5.43f)
, m_walkEfficiencyMaxSpeedRandomFactor(0.1f)
, m_walkMinGroundAngleEfficiencyAngle(btrue,45.f)
, m_walkMaxGroundAngleEfficiencyAngle(btrue,67.f)
, m_walkMinGroundAngleEfficiencyMult(1.f)
, m_walkMaxGroundAngleEfficiencyMult(0.f)
, m_walkMoveTargetMultiplierBlendTime(0.5f)
, m_walkMoveTargetMultiplierMin(0.f)
, m_walkMoveTargetMultiplierMax(1.f)
, m_airForce(500.f)
, m_airTopSpeed(5.f)
, m_airFriction(2.f)
, m_airSuspensionMinYSpeed(4.f)
, m_airSuspensionMaxYSpeed(10.f)
, m_airSuspensionMinMultiplier(0.f)
, m_airSuspensionMaxMultiplier(0.92f)
, m_swimingWaterResistMinSpeed(4.f)
, m_swimingWaterResistMaxSpeed(10.f)
, m_swimingWaterResistMaxMult(10.f)
, m_swimingJumpRange(2.f)
, m_swimingInertiaSpeed(8.f)
, m_swimingTopSpeed(3.f)
, m_swimingTurnDistMult(50.f)
, m_swimingTurnSpeedMult(10.f)
, m_jumpForce(630.f)
, m_jumpToActorYFuncPoint0Dist(5.f)
, m_jumpToActorYFuncPoint1Dist(5.f)
, m_jumpToActorXZFuncPoint0T(0.3f)
, m_jumpToActorXZFuncPoint1T(0.85f)
, m_jumpToActorSpeed(20.f)
, m_jumpToActorMinTime(1.f)
, m_avoidanceRadius(0.8f)
, m_avoidanceMinVariation(0.8f)
, m_avoidanceMaxVariation(0.9f)
, m_sprintForceMultiplier(1.f)
, m_sprintEfficiencyMultiplier(2.f)
, m_sprintMaxSpeedBlendDuration(1.2f)
{
}

GroundAIControllerComponent_Template::~GroundAIControllerComponent_Template()
{
}
}
