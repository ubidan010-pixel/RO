#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChildLaunchComponent.h"
#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef ITF_CONSOLE_FINAL
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ChildLaunchComponent)
BEGIN_SERIALIZATION_CHILD(Ray_ChildLaunchComponent)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_MEMBER("nextLaunchIndex",m_nextLaunchIndex);
    END_CONDITION_BLOCK()
    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("hintFxEnabled",  m_hintFxEnabled);
    END_CONDITION_BLOCK()
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_ChildLaunchComponent)
    VALIDATE_COMPONENT_PARAM("", m_linkComponent, "LinkComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_ChildLaunchComponent::Ray_ChildLaunchComponent()
    : Super()
    , m_childrenList()
    , m_childrenCollected(bfalse)
    , m_nextLaunchIndex(0)
    , m_finalIndex(U32_INVALID)
    , m_hintFxEnabled(btrue)
    , m_hintFxRuntimeDisabled(bfalse)
    , m_hintFxGenerator()
    , m_hintFxActorRef()
    , m_linkComponent(NULL)
    , m_animLightComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_ChildLaunchComponent::~Ray_ChildLaunchComponent()
{
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_linkComponent = m_actor->GetComponent<LinkComponent>();
    
    // optional
    m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();

    // listen to link event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventOnLink,1558255516),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventChildLaunchAll,1581939056),this);
    
    // generates hint fx
    if (!getTemplate()->getHintFxPath().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_hintFxGenerator, getTemplate()->getHintFxPath());
    }
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::onBecomeActive()
{
    Super::onBecomeActive();

    // collect linked children
    // CAVEAT[Laurent]: do *not* move this in onActorLoaded, this brings a lot
    // of issues, notably the behaviors are not initialized so we can't check if
    // a launch behavior is present + disabling other actors in onActorLoaded
    // doesn't seem to be a good idea
    if (!m_childrenCollected)
    {
        collectChildren();
        m_childrenCollected = btrue;
    }
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    // re-collect children (fix for RO-13847 & RO-14286)
    if (!m_childrenCollected)
    {
        collectChildren();
        m_childrenCollected = btrue;
    }
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::Update(f32 _dt)
{
    Super::Update(_dt);
    
    // update hint fx
    updateHintFx(_dt);

    // always disable not-yet launched children (fix for darktoon geysers having
    // active children darktoons on checkpoint reload, due to their
    // startPaused=FALSE flag)
    u32 childrenCount = m_childrenList.size();
    for (u32 i=m_nextLaunchIndex; i<childrenCount; ++i)
    {
        Pickable* childPickable = m_childrenList[i].getIsAbsolute()
            ? SceneObjectPathUtils::getObjectFromAbsolutePath(m_childrenList[i])
            : SceneObjectPathUtils::getObjectFromRelativePath(m_actor, m_childrenList[i]);
        if (Actor* childActor = childPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
        {
            childActor->disable();
        }
    }

    // disable after last launch
    // NB[LaurentCou]: for pop corns only ATM, moved it to update for backtracking
    if (getTemplate()->getDisableAfterLaunch() && !canLaunch())
    {
        m_actor->disable();
    }
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);

    // process events
    if (EventOnLink* link = _event->DynamicCast<EventOnLink>(ITF_GET_STRINGID_CRC(EventOnLink,1558255516)))
    {
        processLink(link);
    }
    else if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_EventChildLaunchAll,1581939056)))
    {
        launchAll();
    }
}

