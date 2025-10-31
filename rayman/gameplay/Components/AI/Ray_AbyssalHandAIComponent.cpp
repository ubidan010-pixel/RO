#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ABYSSALHANDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AbyssalHandAIComponent.h"
#endif //_ITF_RAY_ABYSSALHANDAICOMPONENT_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_RAY_ABYSSLIGHTCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AbyssLightComponent.h"
#endif //_ITF_RAY_ABYSSLIGHTCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AbyssalHandAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_AbyssalHandAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_AbyssalHandAIComponent)
    END_VALIDATE_COMPONENT()

    //------------------------------------------------------------------
    Ray_AbyssalHandAIComponent::Ray_AbyssalHandAIComponent()
    : m_animLightComponent(NULL)
    , m_curve()
    , m_curveRenderer()
    , m_initialPosition(Vec2d::Zero)
    , m_handPosition(Vec2d::Zero)
    , m_length(0.0f)
    , m_rootOrientation(Vec2d::Zero)
    , m_handOrientation(Vec2d::Zero)
    , m_handTargetOrientation(Vec2d::Zero)
    , m_handCurrentSpeed(0.0f)
    , m_handTargetSpeed(0.0f)
    , m_closestPlayer(NULL)
    , m_closestPlayerPosition(Vec2d::Zero)
    , m_closestPlayerDirection(Vec2d::Zero)
    , m_closestPlayerDistance(0.0f)
    , m_timeSinceLastStim(0.0f)
    , m_handState(AbyssalHandState_UNDEFINED)
    , m_handStateTimer(0.0f)
    , m_detectionShape()
    {
    }

    //------------------------------------------------------------------
    Ray_AbyssalHandAIComponent::~Ray_AbyssalHandAIComponent()
    {
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //get components
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();

        //load renderer
        m_curveRenderer.onLoaded( &getTemplate()->getBezierRenderer(), m_actor->getResourceGroup() );

        //init data
        initDataFromPosition();
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        //init state
        changeState(AbyssalHandState_IDLE);
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_handPosition = m_actor->get2DPos();
        m_length = (m_handPosition - m_initialPosition).norm();

        updateHandSpeed(_dt);
        updatePlayerDetection();
        updateOrientation(_dt);
        updateState(_dt);
        updateCurve();

        m_timeSinceLastStim += _dt;
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::Draw()
    {
        Super::Draw();

        if( m_length > 0.0f )
        {
            m_curve.buildEdges(1);
            if( m_curve.getTotalLength() > 0.0f )
            {
                m_curveRenderer.setDepth(m_actor->getDepth());
                m_curveRenderer.draw(m_curve);
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags ) const
    {
        Super::drawEdit(drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::shape(Vec2d::Zero, m_actor->getDepth(), 0.0f, &m_detectionShape, Color::red());
    }

    
    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        initDataFromPosition();
    }
#endif //ITF_SUPPORT_EDITOR

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::initDataFromPosition()
    {
        //get initial position
        m_initialPosition = m_actor->get2DPos();
        //get initial orientation
        m_rootOrientation = Vec2d::Right.Rotate(m_actor->getAngle() + MTH_PIBY2);
        m_handOrientation = m_rootOrientation;
        m_handTargetOrientation = m_rootOrientation;

        //reinit length
        m_length = 0.0f;

        //init curve
        initCurve();

        //init detection AABB
        PhysShapePolygon::CopyShape(&getTemplate()->getDetectionShape(), &m_detectionShape);
        m_detectionShape.Rotate(m_actor->getAngle());
        m_detectionShape.Translate(m_initialPosition);
    }


    //------------------------------------------------------------------
    // Player detection
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updatePlayerDetection()
    {
        //get closest player
        m_closestPlayer = AIUtils::getClosestLivePlayer(m_initialPosition, m_actor->getDepth(), &m_detectionShape);
        if( m_closestPlayer )
        {
            m_closestPlayerPosition = m_closestPlayer->get2DPos();
            const Vec2d difference = m_closestPlayerPosition - m_handPosition;
            m_closestPlayerDistance = difference.norm();
            if( m_closestPlayerDistance > 0.0f )
            {
                m_closestPlayerDirection = difference / m_closestPlayerDistance;
            }
        }
    }

    //------------------------------------------------------------------
    // Orientation & position management
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateOrientation(const f32 _dt)
    {
        //compute target orientation
        if( m_closestPlayer && m_handState != AbyssalHandState_FLEE )
        {
            m_handTargetOrientation = m_closestPlayerDirection;
        }
        else
        {
            m_handTargetOrientation = m_rootOrientation;
        }

        //compute orientation
        const f32 rotationSpeed = getTemplate()->getHandRotationSpeed().ToRadians();
        const Angle currentAngle(bfalse, m_handOrientation.getAngle());
        const Angle targetAngle(bfalse, m_handTargetOrientation.getAngle());
        if( currentAngle != targetAngle )
        {
            const Angle diff = Angle::GetShortestDif(currentAngle, targetAngle);
            const f32 radDiff = diff.ToRadians();
            const f32 radAbsDiff = f32_Abs(radDiff);
            const f32 allowedRotation = _dt * rotationSpeed;
            f32 newAngle = currentAngle.ToRadians();
            if( radAbsDiff > allowedRotation )
            {
                const f32 angleMove = (radDiff / radAbsDiff) * allowedRotation;
                newAngle += angleMove;
            }
            else
            {
                newAngle += radDiff;
            }
            m_handOrientation = Vec2d::Right.Rotate( newAngle );
            m_actor->setAngle(newAngle - MTH_PIBY2);
        }
        //DebugDraw::line2D(m_handPosition, m_handPosition + m_handOrientation, m_actor->getDepth());
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::setHandPosition(const Vec2d & _position)
    {
        m_handPosition = _position;
        m_actor->set2DPos( m_handPosition );        
        m_length = (m_handPosition - m_initialPosition).norm();
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateHandSpeed(const f32 _dt)
    {
        const f32 speedDiff = m_handTargetSpeed - m_handCurrentSpeed;
        if( speedDiff != 0.0f )
        {
            const f32 allowedSpeedChange = getTemplate()->getHandAcceleration() * _dt;
            const f32 speedAbsDiff = f32_Abs(speedDiff);
            f32 speedChange = 0.0f;
            if( allowedSpeedChange < speedAbsDiff )
            {
                speedChange = allowedSpeedChange * (speedDiff / speedAbsDiff);
            }
            else
            {
                speedChange = speedDiff;
            }
            m_handCurrentSpeed += speedChange;
        }
    }

    //------------------------------------------------------------------
    // Light detection
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    bbool Ray_AbyssalHandAIComponent::collideWithLight(const bbool _far) const
    {
        if( m_length > 0.0f )
        {
            //check light along curve
            if( m_curve.getNumEdges() > 0 )
            {
                const f32 curveLength = m_curve.getTotalLength();
                const f32 tStep = 1.0f / curveLength;
                const u32 nbStep = (u32)curveLength;
                for(u32 step = 0; step < nbStep; ++step)
                {
                    const f32 t = step * tStep;
                    const Vec2d segmentStart = m_curve.getPosAtT(t, 0).truncateTo2D();
                    const Vec2d segmentStop = m_curve.getPosAtT(t + tStep, 0).truncateTo2D();
                    //DebugDraw::line2D(segmentStart, segmentStop, m_actor->getDepth(), Color::red());
                    if( Ray_AbyssLightComponent::segmentIntersectWithLight(segmentStart, segmentStop) )
                    {
                        return btrue;
                    }
                }
            }
        }

        //check light on hand
        f32 length = getTemplate()->getHandLightDistance();
        if( !_far )
        {
            length *= 0.25f;
        }
        //DebugDraw::line2D(m_handPosition, m_handPosition + (m_handOrientation * length), m_actor->getDepth(), Color::red());
        return Ray_AbyssLightComponent::segmentIntersectWithLight(m_handPosition, m_handOrientation, length);
    }

    //------------------------------------------------------------------
    bbool Ray_AbyssalHandAIComponent::checkLightPresence(const bbool _far) const
    {
        f32 distance = getTemplate()->getHandLightDistance();
        if( _far )
        {
            distance *= 2.0f;
        }
        return Ray_AbyssLightComponent::segmentIntersectWithLight(m_handPosition, m_closestPlayerDirection, distance);
    }

    //------------------------------------------------------------------
    // State management
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::changeState(const AbyssalHandState _state)
    {
        m_handState = _state;

        
        switch(m_handState)
        {
        case AbyssalHandState_IDLE:
            startIdle();
            break;            
        case AbyssalHandState_FORWARD:
            startForward();
            break;
        case AbyssalHandState_BACKWARD:
            startBackward();
            break;
        case AbyssalHandState_FLEE:
            startFlee();
            break;
        case AbyssalHandState_ATTACK:
            startAttack();
            break;
        default:
            break;
        }

        m_handStateTimer = 0.0f;
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateState(const f32 _dt)
    {
        switch(m_handState)
        {
        case AbyssalHandState_IDLE:
            updateIdle();
            break;
        case AbyssalHandState_FORWARD:
            updateForward(_dt);
            break;
        case AbyssalHandState_BACKWARD:
            updateBackward(_dt);
            break;
        case AbyssalHandState_FLEE:
            updateFlee(_dt);
            break;        
        case AbyssalHandState_ATTACK:
            updateAttack();
            break;
        default:
            break;
        }

        m_handStateTimer += _dt;
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::startIdle()
    {
        if( m_animLightComponent )
        {
            m_animLightComponent->setAnim("STAND");
        }
        m_handTargetSpeed = 0.0f;
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateIdle()
    {
        if( m_closestPlayer != NULL )
        {
            if( !checkLightPresence(btrue) )
            {
                changeState(AbyssalHandState_FORWARD);
            }
            else if( collideWithLight(bfalse) )
            {
                changeState(AbyssalHandState_FLEE);
            }
            else if( m_closestPlayerDistance < getTemplate()->getStimDistance() && m_timeSinceLastStim > getTemplate()->getStimDelay() )
            {
                changeState(AbyssalHandState_ATTACK);
            }
        }
        else if( m_length > 0.0f && m_handStateTimer > getTemplate()->getIdleMinTime() )
        {
            changeState(AbyssalHandState_BACKWARD);
        }
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::startForward()
    {
        m_handTargetSpeed = getTemplate()->getHandForwardSpeed();
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateForward(const f32 _dt)
    {
        if( m_closestPlayer != NULL )
        {
            if( m_closestPlayerDistance > getTemplate()->getStimDistance() )
            {
                if( !checkLightPresence(bfalse) )
                {
                    const Vec2d move = m_closestPlayerDirection * (m_handCurrentSpeed * _dt);
                    setHandPosition(m_handPosition + move);
                }
                else
                {
                    changeState(AbyssalHandState_IDLE);
                }
            }
            else if( m_timeSinceLastStim > getTemplate()->getStimDelay() )
            {
                    changeState(AbyssalHandState_ATTACK);
            }
        }
        else
        {
            changeState(AbyssalHandState_IDLE);
        }
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::startBackward()
    {
        m_handTargetSpeed = getTemplate()->getHandBackwardSpeed();
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateBackward(const f32 _dt)
    {
        if( m_length > 0.0f && m_closestPlayer == NULL && !collideWithLight(bfalse) )
        {
            const Vec2d difference = m_initialPosition - m_handPosition;
            //as length is > 0.0f difference should not be equal to 0.0f
            ITF_ASSERT( difference.sqrnorm() > 0.0f );
            const f32 distance = difference.norm();
            const f32 allowedMove = f32_Min(m_handCurrentSpeed * _dt, distance);
            const Vec2d move = difference * (allowedMove / distance);
            setHandPosition(m_handPosition + move);
        }
        else
        {
            changeState(AbyssalHandState_IDLE);
        }
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::startFlee()
    {
        if( m_animLightComponent )
        {
            m_animLightComponent->setAnim("CATCH");
        }
        m_handTargetSpeed = getTemplate()->getHandEscapeSpeed();
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateFlee(const f32 _dt)
    {
        if( collideWithLight(btrue) )
        {
            if(m_length > 0.0f)
            {
                const Vec2d difference = m_initialPosition - m_handPosition;
                const f32 allowedMove = f32_Min(m_handCurrentSpeed * _dt, m_length);
                const Vec2d move = difference * (allowedMove / m_length);
                setHandPosition(m_handPosition + move);
            }
        }
        else
        {
            changeState(AbyssalHandState_IDLE);
        }
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::startAttack()
    {
        if( m_animLightComponent )
        {
            m_animLightComponent->setAnim("ATTACK");
        }
        sendStim();
        m_handTargetSpeed = 0.0f;
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateAttack()
    {
        if( m_animLightComponent )
        {
            if( m_animLightComponent->isSubAnimFinished() )
            {
                changeState(AbyssalHandState_IDLE);
            }
        }
        else
        {
            changeState(AbyssalHandState_IDLE);
        }
    }

    //------------------------------------------------------------------
    // Curve management
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::initCurve()
    {
        m_curve.clear();
        const Vec3d initialPosition = m_initialPosition.to3d(m_actor->getDepth());
        //const Vec3d rootTangent = m_rootOrientation.to3d();
        //const Vec3d handTangent = m_handOrientation.to3d();
        m_curve.addPoint(initialPosition, initialPosition, initialPosition);// - handTangent, initialPosition + handTangent);
        m_curve.addPoint(initialPosition, initialPosition, initialPosition);// - rootTangent, initialPosition + rootTangent);
    }

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::updateCurve()
    {
        Vec3d curveEnd = m_handPosition.to3d(m_actor->getDepth());
        if( m_animLightComponent )
        {
            const u32 boneIndex = m_animLightComponent->getBoneIndex(getTemplate()->getEndCurveBoneName());
            if( boneIndex != U32_INVALID )
            {
                m_animLightComponent->getBonePos(boneIndex, curveEnd);
            }
        }
        
        //compute tangents factor according to hand distance from root
        m_curve.getPointAt(0).m_pos = curveEnd;
        const f32 maxFactorTangentLength = getTemplate()->getTangentMaxFactorLength();
        const f32 nullFactorTangentLength = getTemplate()->getTangentNullFactorLength();
        const f32 length = f32_Clamp(m_length, nullFactorTangentLength, maxFactorTangentLength);
        const f32 ratio = (length - nullFactorTangentLength ) / (maxFactorTangentLength - nullFactorTangentLength);

        //update head point
        const Vec3d handTangent = m_handOrientation.to3d() * (ratio * getTemplate()->getHandTangentFactor());
        m_curve.getPointAt(0).m_tanA = curveEnd + handTangent;
        m_curve.getPointAt(0).m_tanB = curveEnd - handTangent;

        //update root point
        const Vec3d rootTangent = m_rootOrientation.to3d() * (ratio * getTemplate()->getRootTangentFactor());
        const Vec3d initialPosition = m_initialPosition.to3d();
        m_curve.getPointAt(1).m_tanA = initialPosition + rootTangent;
        m_curve.getPointAt(1).m_tanB = initialPosition - rootTangent;

    }

    //------------------------------------------------------------------
    // Player stim
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_AbyssalHandAIComponent::sendStim()
    {
        if( m_closestPlayer )
        {
            const Faction faction = RAY_FACTION_ENEMY;

            PunchStim stim;
            stim.setSender(m_actor->getRef());
            stim.setDepth(m_actor->getDepth());
            stim.setPos(m_handPosition);
            stim.setPrevPos(m_handPosition);
            stim.setDirection(m_handOrientation);
            stim.setLevel(1);
            stim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
            stim.setAngle(m_handOrientation.getAngleOpt());
            stim.setFaction(faction);
            m_closestPlayer->onEvent(&stim);

            m_timeSinceLastStim = 0.0f;
        }
    }



    //------------------------------------------------------------------
    // Template
    //------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_AbyssalHandAIComponent_Template)
    
    BEGIN_SERIALIZATION(Ray_AbyssalHandAIComponent_Template)
        SERIALIZE_MEMBER("handForwardSpeed", m_handForwardSpeed);
        SERIALIZE_MEMBER("handBackwardSpeed", m_handBackwardSpeed);
        SERIALIZE_MEMBER("handEscapeSpeed", m_handEscapeSpeed);
        SERIALIZE_MEMBER("handAcceleration", m_handAcceleration);
        SERIALIZE_OBJECT("bezierRenderer", m_bezierRenderer);
        SERIALIZE_OBJECT("detectionShape", m_detectionShape);
        SERIALIZE_MEMBER("handLightDistance", m_handLightDistance);
        SERIALIZE_MEMBER("handRotationSpeed", m_handRotationSpeed);
        SERIALIZE_MEMBER("endCurveBoneName", m_endCurveBoneName);
        SERIALIZE_MEMBER("stimDelay", m_stimDelay);
        SERIALIZE_MEMBER("stimDistance", m_stimDistance);
        SERIALIZE_MEMBER("idleMinTime", m_idleMinTime);
        SERIALIZE_MEMBER("rootTangentFactor", m_rootTangentFactor);
        SERIALIZE_MEMBER("handTangentFactor", m_handTangentFactor);
        SERIALIZE_MEMBER("tangentMaxFactorLength", m_tangentMaxFactorLength);
        SERIALIZE_MEMBER("tangentNullFactorLength", m_tangentNullFactorLength);
    END_SERIALIZATION()

    //------------------------------------------------------------------
    Ray_AbyssalHandAIComponent_Template::Ray_AbyssalHandAIComponent_Template()
    : m_handForwardSpeed(5.0f)
    , m_handBackwardSpeed(5.0f)
    , m_handEscapeSpeed(10.0f)
    , m_handAcceleration(2.0f)
    , m_detectionShape()
    , m_handLightDistance(1.5f)
    , m_handRotationSpeed(bfalse, MTH_PI)
    , m_endCurveBoneName(StringID::Invalid)
    , m_stimDelay(1.0f)
    , m_stimDistance(0.5f)
    , m_idleMinTime(1.5f)
    , m_rootTangentFactor(1.0f)
    , m_handTangentFactor(2.0f)
    , m_tangentMaxFactorLength(3.0f)
    , m_tangentNullFactorLength(1.0f)
    {
    }

    //------------------------------------------------------------------
    Ray_AbyssalHandAIComponent_Template::~Ray_AbyssalHandAIComponent_Template()
    {
    }

} // namespace ITF

