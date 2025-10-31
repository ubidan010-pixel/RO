#include "precompiled_gameplay.h"

#ifndef _ITF_ACTORSPAWNCOMPONENT_H_
#include "gameplay/Components/Common/ActorSpawnComponent.h"
#endif //_ITF_ACTORSPAWNCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_KEEPALIVECOMPONENT_H_
#include "engine/actors/components/KeepAlivecomponent.h"
#endif //_ITF_KEEPALIVECOMPONENT_H_

#ifdef ITF_IPAD
#include <string>
#endif // ITF_IPAD

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(ActorSpawnComponent_Template)
BEGIN_SERIALIZATION(ActorSpawnComponent_Template)
    SERIALIZE_MEMBER("spawnDelayed",m_spawnDelayed);
    SERIALIZE_CONTAINER_OBJECT("spawnActors",m_spawnList);
END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(ActorSpawnComponent_Template,SpawnData)
    SERIALIZE_MEMBER("actorLua",m_spawnActorLua);
    SERIALIZE_MEMBER("spawnActorBoneName",m_boneName);
    SERIALIZE_MEMBER("polyline",m_polylineRef);
    SERIALIZE_MEMBER("polylinePoint",m_polylinePointRef);
    SERIALIZE_MEMBER("useParentScale",m_useParentScale);
    SERIALIZE_MEMBER("useParentFlip",m_useParentFlip);
    SERIALIZE_MEMBER("useParentAngle",m_useParentAngle);
    SERIALIZE_MEMBER("offset",m_offset);
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(ActorSpawnComponent)

BEGIN_SERIALIZATION_CHILD(ActorSpawnComponent)
END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(ActorSpawnComponent,SpawnData)
END_SERIALIZATION()


ActorSpawnComponent::ActorSpawnComponent()
: Super()
, m_anim(NULL)
, m_alreadySpawned(bfalse)
, m_spawnDelayed(bfalse)
{
}

ActorSpawnComponent::~ActorSpawnComponent()
{
    clear();
}

void ActorSpawnComponent::clear( bbool _direct /*= bfalse*/  )
{
    m_alreadySpawned = bfalse;

    for ( SpawnList::iterator it = m_spawnList.begin(); it != m_spawnList.end(); ++it )
    {
        SpawnData& spawnData = *it;

        if ( spawnData.m_spawnRef != ITF_INVALID_OBJREF )
        {
            Actor* pObj = static_cast<Actor*>(GETOBJECT(spawnData.m_spawnRef));
            //ITF_ASSERT_MSG(pObj, "A spawned object is deleted too soon");             //<=== the actor is spawned then a the restart and actors spawned destruction it could happen
            if (pObj)
            {
                if ( !_direct && pObj->getComponentFromStaticClassCRC(ITF_GET_STRINGID_CRC(KeepAliveComponent,4149685283)))
                {
                    pObj->getBinding()->unbindFromParent();

                    EventDie eventDie;
                    pObj->onEvent(&eventDie);
                } else
                {
                    //m_actor->getScene()->unregisterActor(pObj);
                    pObj->requestDestruction();
                }
            }
        }
    }
    
    m_spawnList.clear();
}

void ActorSpawnComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_anim = m_actor->GetComponent<AnimLightComponent>();
    m_spawnList.resize( getTemplate()->getSpawnList().size() );

    for ( ActorSpawnComponent_Template::SpawnList::const_iterator it = getTemplate()->getSpawnList().begin(); it != getTemplate()->getSpawnList().end(); ++it )
    {
        ITF_WARNING_PARAM(GetActor(), "spawnData::m_spawnActorLua", !it->m_spawnActorLua.isEmpty(), "[ActorSpawnComponent] No actor specified for spawning in actor '%s'", GetActor()->getLua().getString8().cStr());
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventForceOnActor,267634505),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventUndelaySpawn,2219542589),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874),this);
    
    m_spawnDelayed = getTemplate()->getSpawnDelayed();
}