//------------------------------------------------------------------------------
bbool Ray_ChildLaunchComponent::canLaunch() const
{
    return (m_nextLaunchIndex < m_childrenList.size());
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::launchAll()
{
    // launch everybody
    while (canLaunch())
    {
        launch();
    }
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::launch()
{
    // launched everybody
    if (m_nextLaunchIndex >= m_childrenList.size())
    {
        ITF_ASSERT_MSG(bfalse, "All children already launched");
        return;
    }
    
    if (m_finalIndex!=U32_INVALID && m_nextLaunchIndex>=m_finalIndex)
    {
        // if final, launch everything left
        while (canLaunch())
        {
            launchOne();
        }
    }
    else
    {
        // otherwise launch only one child
        launchOne();
    }
}

//------------------------------------------------------------------------------
Vec3d Ray_ChildLaunchComponent::getLaunchPos() const
{
    Vec3d launchPos = m_actor->getPos();
    if (m_animLightComponent && getTemplate()->getLaunchPolyline().isValid())
    {
        u32 pointCount = 0;
        const Vec2d* points = m_animLightComponent->getCurrentPolylinePointBuffer(
            getTemplate()->getLaunchPolyline(), &pointCount);
        if (points && pointCount)
        {
            launchPos = points[0].to3d(launchPos.m_z);
        }
    }
    return launchPos;
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::launchOne()
{
    // launch one child
    ObjectPath path = m_childrenList[m_nextLaunchIndex++];
    bbool launchOk = bfalse;
    Pickable* childPickable = path.getIsAbsolute()
        ? SceneObjectPathUtils::getObjectFromAbsolutePath(path)
        : SceneObjectPathUtils::getObjectFromRelativePath(m_actor, path);
    if (childPickable)
    {
        if (Actor* childActor = childPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
        {
            // check if launchable
            Ray_EventQueryChildLaunch query;
            childActor->onEvent(&query);

            // *enable* child and launch
            if (query.canLaunch())
            {
                childActor->enable();

                // actually launch
                Ray_EventChildLaunch launch(getLaunchPos());
                launch.setSender(m_actor->getRef());
                childActor->onEvent(&launch);
                launchOk = btrue;
            }
        }
    }

    // assert if something's wrong with the launch
    ITF_ASSERT_MSG(launchOk, "Couldn't launch a child");
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::processLink(EventOnLink* link)
{
#ifdef ITF_SUPPORT_EDITOR // only to ease editing

    // no need to update, children not collected yet
    if (!m_childrenCollected)
    {
        return;
    }

    // re-enable potentially disabled actors
    ChildrenList::iterator it;
    for (it = m_childrenList.begin(); it != m_childrenList.end(); ++it)
    {
        Pickable* childPickable = it->getIsAbsolute()
            ? SceneObjectPathUtils::getObjectFromAbsolutePath(*it)
            : SceneObjectPathUtils::getObjectFromRelativePath(m_actor, *it);
        if (childPickable)
        {
            if (Actor* childActor = childPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
            {
                // check if has a launch behavior
                Ray_EventQueryChildLaunch query;
                childActor->onEvent(&query);
                if (query.canLaunch())
                {
                    // *re-enable* child
                    childActor->enable();
                }
            }
        }
    }

    // brute force, just re-collect children children on the fly
    m_childrenList.clear();
    collectChildren();
    
#endif // ITF_SUPPORT_EDITOR
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::collectChildren()
{
    // guard, we can get there even if validation failed
    if (!m_linkComponent)
    {
        return;
    }

    // collect children that have a launch behavior and disable them
    const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
    ChildrenList finalChildrenList;
    for (LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it)
    {
        const ObjectPath& objPath = it->getPath();
        Pickable* childPickable = objPath.getIsAbsolute()
            ? SceneObjectPathUtils::getObjectFromAbsolutePath(objPath)
            : SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objPath);
        if (childPickable)
        {
            if (Actor* childActor = childPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
            {
                // check if has a launch behavior
                Ray_EventQueryChildLaunch query;
                childActor->onEvent(&query);

                // if found, add to our list, and *disable*, we don't wan't the
                // children to start acting normally if, for instance, a player
                // hits them
                if (query.canLaunch())
                {
                    bool isFinal;
                    if (it->getTagValue(ITF_GET_STRINGID_CRC(final,3571482722), isFinal) && isFinal)
                    {
                        // tagged "final", will get launched at the end
                        finalChildrenList.push_back(objPath);
                    }
                    else
                    {
                        // not tagged, will get launched in order
                        m_childrenList.push_back(objPath);
                    }
                }
            }
        }
    }
    
    // append final children at the end
    if (!finalChildrenList.empty())
    {
        m_finalIndex = m_childrenList.size();
        m_childrenList.insert(
            m_childrenList.end(),
            finalChildrenList.begin(),
            finalChildrenList.end());
    }
}

//------------------------------------------------------------------------------
bbool Ray_ChildLaunchComponent::needsHintFx() const
{
    return (m_hintFxEnabled && !m_hintFxRuntimeDisabled && canLaunch());
}

//------------------------------------------------------------------------------
void Ray_ChildLaunchComponent::updateHintFx(f32 _dt)
{
    // skip if not set
    if (getTemplate()->getHintFxPath().isEmpty())
    {
        return;
    }

    Actor* hintFxActor = m_hintFxActorRef.getActor();

    // create hint fx when enabled and there's children to launch
    if (needsHintFx() && !hintFxActor)
    {
        hintFxActor = m_hintFxGenerator.getSpawnee(m_actor->getScene(), getLaunchPos());
        ITF_WARNING(m_actor, hintFxActor, "Can't spawn hint fx");
        if (hintFxActor)
        {
            m_hintFxActorRef = hintFxActor->getRef();
            m_actor->getBinding()->bindChild(m_hintFxActorRef);
        }
    }
    // destroy hint fx if disabled or no more children to launch
    else if (!needsHintFx() && hintFxActor)
    {
        if (getTemplate()->getHintFxInstantDestroy())
        {
            hintFxActor->requestDestruction();
        }
        else
        {
            EventDie die;
            hintFxActor->onEvent(&die);
        }
        hintFxActor = NULL;
        m_actor->getBinding()->unbindChild(m_hintFxActorRef);
        m_hintFxActorRef.invalidate();
    }

    // move hint fx with platform
    if (hintFxActor)
    {
        hintFxActor->setPos(getLaunchPos());
    }
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_ChildLaunchComponent::drawEdit(class ActorDrawEditInterface* drawInterface, u32 _flags) const
{
    Super::drawEdit(drawInterface, _flags);

    // debug draw children launch rank
    u32 count = m_childrenList.size();
    for (u32 i=0; i<count; ++i)
    {
        ObjectPath path = m_childrenList[i];
        Pickable* childPickable = path.getIsAbsolute()
            ? SceneObjectPathUtils::getObjectFromAbsolutePath(path)
            : SceneObjectPathUtils::getObjectFromRelativePath(m_actor, path);
        if (childPickable)
        {
            Vec3d pos = m_actor->getPos() + 0.5f * (
                childPickable->getPos() - m_actor->getPos());
            if (m_finalIndex!=U32_INVALID && i>=m_finalIndex)
            {
                DebugDraw::text(pos, Color::white(), "final");
            }
            else
            {
                DebugDraw::text(pos, Color::white(), "%d", i);
            }
        }
    }
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ChildLaunchComponent_Template)
BEGIN_SERIALIZATION(Ray_ChildLaunchComponent_Template)
    SERIALIZE_MEMBER("disableAfterLaunch", m_disableAfterLaunch);
    SERIALIZE_MEMBER("launchPolyline", m_launchPolyline);
    SERIALIZE_MEMBER("hintFxPath", m_hintFxPath);
    SERIALIZE_MEMBER("hintFxInstantDestroy", m_hintFxInstantDestroy);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_ChildLaunchComponent_Template::Ray_ChildLaunchComponent_Template()
    : Super()
    , m_disableAfterLaunch(bfalse)
    , m_launchPolyline()
    , m_hintFxPath()
    , m_hintFxInstantDestroy(btrue)
{
}

//------------------------------------------------------------------------------
Ray_ChildLaunchComponent_Template::~Ray_ChildLaunchComponent_Template()
{
}

} // namespace ITF
