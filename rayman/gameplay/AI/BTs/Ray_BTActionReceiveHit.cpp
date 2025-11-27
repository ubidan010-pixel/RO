
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONRECEIVEHIT_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionReceiveHit.h"
#endif // _ITF_RAY_BTACTIONRECEIVEHIT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionReceiveHit);

Ray_BTActionReceiveHit::Ray_BTActionReceiveHit()
: m_animComponent(NULL)
, m_fxController(NULL)
, m_aiController(NULL)
, m_physComponent(NULL)
, m_instigator(ITF_INVALID_OBJREF)
, m_hitLevel(0)
, m_hitFxPos(Vec3d::Zero)
, m_reactType(0)
, m_hitDir(Vec2d::Right)
, m_totalTime(0.f)
, m_timeCounter(0.f)
, m_blockedPolyline(ITF_INVALID_OBJREF)
, m_blockedEdgeIndex(U32_INVALID)
, m_crashedDist(0.f)
, m_crashedVertical(bfalse)
, m_currentSpeed(Vec2d::Zero)
, m_wasBlocked(bfalse)
, m_appexStart(bfalse)
, m_appexStop(bfalse)
, m_appex(bfalse)
, m_crashed(bfalse)
{
}

Ray_BTActionReceiveHit::~Ray_BTActionReceiveHit()
{
}

void Ray_BTActionReceiveHit::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_animComponent = _tree->getActor()->GetComponent<AnimatedComponent>();
    m_fxController = _tree->getActor()->GetComponent<FXControllerComponent>();
    m_aiController = _tree->getActor()->GetComponent<GroundAIControllerComponent>();
    m_physComponent = _tree->getActor()->GetComponent<StickToPolylinePhysComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_tree);
    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768), m_tree);
}

