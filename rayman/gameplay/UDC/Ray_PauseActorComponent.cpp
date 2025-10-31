#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PAUSEACTORCOMPONENT_H_
#include "rayman/gameplay/UDC/Ray_PauseActorComponent.h"
#endif //_ITF_RAY_PAUSEACTORCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_PauseActorComponent)

BEGIN_SERIALIZATION_CHILD(Ray_PauseActorComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_PauseActorComponent)
    VALIDATE_COMPONENT_PARAM("", m_playerDetectorComponent, "Ray_PauseActorComponent requires a PlayerDetectorComponent");
    VALIDATE_COMPONENT_PARAM("", m_linkComponent, "Ray_PauseActorComponent requires a LinkComponent");
END_VALIDATE_COMPONENT()

Ray_PauseActorComponent::Ray_PauseActorComponent()
    : m_playerDetectorComponent(NULL)
{
}

Ray_PauseActorComponent::~Ray_PauseActorComponent()
{
}

void Ray_PauseActorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_playerDetectorComponent = m_actor->GetComponent<PlayerDetectorComponent>();    // mandatory, validated

    pauseChildren(btrue);
    m_goingToPause = bfalse;
    m_alphaRunning = bfalse;
}

void Ray_PauseActorComponent::pauseChildren( bbool _pause )
{
    const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
    for ( LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it )
    {
        const ObjectPath& obj = it->getPath();
        Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,obj);

        if ( pickable )
        {
            Actor* actor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            
            if ( actor )
            {
                if ( actor->isUpdatePaused() != _pause )
                {
                    if ( _pause )
                    {
                        actor->pauseUpdate();
                    }
                    else
                    {
                        actor->unpauseUpdate();
                    }

                    actor->disableDraw(_pause);
                }
            }
        }
    }
}

void Ray_PauseActorComponent::sendEvent( ActorRef _activator, bbool _activated, const bbool _fromRetriggerOnCheckpoint /*= bfalse*/ )
{
    Super::sendEvent(_activator, _activated, _fromRetriggerOnCheckpoint);

    if (!m_alphaRunning)
    {
        m_alphaRunning = btrue;
        m_goingToPause = !_activated;
        if (m_goingToPause)
            m_alhpa = 1.f;
        else
        {
            pauseChildren(bfalse);
            m_alhpa = 0.f;
        }
    }
}

void Ray_PauseActorComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    PhysShape* shape = m_playerDetectorComponent->getCurrentShape();
    if ( shape && shape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
    {
        AABB aabb(m_actor->get2DPos());
        const PhysShapePolygon* poly = static_cast<const PhysShapePolygon*>(shape);
        const PhysShapePolygon::PointsList& points = poly->getVertex();
        u32 numPoints = points.size();
        
        for ( u32 i = 0; i < numPoints; i++ )
        {
            aabb.grow(points[i].Rotate(m_actor->getAngle())+m_actor->get2DPos());
        }

        GetActor()->growRelativeAABBFromAbsolute(aabb);
    }


    if (m_alphaRunning)
    {
        if (m_goingToPause)
        {
            m_alhpa -= 2.f * _dt;
            if (m_alhpa < 0.f)
            {
                m_alhpa = 0.f;
                m_alphaRunning = bfalse;
                pauseChildren(btrue);
            }
        }
        else
        {
            m_alhpa += 2.f * _dt;
            if (m_alhpa > 1.f)
            {
                m_alhpa = 1.f;
                m_alphaRunning = bfalse;
            }
        }

        const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
        for ( LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it )
        {
            const ObjectPath& obj = it->getPath();
            Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,obj);

            if ( pickable )
            {
                Actor* actor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                
                if ( actor )
                {
                    SubSceneActor* pSubScene = static_cast<SubSceneActor*> (actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)));
                    if (pSubScene)
                    {
                        pSubScene->setAlpha(m_alhpa);
                    }
                    else
                    {
                        GraphicComponent* pGraphic = actor->GetComponent<GraphicComponent>();
                        if (pGraphic)
                            pGraphic->setAlpha(m_alhpa);
                    }
                }
                else if (pickable->getObjectType() == BaseObject::eFrise)
                {
                    Frise* pFrise = (Frise*)pickable;
                    pFrise->setglobalAlphaFade(m_alhpa);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_PauseActorComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_PauseActorComponent_Template)
END_SERIALIZATION()

} // namespace ITF