void ActorSpawnComponent::onBecomeActive()
{
    Super::onBecomeActive();
    
    if (!m_spawnDelayed && !m_alreadySpawned)
    {
        spawn();
        m_alreadySpawned = btrue;
    }

    Scene* scene = m_actor->getScene();
    bbool alwaysActive = scene->getAlwaysActiveActors().find(m_actor) != -1;

    for ( SpawnList::iterator it = m_spawnList.begin(); it != m_spawnList.end(); ++it )
    {
        SpawnData& spawnData = *it;
        Actor* actor = AIUtils::getActor(spawnData.m_spawnRef);

        if ( actor )
        {
            actor->setAllowedCell(m_actor->isAllowedForCell());
            if (alwaysActive && scene->getAlwaysActiveActors().find(actor) == -1)
            {
                scene->setAlwaysActiveActor(actor, btrue);
            }
            
            if ( m_actor->isEnabled() )
            {
                actor->enable();
            }
            else
            {
                actor->disable();
            }
        }
    }
}

void ActorSpawnComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    for ( SpawnList::iterator it = m_spawnList.begin(); it != m_spawnList.end(); ++it )
    {
        SpawnData& spawnData = *it;
        Actor* actor = AIUtils::getActor(spawnData.m_spawnRef);

        if ( actor )
        {
            actor->setAllowedCell(m_actor->isAllowedForCell());

            if ( m_actor->isEnabled() )
            {
                actor->enable();
            }
            else
            {
                actor->disable();
            }
        }
    }
}

void ActorSpawnComponent::onResourceReady()
{
    Super::onResourceReady();

    SpawnList::iterator it = m_spawnList.begin();
    ActorSpawnComponent_Template::SpawnList::const_iterator itTemplate = getTemplate()->getSpawnList().begin();

    for ( ; it != m_spawnList.end() && itTemplate != getTemplate()->getSpawnList().end(); ++it, ++itTemplate )
    {
        SpawnData& spawnData = *it;
        const ActorSpawnComponent_Template::SpawnData& spawnDataTemplate = *itTemplate;

        ITF_WARNING_PARAM(GetActor(), "spawnData::m_spawnActorLua", !spawnDataTemplate.m_spawnActorLua.isEmpty(), "[ActorSpawnComponent] No actor specified for spawning in actor '%s'", GetActor()->getLua().getString8().cStr());

        if ( spawnDataTemplate.m_boneName.size() )
        {
            readBoneInfo(spawnDataTemplate.m_boneName,spawnData);
        }
    }
}

void ActorSpawnComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);
    
    if (!m_spawnDelayed && !m_alreadySpawned)
    {
        spawn();
        m_alreadySpawned = btrue;
    }

    SpawnList::iterator it = m_spawnList.begin();
    ActorSpawnComponent_Template::SpawnList::const_iterator itTemplate = getTemplate()->getSpawnList().begin();

    for ( ; it != m_spawnList.end(); ++it )
    {
        SpawnData& spawnData = *it;
        const ActorSpawnComponent_Template::SpawnData& spawnDataTemplate = *itTemplate;

        updateSpawn(
            spawnDataTemplate.m_polylineRef,
            spawnDataTemplate.m_polylinePointRef,
            spawnDataTemplate.m_useParentScale,
            spawnDataTemplate.m_useParentFlip,
            spawnDataTemplate.m_useParentAngle,
            spawnDataTemplate.m_offset,
            spawnData);
    }
}

void ActorSpawnComponent::spawn()
{
    ActorSpawnComponent_Template::SpawnList::const_iterator itTemplate = getTemplate()->getSpawnList().begin();
    SpawnList::iterator it = m_spawnList.begin();

    for ( ; it != m_spawnList.end() && itTemplate != getTemplate()->getSpawnList().end(); ++it, ++itTemplate )
    {
        SpawnData& spawnData = *it;
        const ActorSpawnComponent_Template::SpawnData& templateSpawnData = *itTemplate;

        if (templateSpawnData.m_spawnActorLua.isEmpty())
            continue;

        // spawn the actors onActorLoaded to add them to our resourceGroup
        spawnActor(
            templateSpawnData.m_spawnActorLua,
            templateSpawnData.m_useParentScale,
            templateSpawnData.m_useParentFlip,
            templateSpawnData.m_offset,
            spawnData);
    }
}

void ActorSpawnComponent::spawnActor( const Path& _lua, bbool _useParentScale, bbool _useParentFlip, const Vec3d& _offset, SpawnData& _spawn )
{
    Actor* actor = ACTORSMANAGER->spawnActor(
        m_actor->getPos() + _offset,
        m_actor->getScene(),
        _lua,
        btrue,
        NULL
          );

    if ( _useParentScale )
    {
        actor->setScale(m_actor->getScale());
    }

    if ( _useParentFlip )
    {
        actor->setStartFlipped( m_actor->isFlipped() );
        actor->setFlipped( m_actor->isFlipped() );
    }

    _spawn.m_spawnRef = actor->getRef();
    m_actor->getBinding()->bindChild(actor->getRef());
    actor->setSerializable(bfalse);
    actor->setIsSpawned( btrue );
    actor->setUpdateType( m_actor->getUpdateType() );
    
    // if world doesn't support dynamic loading, do it manually
    if (actor->getScene()->getWorld()->getIgnoreCells())
    {
        actor->loadResources();
    }
}