bbool Ray_BTActionReceiveHit::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"AnimatedComponent",m_animComponent!=NULL,"%s: anim component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_fxController )
    {
        ITF_WARNING_PARAM(_actor,"FXControllerComponent",m_fxController!=NULL,"%s: fxcontroller component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getAnim().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"anim",getTemplate()->getAnim().isValid(),"%s: there is no anim set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTActionReceiveHit::decide( f32 _dt )
{
    return m_tree->getBlackBoard().factExists(Ray_AIFact_hitReceived);
}

void Ray_BTActionReceiveHit::onActivate()
{
    Super::onActivate();

    setupHit(bfalse);
    m_animComponent->setAnim(getTemplate()->getAnim());
}

void Ray_BTActionReceiveHit::onDeactivate()
{
    Super::onDeactivate();

    m_blockedPolyline = ITF_INVALID_OBJREF;
    m_blockedEdgeIndex = U32_INVALID;
    m_physComponent->setFrictionMultiplier(1.f);
    m_physComponent->setGravityMultiplier(1.f);
    m_physComponent->setDisabled(bfalse);
    m_animComponent->resetLockMagicBox();

    m_tree->getBlackBoard().removeFact(Ray_AIFact_hitReceived);
    m_tree->getBlackBoard().removeFact(Ray_AIFact_hitReceivedChanged);
    m_tree->getBlackBoard().removeFact(Ray_AIFact_receivedHitInstigator);
    m_tree->getBlackBoard().removeFact(Ray_AIFact_receivedHitLevel);
    m_tree->getBlackBoard().removeFact(Ray_AIFact_receivedHitFxPos);
    m_tree->getBlackBoard().removeFact(Ray_AIFact_receivedHitType);
    m_tree->getBlackBoard().removeFact(Ray_AIFact_receivedHitDir);
}

bbool Ray_BTActionReceiveHit::update( f32 _dt )
{
    Super::update(_dt);

    if ( m_tree->getBlackBoard().factExists(Ray_AIFact_hitReceivedChanged) )
    {
        setupHit(btrue);
    }

    if ( !m_crashed )
    {
        switch(m_reactType)
        {
        case RECEIVEDHITTYPE_EJECTY:
        case RECEIVEDHITTYPE_UPPUNCH:
            updatePunchUp(_dt);
            break;
        case RECEIVEDHITTYPE_HURTBOUNCE:
            updateHurtBounce(_dt);
            break;
        case RECEIVEDHITTYPE_EARTHQUAKE:
            updateEarthquake(_dt);
            break;
        default:
            updatePunchFront(_dt);
            break;
        }
    }
    else
    {
        updateCrashed(_dt);
    }

    m_currentSpeed = m_physComponent->getSpeed();

    if ( m_wasBlocked )
    {
        m_wasBlocked = bfalse;

        if ( m_reactType == RECEIVEDHITTYPE_UPPUNCH ||
             m_reactType == RECEIVEDHITTYPE_FRONTPUNCH ||
             m_reactType == RECEIVEDHITTYPE_CHARGE_PUNCH )
        {
            setCrashAnim();
        }
    }

    return !checkFinished();
}

void Ray_BTActionReceiveHit::updatePunchFront( f32 _dt )
{
    m_timeCounter = Max(m_timeCounter-_dt,0.f);
}

void Ray_BTActionReceiveHit::updatePunchUp( f32 _dt )
{
    if ( m_appexStart )
    {
        m_physComponent->setGravityMultiplier(0.f);
        m_appexStart = bfalse;
        m_appex = btrue;
    }
    else if ( m_appexStop )
    {
        m_physComponent->setGravityMultiplier(1.f);
        m_appexStop = bfalse;
        m_appex = bfalse;
    }
}

void Ray_BTActionReceiveHit::updateHurtBounce( f32 _dt )
{
    m_timeCounter = Max(m_timeCounter-_dt,0.f);
}

void Ray_BTActionReceiveHit::updateEarthquake( f32 _dt )
{

}

void Ray_BTActionReceiveHit::updateCrashed( f32 _dt )
{
    const BaseObject* obj = GETOBJECT(m_blockedPolyline);

    if ( obj )
    {
        const PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly )
        {
            const PolyLineEdge& edge = poly->getEdgeAt(m_blockedEdgeIndex);
            Vec2d pos = edge.getPos() + ( edge.m_normalizedVector * m_crashedDist );

            m_animComponent->lockMagicBox(pos.to3d(m_tree->getActor()->getPos().m_z));
        }
    }
}

void Ray_BTActionReceiveHit::setupHit( bbool _restart )
{
    m_wasBlocked = bfalse;
    m_appex = bfalse;
    m_crashed = bfalse;
    m_instigator = ITF_INVALID_OBJREF;
    m_hitLevel = 0;
    m_hitFxPos = Vec3d::Zero;
    m_reactType = RECEIVEDHITTYPE_FRONTPUNCH;
    m_hitDir = Vec2d::Right;

    m_animComponent->resetLockMagicBox();

    m_tree->getBlackBoard().getFact(Ray_AIFact_receivedHitInstigator,m_instigator);
    m_tree->getBlackBoard().getFact(Ray_AIFact_receivedHitLevel,m_hitLevel);
    m_tree->getBlackBoard().getFact(Ray_AIFact_receivedHitFxPos,m_hitFxPos);
    m_tree->getBlackBoard().getFact(Ray_AIFact_receivedHitType,m_reactType);
    m_tree->getBlackBoard().getFact(Ray_AIFact_receivedHitDir,m_hitDir);
    m_tree->getBlackBoard().getFact(Ray_AIFact_receivedHitFeedbackAction,m_feedBackAction);

    u32 fxHandle = m_fxController->playFeedback(m_instigator, m_feedBackAction, m_tree->getActor()->getRef());
    m_fxController->setFXPosFromHandle(fxHandle, m_hitFxPos);

    m_appexStart = bfalse;
    m_appexStop = bfalse;

    switch(m_reactType)
    {
    case RECEIVEDHITTYPE_UPPUNCH:
    case RECEIVEDHITTYPE_EJECTY:
        setupPunchUp(_restart);
        break;
    case RECEIVEDHITTYPE_HURTBOUNCE:
        setupHurtBounce(_restart);
        break;
    case RECEIVEDHITTYPE_EARTHQUAKE:
        setupEarthquake(_restart);
        break;
    default:
        setupPunchFront(_restart);
        break;
    }

    m_tree->getBlackBoard().removeFact(Ray_AIFact_hitReceivedChanged);
}

void Ray_BTActionReceiveHit::setCrashAnim()
{
    const BaseObject* obj = GETOBJECT(m_blockedPolyline);

    if ( obj )
    {
        const PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly )
        {
            Actor* myActor = m_tree->getActor();
            const PolyLineEdge& edge = poly->getEdgeAt(m_blockedEdgeIndex);
            Vec2d dirToActor = myActor->get2DPos() - edge.getPos();

            m_crashedDist = edge.m_normalizedVector.dot(dirToActor);

            Vec2d normal = edge.m_normalizedVector.getPerpendicular();
            f32 dotProd = normal.dot(-Vec2d::Up);
            f32 targetAngle;

            if ( dotProd >= 0.95f )
            {
                // crash up
                m_crashedVertical = btrue;
                targetAngle = getVec2Angle(-edge.m_normalizedVector);
            }
            else
            {
                // crash front

                f32 angle = getVec2Angle(normal);
                f32 difA = getShortestAngleDelta(myActor->getAngle(),angle);
                f32 difB = getShortestAngleDelta(myActor->getAngle(),angle+MTH_PI);

                if ( f32_Abs(difA) < f32_Abs(difB) )
                {
                    targetAngle = angle;
                }
                else
                {
                    targetAngle = angle + MTH_PI;
                }

                Vec2d dir = Vec2d::Right.Rotate(targetAngle);

                bbool lookRight = dir.dot(edge.m_normalizedVector.getPerpendicular()) < 0.f;

                m_tree->getActor()->setFlipped(!lookRight);
                m_crashedVertical = bfalse;
            }

            m_crashed = btrue;
            m_physComponent->setDisabled(btrue);
            myActor->setAngle(targetAngle);
        }
    }
}

