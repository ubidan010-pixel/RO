
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONSTAYIDLE_H_
#include "gameplay/ai/BTs/BTActionStayIdle.h"
#endif // _ITF_BTACTIONSTAYIDLE_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionStayIdle);

BTActionStayIdle::BTActionStayIdle()
: m_physComponent(NULL)
, m_animComponent(NULL)
, m_aiController(NULL)
, m_phantomComponent(NULL)
, m_prevState(STAYSTATE_GROUND)
, m_avoiding(bfalse)
, m_avoidanceTimer(0.f)
{
}

void BTActionStayIdle::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_physComponent = _tree->getActor()->GetComponent<StickToPolylinePhysComponent>();
    m_animComponent = _tree->getActor()->GetComponent<AnimLightComponent>();
    m_aiController = _tree->getActor()->GetComponent<GroundAIControllerComponent>();
    m_phantomComponent = _tree->getActor()->GetComponent<PhantomComponent>();
}

bbool BTActionStayIdle::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"AnimLightComponent",m_animComponent!=NULL,"%s: anim component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_physComponent )
    {
        ITF_WARNING_PARAM(_actor,"stickToPolylinePhysComponent",m_physComponent!=NULL,"%s: stickToPolylinePhysComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_aiController )
    {
        ITF_WARNING_PARAM(_actor,"groundAIControllerComponent",m_aiController!=NULL,"%s: groundAIControllerComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_phantomComponent )
    {
        ITF_WARNING_PARAM(_actor,"PhantomComponent",m_phantomComponent!=NULL,"%s: PhantomComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void BTActionStayIdle::onActivate()
{
    Super::onActivate();

    m_currentRegions.clear();

    AIUtils::getPolylineRegions(m_tree->getActor()->getDepth(),m_tree->getActor()->get2DPos(),m_currentRegions);

    if ( getSwimPolyline() )
    {
        m_prevState = STAYSTATE_SWIM;
        m_animComponent->setAnim(getTemplate()->getSwimAnim());
    }
    else if ( m_physComponent->getStickedEdge() )
    {
        m_prevState = STAYSTATE_GROUND;
        m_animComponent->setAnim(getTemplate()->getGroundAnim());
    }
    else
    {
        m_prevState = STAYSTATE_FALL;
        m_animComponent->setAnim(getTemplate()->getFallAnim());
    }
}

bbool BTActionStayIdle::update( f32 _dt )
{
    Super::update(_dt);

    m_avoidanceTimer -= _dt;

    if ( m_avoidanceTimer <= 0.f || m_avoiding )
    {
        m_avoidanceTimer = Seeder::getSharedSeeder().GetFloat(0.5f,0.8f);
        checkAvoidance();
    }

    m_currentRegions.clear();

    AIUtils::getPolylineRegions(m_tree->getActor()->getDepth(),m_tree->getActor()->get2DPos(),m_currentRegions);

    PolyLine* swimPoly = getSwimPolyline();

    StayState newState;

    if ( swimPoly )
    {
        newState = STAYSTATE_SWIM;
    }
    else if ( m_physComponent->getStickedEdge() )
    {
        newState = STAYSTATE_GROUND;
    }
    else
    {
        newState = STAYSTATE_FALL;
    }

    if ( newState != m_prevState )
    {
        switch(newState)
        {
            case STAYSTATE_GROUND:
                m_animComponent->setAnim(getTemplate()->getGroundAnim());
                break;
            case STAYSTATE_FALL:
                m_animComponent->setAnim(getTemplate()->getFallAnim());
                break;
            case STAYSTATE_SWIM:
                {
                    u32 surfaceEdge, closestEdge;
                    f32 surfaceT, closestT;
                    bbool swimSurface,swimInside;

                    m_animComponent->setAnim(getTemplate()->getSwimAnim());

                    AIUtils::getSwimingPolylineData(swimPoly,m_tree->getActor()->getPos(),m_physComponent->getCurrentGravityDir(),1.f,
                        surfaceEdge,surfaceT,swimSurface,
                        closestEdge,closestT,swimInside);

                    m_aiController->setSwimingPolylineData(swimPoly->getRef(),surfaceEdge,surfaceT,swimSurface);
                }
                break;
        }

        m_prevState = newState;
    }
    
    return btrue;
}

void BTActionStayIdle::checkAvoidance()
{
    m_avoiding = bfalse;

    Actor* myActor = m_tree->getActor();
    bbool avoidLeft = bfalse;
    bbool avoidRight = bfalse;
    PhysContactsContainer contacts;
    PHYSWORLD->collidePhantoms(m_phantomComponent->getShapePos(), m_phantomComponent->getShapePos(), myActor->getAngle(),
        m_phantomComponent->getShape(), myActor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
        contacts);

    const u32 contactCount = contacts.size();
    Vec2d dirX = m_physComponent->getCurrentGravityDir().getPerpendicular();
    f32 myDist = dirX.dot(myActor->get2DPos());

    EventQueryPhysicsData physData;

    for ( u32 i = 0; i < contactCount; i++ )
    {
        const SCollidableContact& contact = contacts[i];

        if ( contact.m_collidableUserData == myActor->getRef() )
        {
            continue;
        }

        Actor* actor = AIUtils::getActor(contact.m_collidableUserData);

        if ( !actor )
        {
            continue;
        }

        f32 hisDist = dirX.dot(actor->get2DPos());

        if ( f32_Abs(myDist-hisDist) > getTemplate()->getAvoidanceRadius() )
        {
            continue;
        }

        actor->onEvent(&physData);

        if ( physData.getStickyEdgePolyline() == U32_INVALID )
        {
            continue;
        }

        if ( physData.getSpeed() != Vec2d::Zero )
        {
            continue;
        }

        if ( hisDist > myDist )
        {
            avoidLeft = btrue;
        }
        else
        {
            avoidRight = btrue;
        }

        if ( avoidRight && avoidLeft )
        {
            m_avoiding = bfalse;
            break;
        }
        else
        {
            m_avoiding = btrue;
        }
    }

    if ( m_avoiding )
    {
        Vec2d dirX = m_physComponent->getCurrentGravityDir().getPerpendicular();
        m_aiController->setMoveDir(avoidRight?dirX:-dirX);
    }
}

PolyLine* BTActionStayIdle::getSwimPolyline() const
{
    u32 numRegions = m_currentRegions.size();

    for ( u32 i = 0; i < numRegions; i++ )
    {
        const AIUtils::PolylineRegionData& data = m_currentRegions[i];

        if ( data.m_regionId == s_RegionName_Swim )
        {
            return data.m_polyline;
        }
    }

    return NULL;
}

IMPLEMENT_OBJECT_RTTI(BTActionStayIdle_Template);

BEGIN_SERIALIZATION_CHILD(BTActionStayIdle_Template)

    SERIALIZE_MEMBER("groundAnim",m_groundAnim);
    SERIALIZE_MEMBER("swimAnim",m_swimAnim);
    SERIALIZE_MEMBER("fallAnim",m_fallAnim);
    SERIALIZE_MEMBER("avoidanceRadius",m_avoidanceRadius);

END_SERIALIZATION()

BTActionStayIdle_Template::BTActionStayIdle_Template()
: m_avoidanceRadius(0.4f)
{
}

}