void ActorSpawnComponent::readBoneInfo( const ITF_STDSTRING& _boneName, SpawnData& _spawn )
{
    if (!m_anim)
        return;

    u32 length = u32(_boneName.length());

    if ( _boneName[length-1] == '*')
    {
        StringID boneName(StringID::StrToCRC(_boneName.c_str(), u32(_boneName.length()-1)));

        _spawn.m_boneIndex = m_anim->getBoneIndex(boneName);
        _spawn.m_boneEnd = btrue;
    }
    else
    {
        _spawn.m_boneIndex = m_anim->getBoneIndex(_boneName.c_str());
        _spawn.m_boneEnd = bfalse;
    }
}

void ActorSpawnComponent::updateSpawn( const StringID& _polylineRef, const StringID& _polylinePointRef,
                                       bbool _useParentScale, bbool _useParentFlip, bbool _useParentAngle, const Vec3d& _offset, SpawnData& _spawn )
{
    Actor* actor = AIUtils::getActor(_spawn.m_spawnRef);

    if ( actor )
    {
        if ( actor->getBinding()->getParent() != m_actor->getRef() )
        {
            m_actor->getBinding()->bindChild(actor->getRef());
        }

        if ( m_anim && _spawn.m_boneIndex != U32_INVALID )
        {
            updateSpawnFromBone(actor,_spawn.m_boneIndex,_spawn.m_boneEnd, _useParentAngle, _offset);
        }
        else if ( m_anim && _polylinePointRef.isValid() &&
                            _polylinePointRef.isValid() )
        {
            updateSpawnFromPolyline(actor,_polylineRef,_polylinePointRef, _useParentAngle, _offset);
        }
        else
        {
            actor->setPos(m_actor->getPos() + _offset);
            if (_useParentAngle)
            {
                actor->setAngle(m_actor->getAngle());
            }
        }

        if ( _useParentScale )
        {
            actor->setScale(m_actor->getScale());
        }

        if ( _useParentFlip )
        {
            actor->setStartFlipped( m_actor->isFlipped() );
            actor->setFlipped( m_actor->isFlipped() );
        }

        GetActor()->growRelativeAABBFromAbsolute(actor->getAABB());
    }
}

void ActorSpawnComponent::updateSpawnFromBone( Actor* _actor, u32 _boneIndex, bbool _boneEnd, bbool _useParentAngle, const Vec3d& _offset )
{
    if (!m_anim)
        return;

    AnimBoneDyn* bone = m_anim->getBone(_boneIndex);

    ITF_ASSERT_MSG(bone, "Missing bone or invalid index");

    if(bone)
    {
        Vec2d pos;

        if ( _boneEnd )
        {
            pos = bone->m_PosEnd;
        }
        else
        {
            pos = bone->m_Pos;
        }

        f32 angle = bone->getAngle();

        if (m_actor->isFlipped())
            angle += MTH_PI;

        _actor->setPos(pos.to3d(m_actor->getPos().m_z) + _offset);
        if (_useParentAngle)
        {
            _actor->setAngle(angle);
        }
    }
}

void ActorSpawnComponent::updateSpawnFromPolyline( Actor* _actor, const StringID& _polyID, const StringID& _pointID, bbool _useParentAngle, const Vec3d& _offset )
{
    if (!m_anim)
        return;

    AnimPolyline* pPolyline = m_anim->getCurrentPolyline(_polyID);
    const Vec2d* points = m_anim->getCurrentPolylinePointBuffer(_polyID);

    if (pPolyline)
    {
        u32 numPoints = pPolyline->m_PolylinePointList.size();
        u32 pointIndex = pPolyline->m_NameToPolylinePoint.find(_pointID);

        if ( pointIndex < numPoints )
        {
            const AnimPolylinePoint& pointInfo = pPolyline->m_PolylinePointList[pointIndex];

            if ( pointInfo.m_BonePtr )
            {
                AnimBoneDyn* bone = m_anim->getBone(pointInfo.m_BonePtr->getIndex());

                if ( bone )
                {
                    Vec2d pos = points[pointIndex];
                    f32 angle = bone->getAngle();

                    if (m_actor->isFlipped())
                        angle += MTH_PI;

                    _actor->setPos(pos.to3d(m_actor->getPos().m_z) + _offset);
                    if (_useParentAngle)
                    {
                        _actor->setAngle(angle);
                    }
                }
            }
        }
    }
}