void Ray_BTActionReceiveHit::setupPunchFront( bbool _restart )
{
    f32 force = 0;
    f32 friction = 1.f;

    switch(m_hitLevel)
    {
    case 0:
        m_totalTime = m_timeCounter = getTemplate()->getReceivePunchFrontWeakTime();
        force = getTemplate()->getReceivePunchFrontWeakForce();
        friction = getTemplate()->getReceivePunchFrontWeakFrictionMultiplier();
        break;
    case 1:
        m_totalTime = m_timeCounter = getTemplate()->getReceivePunchFrontStrongTime();
        force = getTemplate()->getReceivePunchFrontStrongForce();
        friction = getTemplate()->getReceivePunchFrontStrongFrictionMultiplier();
        break;
    default:
        m_totalTime = m_timeCounter = getTemplate()->getReceivePunchFrontMegaTime();
        force = getTemplate()->getReceivePunchFrontMegaForce();
        friction = getTemplate()->getReceivePunchFrontMegaFrictionMultiplier();
        break;
    }

    Vec2d hitDir;

    if ( m_aiController->isSwiming() == STANCE_SWIM )
    {
        hitDir = m_hitDir;
    }
    else
    {
        Vec2d lookDir = AIUtils::getLookDir(m_tree->getActor(),m_tree->getActor()->isFlipped());

        if ( m_hitDir.dot(lookDir) > 0.f )
        {
            lookDir *= -1.f;
            m_tree->getActor()->setFlipped(!m_tree->getActor()->isFlipped());
        }

        hitDir = -lookDir;
    }

    // Cancel our speed
    m_physComponent->setSpeed(Vec2d::Zero);

    // Add the force based on the hit level
    m_physComponent->addForce(hitDir*force);
    m_physComponent->setFrictionMultiplier(friction);

    if ( _restart )
    {
        m_animComponent->resetTree();
    }
}

