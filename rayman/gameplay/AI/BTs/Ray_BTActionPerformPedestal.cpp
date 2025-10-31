
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONPERFORMPEDESTAL_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionPerformPedestal.h"
#endif // _ITF_RAY_BTACTIONPERFORMPEDESTAL_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/AI/BTs/Ray_AIBTFacts.h"
#endif // _ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionPerformPedestal);

const f32 Ray_BTActionPerformPedestal::s_onPedestalInterpolateDuration = 0.1f;

Ray_BTActionPerformPedestal::Ray_BTActionPerformPedestal()
: m_animComponent(NULL)
, m_physComponent(NULL)
, m_aiControllerComponent(NULL)
, m_phantomComponent(NULL)
, m_supportBoneIndex(U32_INVALID)
, m_onPedestalInterpolateCounter(0.f)
, m_startPedestalPos(Vec2d::Zero)
, m_firstUpdate(bfalse)
, m_onPedestalValid(bfalse)
, m_doingPedestal(bfalse)
{
}

Ray_BTActionPerformPedestal::~Ray_BTActionPerformPedestal()
{
}

void Ray_BTActionPerformPedestal::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_animComponent = _tree->getActor()->GetComponent<AnimatedComponent>();
    m_physComponent = _tree->getActor()->GetComponent<StickToPolylinePhysComponent>();
    m_phantomComponent = _tree->getActor()->GetComponent<PhantomComponent>();
    m_aiControllerComponent = _tree->getActor()->GetComponent<GroundAIControllerComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(Ray_EventQueryIsInPedestal,2146089474), m_tree);
    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(Ray_EventQueryPedestalInfo,2107418028), m_tree);
    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(Ray_EventReleasePedestal,4284763324), m_tree);
    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(Ray_EventCheckPedestalSupport,1578355683), m_tree);
}