void ActorSpawnComponent::onEvent( Event* _event )
{
    if ( EventForceOnActor* physBodyForce = _event->DynamicCast<EventForceOnActor>(ITF_GET_STRINGID_CRC(EventForceOnActor,267634505)) )
    {
        processForceOnActor(physBodyForce);
    }
    else if ( EventAnimUpdated* animUpdated = _event->DynamicCast<EventAnimUpdated>(ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305)) )
    {
        updateAllSpawn();
    }
    else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventUndelaySpawn,2219542589)) )
    {
        m_spawnDelayed = bfalse;
    }
    else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874)) )
    {
        sendEventToSpawns(_event);
    }
}

void ActorSpawnComponent::updateAllSpawn()
{
    SpawnList::iterator it = m_spawnList.begin();
    ActorSpawnComponent_Template::SpawnList::const_iterator itTemplate = getTemplate()->getSpawnList().begin();

    for ( ; it != m_spawnList.end() && itTemplate != getTemplate()->getSpawnList().end(); ++it, ++itTemplate )
    {
        SpawnData& spawnData = *it;
        const ActorSpawnComponent_Template::SpawnData& spawnDataTemplate = *itTemplate;

        updateSpawn(
            spawnDataTemplate.m_polylineRef,
            spawnDataTemplate.m_polylinePointRef,
            spawnDataTemplate.m_useParentScale,
            spawnDataTemplate.m_useParentFlip,
            spawnDataTemplate.m_useParentAngle,
            spawnDataTemplate.m_offset,
            spawnData);
    }
}

void ActorSpawnComponent::processForceOnActor( EventForceOnActor* _forceInfo )
{
    ActorSpawnComponent_Template::SpawnList::const_iterator itTemplate = getTemplate()->getSpawnList().begin();
    SpawnList::const_iterator it = m_spawnList.begin();

    for ( ; it != m_spawnList.end() && itTemplate != getTemplate()->getSpawnList().end(); ++it, ++itTemplate )
    {
        const SpawnData& spawn = *it;

        if ( spawn.m_spawnRef == _forceInfo->getActor() )
        {
            Actor* spawnedActor = AIUtils::getActor(spawn.m_spawnRef);

            if ( spawnedActor )
            {
                const ActorSpawnComponent_Template::SpawnData& spawnDataTemplate = *itTemplate;

                if ( spawnDataTemplate.m_polylinePointRef.isValid() && spawnDataTemplate.m_polylineRef.isValid() )
                {
                    EventForceTransfer forceTransfer(spawnDataTemplate.m_polylineRef,spawnDataTemplate.m_polylinePointRef,_forceInfo->getPos(),_forceInfo->getForce());
                    m_actor->onEvent(&forceTransfer);
                }
            }
            break;
        }
    }
}

void ActorSpawnComponent::sendEventToSpawns( Event* _event )
{
    for ( SpawnList::const_iterator it = m_spawnList.begin(); it != m_spawnList.end(); ++it )
    {
        const SpawnData& spawn = *it;

        if ( Actor* spawnedActor = AIUtils::getActor(spawn.m_spawnRef) )
        {
            spawnedActor->onEvent(_event);
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
void ActorSpawnComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    ActorSpawnComponent_Template::SpawnList::const_iterator itTemplate = getTemplate()->getSpawnList().begin();
    SpawnList::iterator it = m_spawnList.begin();

    for ( ; it != m_spawnList.end() && itTemplate != getTemplate()->getSpawnList().end(); ++it, ++itTemplate )
    {
        SpawnData& spawnData = *it;
        const ActorSpawnComponent_Template::SpawnData& spawnDataTemplate = *itTemplate;

        updateSpawn(
            spawnDataTemplate.m_polylineRef,
            spawnDataTemplate.m_polylinePointRef,
            spawnDataTemplate.m_useParentScale,
            spawnDataTemplate.m_useParentFlip,
            spawnDataTemplate.m_useParentAngle,
            spawnDataTemplate.m_offset,
            spawnData);
    }
}
#endif // ITF_SUPPORT_EDITOR

} // namespace