void Ray_BTActionReceiveHit::setupPunchUp( bbool _restart )
{
    m_totalTime = m_timeCounter = 0.f;

    if ( !_restart || m_physComponent->getSpeed().dot(m_physComponent->getCurrentGravityDir()) > 1.f ||
        m_hitLevel > 0 )
    {
        f32 force = 0;
        f32 gravity = 1.f;

        switch(m_hitLevel)
        {
        case 0:
            force = getTemplate()->getReceivePunchUpWeakForce();
            gravity = getTemplate()->getReceivePunchUpWeakGravityMultiplier();
            break;
        case 1:
            force = getTemplate()->getReceivePunchUpStrongForce();
            gravity = getTemplate()->getReceivePunchUpStrongGravityMultiplier();
            break;
        default:
            force = getTemplate()->getReceivePunchUpMegaForce();
            gravity = getTemplate()->getReceivePunchUpMegaGravityMultiplier();
            break;
        }

        m_physComponent->setSpeed(Vec2d::Zero);
        m_physComponent->addForce(-m_physComponent->getCurrentGravityDir()*force);
        m_physComponent->setGravityMultiplier(gravity);
        m_appex = bfalse;
    }
    else
    {
        if ( m_appex )
        {
            m_animComponent->resetCurTime();
        }
    }
}

void Ray_BTActionReceiveHit::setupHurtBounce( bbool _restart )
{
    f32 friction = 1.f;
    f32 forceH = 0.f;
    f32 forceV = 0.f;

    switch(m_hitLevel)
    {
    case 0:
        m_totalTime = m_timeCounter = getTemplate()->getReceiveBounceWeakTime();
        friction = getTemplate()->getReceiveBounceWeakFrictionMult();
        forceH = getTemplate()->getReceiveBounceHWeakForce();
        forceV = getTemplate()->getReceiveBounceVWeakForce();
        break;
    case 1:
        m_totalTime = m_timeCounter = getTemplate()->getReceiveBounceStrongTime();
        friction = getTemplate()->getReceiveBounceStrongFrictionMult();
        forceH = getTemplate()->getReceiveBounceHStrongForce();
        forceV = getTemplate()->getReceiveBounceVStrongForce();
        break;
    default:
        m_totalTime = m_timeCounter = getTemplate()->getReceiveBounceMegaTime();
        friction = getTemplate()->getReceiveBounceMegaFrictionMult();
        forceH = getTemplate()->getReceiveBounceHMegaForce();
        forceV = getTemplate()->getReceiveBounceVMegaForce();
        break;
    }

    m_physComponent->setSpeed(Vec2d::Zero);

    if ( m_hitDir != Vec2d::Zero )
    {
        StickToPolylinePhysComponent::EdgeOrientationType orientationType = m_physComponent->getEdgeOrientationType(-m_hitDir.getPerpendicular());

        switch(orientationType)
        {
        case StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND:
            {
                m_physComponent->addForce(-m_physComponent->getCurrentGravityDir()*forceV);
            }
            break;
        case StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL:
            {
                Vec2d forceDir;
                const PolyLineEdge* edge = m_physComponent->getStickedEdge();

                if ( edge )
                {
                    forceDir = edge->m_normalizedVector;
                }
                else
                {
                    forceDir = Vec2d::Right;
                }

                f32 forceSign = m_hitDir.dot(forceDir) < 0.f ? -1.f : 1.f;

                m_physComponent->addForce(forceDir*forceSign*forceH);
                m_physComponent->setFrictionMultiplier(friction);
            }
            break;
        case StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF:
            break;
        }
    }

    if ( _restart )
    {
        m_animComponent->resetTree();
    }
}

void Ray_BTActionReceiveHit::setupEarthquake( bbool _restart )
{
    m_physComponent->setSpeed(Vec2d::Zero);

    if ( !m_physComponent->getStickedEdge() )
    {
        f32 force = 0;

        switch(m_hitLevel)
        {
        case 0:
            force = getTemplate()->getReceiveEarthquakeWeakForce();
            break;
        case 1:
            force = getTemplate()->getReceiveEarthquakeStrongForce();
            break;
        default:
            force = getTemplate()->getReceiveEarthquakeMegaForce();
            break;
        }

        m_physComponent->addForce(m_physComponent->getCurrentGravityDir()*force);
    }

    if ( _restart )
    {
        m_animComponent->resetTree();
    }
}

