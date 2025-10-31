#include "precompiled_gameplay.h"

#ifndef _ITF_AABBPREFETCH_COMPONENT_H_
#include "gameplay/components/misc/AABBPrefetchComponent.h"
#endif //_ITF_AABBPREFETCH_COMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(AABBPrefetchComponent)

BEGIN_SERIALIZATION_CHILD(AABBPrefetchComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(AABBPrefetchComponent)
/*
This is not good anymore because a stargate can be triggered by the stargate, without detectors, or even with detectors
if(Actor* owner = GetActor())
{
    const LinkComponent* pLinkComponent = owner->GetComponent<const LinkComponent>();
    VALIDATE_COMPONENT_PARAM("", pLinkComponent, "AABBPrefetchComponent require LinkComponent");

    bbool validChildFound = bfalse;
    const LinkComponent::ChildrenList& children = pLinkComponent->getChildren();
    for(u32 i = 0; i < children.size(); ++i)
    {
        const ObjectPath& path = children[i].getPath();
        const bbool isAbsolute = path.getIsAbsolute();

        const Pickable* pChild = NULL;
        if(isAbsolute)
            pChild = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
        else
            pChild = SceneObjectPathUtils::getObjectFromRelativePath(owner, path);

        if(pChild && pChild->getObjectType() == BaseObject::eActor)
        {
            if(static_cast<const Actor*>(pChild)->GetComponent<const PlayerDetectorComponent>())
            {
                validChildFound = btrue;
                break;
            }
        }
    }
    VALIDATE_COMPONENT_PARAM("", validChildFound, "Missing link to player detector");
}
*/
END_VALIDATE_COMPONENT()

AABBPrefetchComponent::AABBPrefetchComponent()
: Super()
, m_view(NULL)
, m_ready(bfalse)
, m_forceStart(bfalse)
, m_failSafeCounter(0.f)
, m_forceUpdateByChild(bfalse)
, m_forceUpdateByChild_Frame(0)
{
}

AABBPrefetchComponent::~AABBPrefetchComponent()
{
    if(isPrefetching())
        stop();
}

bbool AABBPrefetchComponent::prefetchComplete() const
{
    static const f32 s_failSafeMaxTime = 5.f;

    ITF_ASSERT_MSG(m_ready || isPrefetching(), "prefetch wasn't started! PREFETCH WILL NEVER BE READY!");
    return m_ready || m_failSafeCounter >= s_failSafeMaxTime;
}

void AABBPrefetchComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);

    if(Pickable::HotReloadType_Checkpoint == _hotReload)
        if(m_view)
            stop();
}

void AABBPrefetchComponent::onActorRemovedFromScene( Scene* _pScene, bbool )
{
}

void AABBPrefetchComponent::onSceneActive()
{
    if(isPrefetching())
        m_view->setActive(btrue);
}

void AABBPrefetchComponent::onSceneInactive()
{
    if(isPrefetching())
        m_view->setActive(bfalse);
}

void AABBPrefetchComponent::onBecomeActive()
{
    if(!isUpdateForcedByChild())
    {
        if(ObjectRefList* parents = LINKMANAGER->getParents(m_actor->getRef()))
        {
            SafeArray<BaseObject*> resolvedRefs;
            ID_SERVER->getObjectListNoNull(*parents, resolvedRefs);
            for(u32 i = 0; i < resolvedRefs.size(); ++i)
            {
                Pickable* pObj = static_cast<Pickable*>(resolvedRefs[i]);

                if(pObj->getObjectType() == BaseObject::eActor)
                {
                    Actor* pActor = static_cast<Actor*>(pObj);

                    if(AABBPrefetchComponent* pAC = pActor->GetComponent<AABBPrefetchComponent>())
                        pAC->forceUpdateByChild(btrue);
                }
            }
        }
    }
}

void AABBPrefetchComponent::onBecomeInactive()
{
    if(!isUpdateForcedByChild())
    {
        if(ObjectRefList* parents = LINKMANAGER->getParents(m_actor->getRef()))
        {
            SafeArray<BaseObject*> resolvedRefs;
            ID_SERVER->getObjectListNoNull(*parents, resolvedRefs);
            for(u32 i = 0; i < resolvedRefs.size(); ++i)
            {
                Pickable* pObj = static_cast<Pickable*>(resolvedRefs[i]);

                if(pObj->getObjectType() == BaseObject::eActor)
                {
                    Actor* pActor = static_cast<Actor*>(pObj);

                    if(AABBPrefetchComponent* pAC = pActor->GetComponent<AABBPrefetchComponent>())
                        pAC->forceUpdateByChild(bfalse);
                }
            }
        }
    }
}

