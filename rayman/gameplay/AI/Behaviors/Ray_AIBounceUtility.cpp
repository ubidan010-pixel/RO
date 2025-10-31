#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBOUNCEUTILITY_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIBounceUtility.h"
#endif

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBounceUtility::bounceUtility_registerEvents(Actor *_actor, AIComponent *_aiComponent)
    {
        ACTOR_REGISTER_EVENT_COMPONENT(_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),_aiComponent);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBounceUtility::bounceUtility_reset()
    {
        m_bouncingOnWall = bfalse;
        m_bouncingOnWall_isFromCharacter = bfalse;
        m_bouncingOnWall_wallNormal.set(1,0);
        m_bouncingOnWall_NextSpeed.set(0,0);
        m_lastBlockingEdge.set(0,0);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Vec2d Ray_AIBounceUtility::bounceUtility_getForceToReach(f32 _mass, const Vec2d &_speed, const Vec2d &_currentPos, const Vec2d &_goal, const f32 _forceDuration, f32 _maxForce)
    {
        ITF_ASSERT_CRASH(_forceDuration>=LOGICDT&& _maxForce>0, "incorrect parameters for getForceToReach");

        //Simple formula for reaching a position with a constant force for a given duration

        f32 dt = LOGICDT;
        f32 frameCount = _forceDuration/dt;
        f32 sum_dt2 = frameCount * (frameCount - 1) * 0.5f * (dt*dt);
        Vec2d acceleration = (_goal-_currentPos-(frameCount*dt)*_speed)*(1.f/sum_dt2);

        Vec2d force = _mass*acceleration;

        f32 forceNorm = force.norm();
        if (forceNorm>_maxForce)
        {
            force *= _maxForce/forceNorm;
        }

        return force;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Vec2d Ray_AIBounceUtility::bounceUtility_getBounceOnlyForce(const Vec2d& _edgeDir, f32 _factor ) const
    {
        Vec2d ret = _edgeDir.getPerpendicular();

        ITF_ASSERT_MSG(f32_Abs(_edgeDir.sqrnorm()-1)<0.001f, "edgedir must be normal");
        if (ret.m_y<0)
            ret = -ret;

        f32 angle = atan2f(ret.m_x, ret.m_y);

        if (f32_Abs(angle)<m_smallAngleForBounceOnlyForce)
        {
            angle = 0.f;
        }
        if (angle>m_maxBounceOnlyForce_Angle)
            angle = m_maxBounceOnlyForce_Angle;
        else if (angle<-m_maxBounceOnlyForce_Angle)
            angle = -m_maxBounceOnlyForce_Angle;

        Vec2d::SinCos(&ret, angle);

        ITF_ASSERT(m_bounceForceAmplitude>0.f);

        ret *= ( m_bounceForceAmplitude * _factor );

        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBounceUtility::bounceUtility_EnterJumpOrFall(const StringID &_jumpingAnim, StickToPolylinePhysComponent *_physComponent, AnimLightComponent *_animComponent)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBounceUtility::bounceUtility_UpdateJumpOrFall(StickToPolylinePhysComponent *_physComponent,
        const StringID &_jumpAnim,
        const StringID &_fallAnim,
        f32 _speedThatTriggersFloating,
        const Vec2d &_lastSpeed)
    {
        bbool justWallBounced = bounceUtility_updateBounceOnWall(_physComponent);

        const Vec2d &speed = _physComponent->getSpeed();

        if (speed.m_y!=0)
        {
            StringID newAnim = (speed.m_y>0)?_jumpAnim:_fallAnim;
            if (getCurrentAnim()!=newAnim)
            {
                setCurrentAnim(newAnim);
            }
        }

        Vec2d edgeDir;
        bbool isFromActor = bfalse;
        if (bounceUtility_isOnGroundEdge(_physComponent, edgeDir, isFromActor))
        {
            bounceUtility_UpdateJumpOrFall_onIsOnGround(edgeDir, isFromActor);
        }
        else
        {
            if (justWallBounced && m_hasStateForWallBounce)
            {
                //do nothing
            }
            else if (speed.m_y<=_speedThatTriggersFloating && _lastSpeed.m_y >_speedThatTriggersFloating)
            {
                bounceUtility_UpdateJumpOrFall_onNearTopOfJump();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Vec2d Ray_AIBounceUtility::bounceUtility_mayBlendEdgeDirWhenGround(StickToPolylinePhysComponent *_physComponent, const Vec2d &_normalizedEdgeDir)
    {
        Vec2d normalizedEdgeDir = _normalizedEdgeDir;
        if (_physComponent->getEdgeOrientationType(_normalizedEdgeDir)==StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND)
        {
            if (m_groundToVerticalBlendFactor>0)
            {
                normalizedEdgeDir = _normalizedEdgeDir*(1-m_groundToVerticalBlendFactor)+Vec2d::Right*m_groundToVerticalBlendFactor;
                normalizedEdgeDir.normalize();
            }
        }
        return normalizedEdgeDir;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBounceUtility::bounceUtility_bounceOnEdge_applyBouncePhysics(StickToPolylinePhysComponent *_physComponent,
        const Vec2d &_normalizedEdgeDir)
    {
        Vec2d normalizedEdgeDir = bounceUtility_mayBlendEdgeDirWhenGround(_physComponent, _normalizedEdgeDir);

        Vec2d speed = _physComponent->getSpeed();
        Vec2d force = bounceUtility_getBounceForce(normalizedEdgeDir);
        _physComponent->addForce(force);

        Vec2d normal = normalizedEdgeDir.getPerpendicular();
        speed = normalizedEdgeDir*speed.dot(normalizedEdgeDir)*m_bounceUtility_groundBounceSpeedFactor.m_x+
            normal*speed.dot(normal)*m_bounceUtility_groundBounceSpeedFactor.m_y;

        //speed.m_x *= m_bounceUtility_groundBounceSpeedFactor.m_x;
        //speed.m_y *= m_bounceUtility_groundBounceSpeedFactor.m_y;
        _physComponent->setSpeed(speed);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBounceUtility::bounceUtility_bounceOnEdge(const StringID &animationID,
        StickToPolylinePhysComponent *physComponent,
        AnimLightComponent *animComponent,
        const Vec2d &_normalizedEdgeDir)
    {
        bounceUtility_bounceOnEdge_applyBouncePhysics(physComponent, _normalizedEdgeDir);

        animComponent->resetCurTime();
        setCurrentAnim(animationID);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBounceUtility::bounceUtility_processEvent(Event *_event, StickToPolylinePhysComponent *_physComponent)
    {
        if ( EventBlockedByPolyline * blockedOnPolyEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
        {
            BaseObject* obj = GETOBJECT(blockedOnPolyEvent->getPolyline());
            if ( obj )
            {
                const PolyLine* poly = static_cast<PolyLine*>(obj);
                const PolyLineEdge* edge = &poly->getEdgeAt(blockedOnPolyEvent->getEdge());
                bbool isFromActor = poly->getOwnerActor()!=NULL;

                Vec2d realNormal = edge->m_normalizedVector.getPerpendicular();
                if (blockedOnPolyEvent->getSpeed().dot(realNormal)>0)
                    return;

                if (_physComponent->getEdgeOrientationType(edge->m_normalizedVector)==StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL)
                {

                    Vec2d normal;
                    if (realNormal.m_x>0)
                        normal.set(1,0);
                    else
                        normal.set(-1,0);

                    Vec2d newSpeed = blockedOnPolyEvent->getSpeed();
                    f32 incidentSpeed = newSpeed.dot(normal);
                    if (incidentSpeed>0.f)
                    {
                        incidentSpeed = 0.f;
                    }
                    else
                    {
                        newSpeed -= 2*normal*incidentSpeed;
                    }

                    _physComponent->unstick();
                    _physComponent->resetTorque();

                    if (incidentSpeed!=0.f)
                    {
                        f32 minSpeed = 0.05f;
                        if ((newSpeed.m_x!=0) && (f32_Abs(newSpeed.m_x)<minSpeed))
                        {
                            if (newSpeed.m_x<0)
                                newSpeed.m_x = -minSpeed;
                            else
                                newSpeed.m_x = minSpeed;
                        }
                        m_bouncingOnWall_wallNormal = normal;
                        m_bouncingOnWall_isFromCharacter = isFromActor;
                        m_bouncingOnWall = btrue;
                        m_bouncingOnWall_NextSpeed = newSpeed;
                    }

					if (m_bounceOnWall_onlyKeepNormalPartOfSpeed)
					{
						m_bouncingOnWall_NextSpeed = normal*(normal.dot(m_bouncingOnWall_NextSpeed));
					}

                }
                m_lastBlockingEdge = edge->m_normalizedVector;
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIBounceUtility::bounceUtility_updateBounceOnWall(StickToPolylinePhysComponent *_physComponent)
    {
        bbool justBounced = bfalse;
        if (m_bouncingOnWall)
        {
            _physComponent->setSpeed(m_bouncingOnWall_NextSpeed) ;
            bounceUtility_UpdateBounceOnWall_onBouncingOnWall(m_bouncingOnWall_wallNormal, m_bouncingOnWall_NextSpeed);
            m_bouncingOnWall = bfalse;
            justBounced = btrue;
        }
        return justBounced;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIBounceUtility::bounceUtility_isOnGroundEdge(StickToPolylinePhysComponent *_physComponent, Vec2d &_outEdge, bbool &_isFromActor)
    {
        _outEdge.set(0,0);
        const PolyLineEdge * edge = _physComponent->getStickedEdge();
        _isFromActor = bfalse;
        if (edge /*&& _physComponent->getEdgeOrientationType(edge->m_normalizedVector)==StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND*/)
        {
            _isFromActor = _physComponent->getStickedPolyline()->getOwnerActor()!=NULL;
            _outEdge = edge->m_normalizedVector;
            return btrue;
        }
        else
        {
            f32 edgeSqrNorm = m_lastBlockingEdge.sqrnorm();
            ITF_ASSERT(edgeSqrNorm==0.f || f32_Abs(edgeSqrNorm-1)<0.01f);
            if (edgeSqrNorm!=0.f && _physComponent->getEdgeOrientationType(m_lastBlockingEdge)==StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND)
            {
                _outEdge = m_lastBlockingEdge;
                return btrue;
            }
        }
        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBounceUtility::bounceUtility_bounceIfOnGroundEdge(const StringID &jumpingAnim,
        StickToPolylinePhysComponent *_physComponent, 
        AnimLightComponent *_animComponent)
    {
        Vec2d edgeDir;
        bbool isFromActor = bfalse;
        if (bounceUtility_isOnGroundEdge(_physComponent, edgeDir, isFromActor))
        {
            bounceUtility_bounceOnEdge(jumpingAnim, _physComponent, _animComponent, edgeDir);
        }
    }

}