bbool Ray_BTActionReceiveHit::checkFinished() const
{
    if ( !m_crashed )
    {
        switch(m_reactType)
        {
        case RECEIVEDHITTYPE_EJECTY:
        case RECEIVEDHITTYPE_UPPUNCH:
            {
                if ( checkFinishedPunchUp() )
                {
                    return btrue;
                }
            }
            break;
        case RECEIVEDHITTYPE_HURTBOUNCE:
            {
                if ( checkFinishedHurtBounce() )
                {
                    return btrue;
                }
            }
            break;
        case RECEIVEDHITTYPE_EARTHQUAKE:
            {
                if ( checkFinishedEarthquake() )
                {
                    return btrue;
                }
            }
            break;
        default:
            {
                if ( checkFinishedPunchFront() )
                {
                    return btrue;
                }
            }
            break;
        }
    }
    else
    {
        if ( checkFinishedCrashed() )
        {
            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_BTActionReceiveHit::checkFinishedPunchFront() const
{
    if ( !m_timeCounter )
    {
        return btrue;
    }

    return bfalse;
}

bbool Ray_BTActionReceiveHit::checkFinishedPunchUp() const
{
    if ( m_physComponent->getStickedEdge() && m_animComponent->isMainNodeFinished() )
    {
        return btrue;
    }

    return bfalse;
}

bbool Ray_BTActionReceiveHit::checkFinishedHurtBounce() const
{
    if ( !m_timeCounter )
    {
        return btrue;
    }

    return bfalse;
}

bbool Ray_BTActionReceiveHit::checkFinishedEarthquake() const
{
    if ( m_animComponent->isMainNodeFinished() )
    {
        return btrue;
    }

    return bfalse;
}

bbool Ray_BTActionReceiveHit::checkFinishedCrashed() const
{
    if ( m_animComponent->isMainNodeFinished() )
    {
        return btrue;
    }

    return bfalse;
}

void Ray_BTActionReceiveHit::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == MRK_UpHitApex_Start )
        {
            m_appexStart = btrue;
        }
        else if ( animEvent->getName() == MRK_UpHitApex_Stop )
        {
            m_appexStop = btrue;
        }
    }
    else if ( EventBlockedByPolyline* blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
    {
        if ( !m_crashed )
        {
            PolyLine* poly = NULL;
            const PolyLineEdge* edge = NULL;

            AIUtils::getPolyLine(blockedEvent->getPolyline(),blockedEvent->getEdge(),poly,edge);

            if ( edge )
            {
                f32 dotProd = edge->m_normalizedVector.getPerpendicular().dot(blockedEvent->getSpeed());

                // Make sure our speed was going quite opposite to the edge we are blocked by
                if ( dotProd < -1.f )
                {
                    m_wasBlocked = btrue;
                    m_blockedPolyline = blockedEvent->getPolyline();
                    m_blockedEdgeIndex = blockedEvent->getEdge();
                }
            }
        }
    }
}

void Ray_BTActionReceiveHit::updateAnimInput()
{
    Super::updateAnimInput();

    m_animComponent->setInput(ITF_GET_STRINGID_CRC(ReceiveHitCrash,3219639338),m_crashed);
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(ReceiveHitCrashVertical,7195170),m_crashedVertical);
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(ReceivedHitLevel,3435787350),m_hitLevel);
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(ReceivedHitType,490560957),m_reactType);
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionReceiveHit_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionReceiveHit_Template)

    SERIALIZE_MEMBER("anim", m_anim);
    SERIALIZE_MEMBER("punchFrontWeakForce",m_receivePunchFrontWeakForce);
    SERIALIZE_MEMBER("punchFrontStrongForce",m_receivePunchFrontStrongForce);
    SERIALIZE_MEMBER("punchFrontMegaForce",m_receivePunchFrontMegaForce);
    SERIALIZE_MEMBER("punchFrontWeakFrictionMult",m_receivePunchFrontWeakFrictionMult);
    SERIALIZE_MEMBER("punchFrontStrongFrictionMult",m_receivePunchFrontStrongFrictionMult);
    SERIALIZE_MEMBER("punchFrontMegaFrictionMult",m_receivePunchFrontMegaFrictionMult);
    SERIALIZE_MEMBER("punchFrontWeakTime",m_receivePunchFrontWeakTime);
    SERIALIZE_MEMBER("punchFrontStrongTime",m_receivePunchFrontStrongTime);
    SERIALIZE_MEMBER("punchFrontMegaTime",m_receivePunchFrontMegaTime);
    SERIALIZE_MEMBER("punchUpWeakForce",m_receivePunchUpWeakForce);
    SERIALIZE_MEMBER("punchUpStrongForce",m_receivePunchUpStrongForce);
    SERIALIZE_MEMBER("punchUpMegaForce",m_receivePunchUpMegaForce);
    SERIALIZE_MEMBER("punchUpWeakGravityMultiplier",m_receivePunchUpWeakGravityMult);
    SERIALIZE_MEMBER("punchUpStrongGravityMultiplier",m_receivePunchUpStrongGravityMult);
    SERIALIZE_MEMBER("punchUpMegaGravityMultiplier",m_receivePunchUpMegaGravityMult);
    SERIALIZE_MEMBER("earthquakeWeakForce",m_receiveEarthquakeWeakForce);
    SERIALIZE_MEMBER("earthquakeStrongForce",m_receiveEarthquakeStrongForce);
    SERIALIZE_MEMBER("earthquakeMegaForce",m_receiveEarthquakeMegaForce);
    SERIALIZE_MEMBER("bounceVWeakForce",m_receiveBounceVWeakForce);
    SERIALIZE_MEMBER("bounceVStrongForce",m_receiveBounceVStrongForce);
    SERIALIZE_MEMBER("bounceVMegaForce",m_receiveBounceVMegaForce);
    SERIALIZE_MEMBER("bounceHWeakForce",m_receiveBounceHWeakForce);
    SERIALIZE_MEMBER("bounceHStrongForce",m_receiveBounceHStrongForce);
    SERIALIZE_MEMBER("bounceHMegaForce",m_receiveBounceHMegaForce);
    SERIALIZE_MEMBER("bounceWeakTime",m_receiveBounceWeakTime);
    SERIALIZE_MEMBER("bounceStrongTime",m_receiveBounceStrongTime);
    SERIALIZE_MEMBER("bounceMegaTime",m_receiveBounceMegaTime);
    SERIALIZE_MEMBER("bounceWeakFrictionMultiplier",m_receiveBounceWeakFrictionMult);
    SERIALIZE_MEMBER("bounceStrongFrictionMultiplier",m_receiveBounceStrongFrictionMult);
    SERIALIZE_MEMBER("bounceMegaFrictionMultiplier",m_receiveBounceMegaFrictionMult);