void AABBPrefetchComponent::Update( f32 _deltaTime )
{
    // Must ensure that parents are updated
    if(!isUpdateForcedByChild() && getForceUpdateFrame() != (CURRENTFRAME - 1))
    {
        if(ObjectRefList* parents = LINKMANAGER->getParents(m_actor->getRef()))
        {
            static SafeArray<BaseObject*> resolvedRefs;
            resolvedRefs.clear();

            ID_SERVER->getObjectListNoNull(*parents, resolvedRefs);
            for(u32 i = 0; i < resolvedRefs.size(); ++i)
            {
                Pickable* pObj = static_cast<Pickable*>(resolvedRefs[i]);

                if(pObj->getObjectType() == BaseObject::eActor)
                {
                    Actor* pActor = static_cast<Actor*>(pObj);

                    if(AABBPrefetchComponent* pAC = pActor->GetComponent<AABBPrefetchComponent>())
                        pAC->forceUpdateByChild(btrue);
                }
            }
        }
    }

    Actor* owner = GetActor();
    const LinkComponent* pLinkComponent = owner->GetComponent<const LinkComponent>();

    bbool bAtLeastOnePlayerDetected = m_forceStart;

    if ( !bAtLeastOnePlayerDetected )
    {
        const LinkComponent::ChildrenList& children = pLinkComponent->getChildren();
        for(u32 i = 0; i < children.size(); ++i)
        {
            const ObjectPath& path = children[i].getPath();
            const bbool isAbsolute = path.getIsAbsolute();

            Pickable* pChild = NULL;
            if(isAbsolute)
                pChild = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
            else
                pChild = SceneObjectPathUtils::getObjectFromRelativePath(owner, path);

            if(pChild && pChild->getObjectType() == BaseObject::eActor && pChild->isActive())
            {
                if(const ShapeDetectorComponent* pShapeDetectorComponent = static_cast<const Actor*>(pChild)->GetComponent<const ShapeDetectorComponent>())
                {
                    if(!pShapeDetectorComponent->isDisabled())
                    {
                        const ActorRefList& detected = pShapeDetectorComponent->getActorsInside();

                        if(detected.size() > 0)
                        {
                            bAtLeastOnePlayerDetected = btrue;
                            break;
                        }
                    }
                }
            }
        }
    }

    if(bAtLeastOnePlayerDetected && !isPrefetching())
    {
        start();
    }
    else if (!bAtLeastOnePlayerDetected && isPrefetching())
    {
        stop();
    }

    if(isPrefetching())
    {
        if(!prefetchComplete())
        {
            World* pWorld = getWorld();
            m_ready = pWorld->isPhysicalReady(*m_view);
            m_failSafeCounter += _deltaTime;
            
            if(m_ready)
            {
                LOG_OLIV("[AABBPrefetchComponent] Ready at frame %u", CURRENTFRAME);
            }
        }
    }
}

void AABBPrefetchComponent::start()
{
    LOG_OLIV("[AABBPrefetchComponent] Start at frame %u", CURRENTFRAME);

    ITF_ASSERT_CRASH(!isPrefetching(), "Can't start twice!");

    m_ready = bfalse;
    m_view = View::createView("AABBPrefetchComponent", bfalse, btrue);
    const f32 deltaX = CAMERA->getDeltaX();
    const f32 deltaY = CAMERA->getDeltaY();
    AABB targetAABB = m_actor->getAABB();
    targetAABB.projectTo2D(deltaX, deltaY, m_actor->getDepth());
    m_view->setTargetAABB(targetAABB);
    m_view->setActive(btrue);
    m_failSafeCounter = 0.f;

    // Make sure the update is called
    if(!isUpdateForcedByChild())
        AIUtils::setAlwaysActive(m_actor, btrue);
}

void AABBPrefetchComponent::stop()
{
    LOG_OLIV("[AABBPrefetchComponent] Stop at frame %u", CURRENTFRAME);

    ITF_ASSERT_CRASH(isPrefetching(), "Can't stop if not started!");

    m_view->setActive(bfalse);
    View::deleteView(m_view);
    m_view = NULL;
    m_failSafeCounter = 0.f;

    if(!isUpdateForcedByChild())
        AIUtils::setAlwaysActive(m_actor, bfalse);
}

void AABBPrefetchComponent::forceStart( bbool _val )
{
    m_forceStart = _val;
    
    // Set always active while prefetch is activated
    if(!isUpdateForcedByChild())
        AIUtils::setAlwaysActive(m_actor, _val);
}

void AABBPrefetchComponent::forceUpdateByChild( bbool _b )
{
    m_forceUpdateByChild = _b;
    AIUtils::setAlwaysActive(m_actor, _b);

    if(_b)
        m_forceUpdateByChild_Frame = CURRENTFRAME;
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(AABBPrefetchComponent_Template)

BEGIN_SERIALIZATION(AABBPrefetchComponent_Template)
END_SERIALIZATION()

}