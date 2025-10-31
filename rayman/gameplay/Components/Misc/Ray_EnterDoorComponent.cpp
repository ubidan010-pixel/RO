#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ENTERDOORCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_EnterDoorComponent.h"
#endif //_ITF_RAY_ENTERDOORCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_EnterDoorComponent)

BEGIN_SERIALIZATION_CHILD(Ray_EnterDoorComponent)
END_SERIALIZATION()

Ray_EnterDoorComponent::Ray_EnterDoorComponent()
: m_entryPointPos(Vec2d::Zero)
, m_exitPointPos(Vec2d::Zero)
, m_isTriggerDisabled(bfalse)
{
}

Ray_EnterDoorComponent::~Ray_EnterDoorComponent()
{
}

void Ray_EnterDoorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryDoorData,2144329540),this);
    m_isTriggerDisabled = bfalse;
}

void Ray_EnterDoorComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    setPoints();
}

void Ray_EnterDoorComponent::setPoints()
{
    AnimLightComponent* animLight = m_actor->GetComponent<AnimLightComponent>();
    AnimPolyline* pPolyline = NULL;

    if ( animLight )
    {
        const Vec2d* pointsBuffer = animLight->getCurrentFirstPolylinePoint(getTemplate()->getEntryPoint(), &pPolyline);
        if (pointsBuffer)
        {
            m_entryPointPos = ( *pointsBuffer - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());
        }

        pointsBuffer = animLight->getCurrentFirstPolylinePoint(getTemplate()->getExitPoint(), &pPolyline);
        if (pointsBuffer)
        {
            m_exitPointPos = ( *pointsBuffer - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());
        }
    }
}

Vec2d Ray_EnterDoorComponent::getEntryPoint() const
{
    return m_entryPointPos.Rotate(m_actor->getAngle()) + m_actor->get2DPos();
}

Vec2d Ray_EnterDoorComponent::getExitPoint() const
{
    return m_exitPointPos.Rotate(m_actor->getAngle()) + m_actor->get2DPos();
}

void Ray_EnterDoorComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( EventTrigger* triggered = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
    {
        triggerDoor(triggered);
    }
    else if ( Ray_EventQueryDoorData* queryData = _event->DynamicCast<Ray_EventQueryDoorData>(ITF_GET_STRINGID_CRC(Ray_EventQueryDoorData,2144329540)) )
    {
        queryData->setData(getEntryPoint(),getExitPoint(),getTemplate()->getNoEntry());
    }
}

void Ray_EnterDoorComponent::triggerDoor( EventTrigger* _trigger )
{
    const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
    
    if (m_isTriggerDisabled)
    {
        return;
    }

    if ( children.size() )
    {
        const ObjectPath& objPath = children[0].getPath();
        Pickable* pickable;
        
        if ( objPath.getIsAbsolute() )
        {
            pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(objPath);
        }
        else
        {
            pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,objPath);
        }

        if ( pickable )
        {
            Actor* child = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

            if ( child )
            {
                Actor* activator = AIUtils::getActor(_trigger->getActivator());

                if ( activator )
                {
                    Ray_EventTriggerEnterDoor enterDoorEvent(m_actor->getRef(),child->getRef());
                    activator->onEvent(&enterDoorEvent);
                }
            }
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_EnterDoorComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    Vec2d entryPoint = getEntryPoint();

    GFX_ADAPTER->drawDBGCircle(entryPoint.m_x, entryPoint.m_y, 0.2f, 0.f, 1.f, 0.f, 0.0f, GetActor()->getDepth());

    Vec2d exitPoint = getExitPoint();

    GFX_ADAPTER->drawDBGCircle(exitPoint.m_x, exitPoint.m_y, 0.2f, 1.f, 0.f, 0.f, 0.0f, GetActor()->getDepth());
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_EnterDoorComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_EnterDoorComponent_Template)

    SERIALIZE_MEMBER("entryPointId",m_entryPointID);
    SERIALIZE_MEMBER("exitPointId",m_exitPointID);
    SERIALIZE_MEMBER("noEntry",m_noEntry);

END_SERIALIZATION()

}