END_SERIALIZATION()

Ray_BTActionReceiveHit_Template::Ray_BTActionReceiveHit_Template()
: m_receivePunchFrontWeakForce(200.f)
, m_receivePunchFrontStrongForce(600.f)
, m_receivePunchFrontMegaForce(1200.f)
, m_receivePunchFrontWeakFrictionMult(0.6f)
, m_receivePunchFrontStrongFrictionMult(0.6f)
, m_receivePunchFrontMegaFrictionMult(0.5f)
, m_receivePunchFrontWeakTime(0.1f)
, m_receivePunchFrontStrongTime(0.2f)
, m_receivePunchFrontMegaTime(0.6f)
, m_receivePunchUpWeakForce(400.f)
, m_receivePunchUpStrongForce(650.f)
, m_receivePunchUpMegaForce(800.f)
, m_receivePunchUpWeakGravityMult(0.5f)
, m_receivePunchUpStrongGravityMult(0.5f)
, m_receivePunchUpMegaGravityMult(0.5f)
, m_receiveEarthquakeWeakForce(5000.f)
, m_receiveEarthquakeStrongForce(5000.f)
, m_receiveEarthquakeMegaForce(5000.f)
, m_receiveBounceVWeakForce(1100.f)
, m_receiveBounceVStrongForce(1150.f)
, m_receiveBounceVMegaForce(1200.f)
, m_receiveBounceHWeakForce(400.f)
, m_receiveBounceHStrongForce(500.f)
, m_receiveBounceHMegaForce(700.f)
, m_receiveBounceWeakTime(0.2f)
, m_receiveBounceStrongTime(0.2f)
, m_receiveBounceMegaTime(0.2f)
, m_receiveBounceWeakFrictionMult(0.5f)
, m_receiveBounceStrongFrictionMult(0.6f)
, m_receiveBounceMegaFrictionMult(0.2f)
{
}

}