bbool Ray_BTActionPerformPedestal::validate( Actor* _actor ) const
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

    if ( !m_physComponent )
    {
        ITF_WARNING_PARAM(_actor,"StickToPolylinePhysComponent",m_physComponent!=NULL,"%s: sticktopolylinephyscomponent component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_aiControllerComponent )
    {
        ITF_WARNING_PARAM(_actor,"GroundAIControllerComponent",m_aiControllerComponent!=NULL,"%s: ground ai controller component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_phantomComponent )
    {
        ITF_WARNING_PARAM(_actor,"PhantomComponent",m_phantomComponent!=NULL,"%s: phantom component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getAnimPedestal().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"animPedestal",getTemplate()->getAnimPedestal().isValid(),"%s: there is no pedestal anim set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getAnimJump().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"animJump",getTemplate()->getAnimJump().isValid(),"%s: there is no jump anim set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getSupportBone().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"anim",getTemplate()->getSupportBone().isValid(),"%s: there is no support bone set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTActionPerformPedestal::decide( f32 _dt )
{
    return m_physComponent->getStickedEdgeIndex() != U32_INVALID || m_tree->getBlackBoard().factExists(Ray_AIFact_OnPedestalActor);
}

void Ray_BTActionPerformPedestal::onActivate()
{
    Super::onActivate();

    m_doingPedestal = bfalse;
    m_firstUpdate = btrue;

    if ( checkNeedJump() )
    {
        setJump();
    }
    else
    {
        setPedestal();
    }
}

void Ray_BTActionPerformPedestal::setJump()
{
    if ( m_doingPedestal || m_firstUpdate )
    {
        m_doingPedestal = bfalse;
        m_animComponent->setAnim(getTemplate()->getAnimJump());
        m_aiControllerComponent->setDisablePhysics(bfalse);
        m_physComponent->setUpdateRotation(btrue);
        m_tree->getBlackBoard().removeFact(Ray_AIFact_OnPedestalActor);
        m_aiControllerComponent->setJump(btrue);
    }
}

void Ray_BTActionPerformPedestal::setPedestal()
{
    if ( !m_doingPedestal || m_firstUpdate )
    {
        m_doingPedestal = btrue;
        m_animComponent->setAnim(getTemplate()->getAnimPedestal());
        m_physComponent->setUpdateRotation(bfalse);

        ObjectRef pedestalRef = ITF_INVALID_OBJREF;

        m_tree->getBlackBoard().getFact(Ray_AIFact_OnPedestalActor,pedestalRef);

        Actor* pedestalActor = AIUtils::getActor(pedestalRef);

        if ( pedestalActor )
        {
            m_onPedestalInterpolateCounter = s_onPedestalInterpolateDuration;
            m_startPedestalPos = m_tree->getActor()->get2DPos();
            m_onPedestalValid = btrue;
            m_aiControllerComponent->setDisablePhysics(btrue);
        }
        else
        {
            m_onPedestalValid = bfalse;
        }
    }
}

void Ray_BTActionPerformPedestal::onDeactivate()
{
    Super::onDeactivate();

    if ( m_doingPedestal )
    {
        m_aiControllerComponent->setDisablePhysics(bfalse);
        m_physComponent->setUpdateRotation(btrue);

        if ( m_tree->getBlackBoard().factExists(Ray_AIFact_OnPedestalActor) )
        {
            ObjectRef pedestalRef = ITF_INVALID_OBJREF;

            m_tree->getBlackBoard().getFact(Ray_AIFact_OnPedestalActor,pedestalRef);

            Actor* pedestalActor = AIUtils::getActor(pedestalRef);

            if ( pedestalActor )
            {
                Ray_EventReleasePedestal releaseEvent(m_tree->getActor()->getRef());

                pedestalActor->onEvent(&releaseEvent);
            }

            m_tree->getBlackBoard().removeFact(Ray_AIFact_OnPedestalActor);
        }
    }

    m_objectsOnPedestal.clear();
}

bbool Ray_BTActionPerformPedestal::update( f32 _dt )
{
    Super::update(_dt);

    if ( m_doingPedestal )
    {
        updatePedestal(_dt);
    }
    else
    {
        updateJump(_dt);
    }

    return m_onPedestalValid || !m_tree->getBlackBoard().factExists(Ray_AIFact_OnPedestalActor);
}

void Ray_BTActionPerformPedestal::updatePedestal( f32 _dt )
{
    ObjectRef pedestalRef = ITF_INVALID_OBJREF;

    m_tree->getBlackBoard().getFact(Ray_AIFact_OnPedestalActor,pedestalRef);

    Actor* pedestalActor = AIUtils::getActor(pedestalRef);

    if ( pedestalActor )
    {
        Ray_EventQueryPedestalInfo pedestalInfo(m_tree->getActor()->getRef());

        pedestalActor->onEvent(&pedestalInfo);

        if ( !pedestalInfo.getValid() )
        {
            m_onPedestalValid = bfalse;
            return;
        }

        u32 numObjectsOnPedestal = pedestalInfo.getObjectsOnPedestal().size();

        if ( numObjectsOnPedestal > 1 )
        {
            // There is more than one character on the pedestal, check someone is doing the pedestal and jump if it is so
            for ( u32 i = 0; i < numObjectsOnPedestal ; i++ )
            {
                ObjectRef objRef = pedestalInfo.getObjectsOnPedestal()[i];

                if ( objRef == m_tree->getActor()->getRef() )
                {
                    continue;
                }

                Actor* objActor = AIUtils::getActor(objRef);

                if ( !objActor )
                {
                    continue;
                }

                Ray_EventQueryIsInPedestal inPedestal;

                objActor->onEvent(&inPedestal);

                if ( !inPedestal.getIsInPedestal() )
                {
                    continue;
                }

                if ( getTemplate()->getWalkTargetFactActor().isValid() && getTemplate()->getWalkTargetFactPos().isValid() )
                {
                    objActor->onEvent(&pedestalInfo);

                    m_tree->getBlackBoard().setFact(getTemplate()->getWalkTargetFactPos(),pedestalInfo.getPedestalPos());
                    m_tree->getBlackBoard().removeFact(getTemplate()->getWalkTargetFactActor());
                }

                Ray_EventReleasePedestal releaseEvent(m_tree->getActor()->getRef());

                pedestalActor->onEvent(&releaseEvent);

                setJump();

                return;
            }
        }

        Vec2d pedestalPos = pedestalInfo.getPedestalPos();
        pedestalPos -= m_physComponent->getCurrentGravityDir() * m_physComponent->getRadius();

        if ( m_onPedestalInterpolateCounter == 0.f )
        {
            m_tree->getActor()->set2DPos(pedestalPos);
        }
        else
        {
            f32 t = 1.f - (m_onPedestalInterpolateCounter/s_onPedestalInterpolateDuration);
            Vec2d newPos = Interpolate(m_startPedestalPos,pedestalPos,t);

            m_tree->getActor()->set2DPos(newPos);

            m_onPedestalInterpolateCounter = Max(m_onPedestalInterpolateCounter-_dt,0.f);
        }
    }

    f32 targetAngle = getVec2Angle(m_physComponent->getCurrentGravityDir().getPerpendicular());
    f32 myAngle = m_tree->getActor()->getAngle();
    f32 dif = getShortestAngleDelta(myAngle,targetAngle);
    f32 newAngle = myAngle + (dif*0.1f);

    m_tree->getActor()->updateComponentPosAngle(m_tree->getActor()->getPos(),newAngle,NULL);

    checkTriggerPedestal();
}

void Ray_BTActionPerformPedestal::updateJump( f32 _dt )
{
    m_aiControllerComponent->setJump();
}

void Ray_BTActionPerformPedestal::checkTriggerPedestal()
{
    Vec2d pedestalPos = getPedestalPos();

    if ( pedestalPos == Vec2d::Zero )
    {
        return;
    }

    PhysShapeCircle circle(getTemplate()->getAreaRadius());
    PhysContactsContainer contacts;
    f32 distCheckSq = getTemplate()->getDistanceCheck()*getTemplate()->getDistanceCheck();
    ObjectRef actorRef = m_tree->getActor()->getRef();

    PHYSWORLD->collidePhantoms(pedestalPos,pedestalPos,m_tree->getActor()->getAngle(),&circle,m_tree->getActor()->getDepth(),(ECollisionFilter)(ECOLLISIONFILTER_CHARACTERS),contacts);

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_collidableUserData == actorRef )
        {
            continue;
        }

        Actor* actor = AIUtils::getActor(contact.m_collidableUserData);

        if ( !actor )
        {
            continue;
        }

        if ( m_objectsOnPedestal.find(contact.m_collidableUserData) != -1 )
        {
            continue;
        }

        f32 distSq = ( pedestalPos - actor->get2DPos() ).sqrnorm();

        if ( distSq > distCheckSq )
        {
            continue;
        }

        Ray_EventCheckPedestalSupport pedestalCheck(actorRef,pedestalPos);

        actor->onEvent(&pedestalCheck);

        if ( pedestalCheck.getTriggerPedestal() )
        {
            m_objectsOnPedestal.push_back(actor->getRef());
        }
    }
}

bbool Ray_BTActionPerformPedestal::checkNeedJump() const
{
    EventQueryPhysicsData queryPhysEvent;
    Actor* myActor = m_tree->getActor();
    PhysContactsContainer contacts;
    PHYSWORLD->collidePhantoms(m_phantomComponent->getShapePos(), m_phantomComponent->getShapePos(), myActor->getAngle(),
        m_phantomComponent->getShape(), myActor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
        contacts);

    const u32 contactCount = contacts.size();

    for ( u32 i = 0; i < contactCount; i++ )
    {
        const SCollidableContact& contact = contacts[i];

        if ( contact.m_collidableUserData == myActor->getRef() )
        {
            continue;
        }

        if ( m_tree->getBlackBoard().factExists(Ray_AIFact_OnPedestalActor) )
        {
            ObjectRef pedestalRef = ITF_INVALID_OBJREF;

            m_tree->getBlackBoard().getFact(Ray_AIFact_OnPedestalActor,pedestalRef);

            if ( pedestalRef == contact.m_collidableUserData )
            {
                continue;
            }
        }

        Actor* targetActor = AIUtils::getActor(contact.m_collidableUserData);

        if ( !targetActor )
        {
            continue;
        }

        Ray_EventQueryIsInPedestal inPedestal;

        targetActor->onEvent(&inPedestal);

        if ( !inPedestal.getIsInPedestal() )
        {
            continue;
        }

        targetActor->onEvent(&queryPhysEvent);

        if ( queryPhysEvent.getStickyEdgePolyline() != U32_INVALID )
        {
            // A guy doing the pedestal on the ground, for sure we need to jump
            return btrue;
        }

    }

    // We didn't find anyone else doing the pedestal here, so we don't need to jump

    return bfalse;
}

Vec2d Ray_BTActionPerformPedestal::getPedestalPos() const
{
    Vec2d pedestalPos = Vec2d::Zero;

    if (m_supportBoneIndex == U32_INVALID)
    {
        m_supportBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getSupportBone());

        if ( m_supportBoneIndex != U32_INVALID )
        {
            m_animComponent->getBonePos(m_supportBoneIndex, pedestalPos);
        }
    }
    else
    {
        m_animComponent->getBonePos(m_supportBoneIndex, pedestalPos);
    }

    return pedestalPos;
}

void Ray_BTActionPerformPedestal::updateAnimInput()
{
    Super::updateAnimInput();

    m_animComponent->setInput(ITF_GET_STRINGID_CRC(IsSupporting,987492597), m_objectsOnPedestal.size() > 0);
}

void Ray_BTActionPerformPedestal::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if (Ray_EventQueryPedestalInfo * pedestalQueryInfo = _event->DynamicCast<Ray_EventQueryPedestalInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryPedestalInfo,2107418028)))
    {
        processQueryPedestalInfo(pedestalQueryInfo);
    }
    else if (Ray_EventReleasePedestal * pedestalRelease = _event->DynamicCast<Ray_EventReleasePedestal>(ITF_GET_STRINGID_CRC(Ray_EventReleasePedestal,4284763324)))
    {
        processReleasePedestal(pedestalRelease);
    }
    else if (Ray_EventCheckPedestalSupport * pedestalSupportCheck = _event->DynamicCast<Ray_EventCheckPedestalSupport>(ITF_GET_STRINGID_CRC(Ray_EventCheckPedestalSupport,1578355683)))
    {
        processCheckPedestalSupport(pedestalSupportCheck);
    }
    else if ( Ray_EventQueryIsInPedestal* queryInPedestal = _event->DynamicCast<Ray_EventQueryIsInPedestal>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsInPedestal,2146089474)) )
    {
        queryInPedestal->setIsInPedestal(m_doingPedestal);
    }
}

void Ray_BTActionPerformPedestal::processQueryPedestalInfo( Ray_EventQueryPedestalInfo * _pedestalSupportCheck )
{
    if ( m_objectsOnPedestal.find(_pedestalSupportCheck->getSender()) == -1 )
    {
        return;
    }

    _pedestalSupportCheck->setValid(btrue);
    _pedestalSupportCheck->setPedestalPos(getPedestalPos());

    u32 numObjects = m_objectsOnPedestal.size();

    for ( u32 i = 0; i < numObjects; i++ )
    {
        _pedestalSupportCheck->addObjectOnPedestal(m_objectsOnPedestal[i]);
    }
}

void Ray_BTActionPerformPedestal::processReleasePedestal( Ray_EventReleasePedestal * _releasePedestal )
{
    u32 index = m_objectsOnPedestal.find(_releasePedestal->getSender());

    if ( index != -1 )
    {
        m_objectsOnPedestal.eraseNoOrder(index);
    }
}

void Ray_BTActionPerformPedestal::processCheckPedestalSupport( Ray_EventCheckPedestalSupport * _pedestalSupportCheck )
{
    if ( m_doingPedestal )
    {
        return;
    }

    f32 dotProd = m_physComponent->getCurrentGravityDir().dot(m_physComponent->getSpeed());

    if ( dotProd < 0.f )
    {
        return;
    }

    Vec2d feetPos = m_tree->getActor()->get2DPos() + ( ( Vec2d::Right.Rotate(m_tree->getActor()->getAngle()-MTH_PIBY2) ) * m_physComponent->getRadius() );
    f32 distSq = ( feetPos - _pedestalSupportCheck->getPedestalPos() ).sqrnorm();
    f32 distCheckSq = getTemplate()->getFeetDistanceCheck() * getTemplate()->getFeetDistanceCheck();

    if ( distSq > distCheckSq )
    {
        return;
    }

    _pedestalSupportCheck->setTriggerPedestal();

    m_tree->getBlackBoard().setFact(Ray_AIFact_OnPedestalActor,_pedestalSupportCheck->getSender());

    setPedestal();
    m_firstUpdate = btrue;
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_BTActionPerformPedestal::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const
{
    Super::drawEdit(drawInterface,_flags);

    Vec2d supportPos = getPedestalPos();

    if ( supportPos != Vec2d::Zero )
    {
        PhysShapeCircle circle(getTemplate()->getAreaRadius());

        DebugDraw::shape(supportPos, m_tree->getActor()->getDepth(), m_tree->getActor()->getAngle(), &circle,
            Color::white(), 0.f, "pedestal", 65 * MTH_DEGTORAD);
    }
}
#endif // ITF_SUPPORT_EDITOR

IMPLEMENT_OBJECT_RTTI(Ray_BTActionPerformPedestal_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionPerformPedestal_Template)

    SERIALIZE_MEMBER("animPedestal", m_animPedestal);
    SERIALIZE_MEMBER("animJump", m_animJump);
    SERIALIZE_MEMBER("supportBone", m_supportBone);
    SERIALIZE_MEMBER("walkTargetFactActor", m_walkTargetFactActor);
    SERIALIZE_MEMBER("walkTargetFactPos", m_walkTargetFactPos);
    SERIALIZE_MEMBER("areaRadius", m_areaRadius);
    SERIALIZE_MEMBER("distanceCheck", m_distanceCheck);
    SERIALIZE_MEMBER("feetDistanceCheck", m_feetDistanceCheck);

END_SERIALIZATION()

Ray_BTActionPerformPedestal_Template::Ray_BTActionPerformPedestal_Template()
: m_areaRadius(0.5f)
, m_distanceCheck(0.5f)
, m_feetDistanceCheck(0.5f)
{
}

}