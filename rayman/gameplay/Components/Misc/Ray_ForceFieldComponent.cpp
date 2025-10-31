#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FORCEFIELDCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ForceFieldComponent.h"
#endif //_ITF_RAY_FORCEFIELDCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

#define MUSIC_INPUT_FORCE_FIELD ITF_GET_STRINGID_CRC(ForceField, 1883637587)

    IMPLEMENT_OBJECT_RTTI(Ray_ForceFieldComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ForceFieldComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(Ray_ForceFieldComponent)
    VALIDATE_COMPONENT_PARAM("Ray_ForceFieldComponent::LinkComponent", m_linkComponent, "You need an LinkComponent");
    END_VALIDATE_COMPONENT()

        Ray_ForceFieldComponent::Ray_ForceFieldComponent()
        : m_linkComponent(NULL)
        , m_locked(btrue)
        , m_firstUpdate(btrue)
        , m_fxHandle(U32_INVALID)
        , m_fxControllerComponent(NULL)
        , m_timer(0.0f)
        , m_animComponent(NULL)
    {
    }

    void Ray_ForceFieldComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153),this);
        clear();

        const SafeArray<Ray_ForceFieldComponent_Template::LinkEvent_Template> & linkEvents = getTemplate()->getLinkEvents();
        for (u32 i = 0; i < linkEvents.size(); ++i)
        {
            const Ray_ForceFieldComponent_Template::LinkEvent_Template & linkEventTemplate = linkEvents[i];
            ITF_MAP<StringID, LinkEvent>::iterator it = m_linkEvents.find(linkEventTemplate.m_tag);
            if (it == m_linkEvents.end())
            {
                LinkEvent & linkEvent = m_linkEvents[linkEventTemplate.m_tag];
                if (linkEventTemplate.m_onActivateForceFieldEvent)
                {
                    linkEvent.m_onActivateForceFieldEvent = static_cast<Event*>(linkEventTemplate.m_onActivateForceFieldEvent->CreateNewObject());
                    BinaryClone(const_cast<Event*>(linkEventTemplate.m_onActivateForceFieldEvent),linkEvent.m_onActivateForceFieldEvent);
                }
                if (linkEventTemplate.m_onDeactivateForceFieldEvent)
                {
                    linkEvent.m_onDeactivateForceFieldEvent = static_cast<Event*>(linkEventTemplate.m_onDeactivateForceFieldEvent->CreateNewObject());
                    BinaryClone(const_cast<Event*>(linkEventTemplate.m_onDeactivateForceFieldEvent),linkEvent.m_onDeactivateForceFieldEvent);
                }
            }
        }

        if (!getTemplate()->getTextureFile().isEmpty())
        {
            m_texture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureFile());
        }

        m_firstUpdate = btrue;
        m_locksTmp.clear();
        getLocks(m_locksTmp);

        if (m_lockData.size() < m_locksTmp.size())
            m_lockData.resize(m_locksTmp.size());

        for (u32 i = 0; i < m_locksTmp.size(); ++i)
        {
            Actor * actor = m_locksTmp[i];
            LockData & lockData = m_lockData[i];
            lockData.m_circleOffset = Seeder::getSharedSeeder().GetFloat(-getTemplate()->getPatchCircleTangeantRotationFrequency(),getTemplate()->getPatchCircleTangeantRotationFrequency());
            lockData.m_targetOffset = Seeder::getSharedSeeder().GetFloat(-getTemplate()->getPatchTargetTangeantRotationFrequency(),getTemplate()->getPatchTargetTangeantRotationFrequency());
            lockData.m_target = actor->getRef();
            lockData.m_uvStart = Seeder::getSharedSeeder().GetFloat(0.0f,getTemplate()->getPatchTileLength());
        }

        m_enemyVacuumedList.clear();
    }

    Ray_ForceFieldComponent::~Ray_ForceFieldComponent()
    {
        if (m_texture.isValidResourceId())
        {
            m_actor->removeResource(m_texture);
            m_texture.invalidateResourceId();
        }
        clear();
    }


	//----------------------------------------------------------------------------------------------------------------------------
    void Ray_ForceFieldComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if (m_firstUpdate)
        {
            startLocked();
            m_firstUpdate = bfalse;
        }

        if (m_locked)
            updateLocked();
        else
            updateOpen();

        m_timer += _deltaTime;

        AIUtils::growAABB(m_linkComponent);

        for (ITF_VECTOR<LockData>::iterator it = m_lockData.begin(); it != m_lockData.end(); ++it)
        {
            LockData & lockData = *it;
            Actor * actor = lockData.m_target.getActor();
            if (!actor)
                continue;
            lockData.m_uvStart += getTemplate()->getPatchScrollSpeed();
        }
    }

    void Ray_ForceFieldComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (Ray_EventQueryAIData * query = _event->DynamicCast<Ray_EventQueryAIData>(ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000)))
        {
            query->setSoftCollisionRadius(getSoftCollisionRadius());
        }
        else if (Ray_EventQueryOpen * queryOpen = _event->DynamicCast<Ray_EventQueryOpen>(ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153)))
        {
            queryOpen->setOpen(!m_locked);
        }
    }

    f32 Ray_ForceFieldComponent::getSoftCollisionRadius() const
    {
        return m_actor->getScale().m_y * getTemplate()->getSoftCollisionRadius() * (m_locked?1.0f:0.0f);
    }

    void Ray_ForceFieldComponent::updateLocked()
    {   
        m_locksTmp.clear();
        getLocks(m_locksTmp);

        bbool locked = bfalse;
        for (u32 i = 0; i < m_locksTmp.size(); ++i)
        {
            // check vacuumed actors
            isEnemyVacuumed( m_locksTmp[i] );       

            if (isEnemyAlive(m_locksTmp[i]))
            {
                locked = btrue;
                //break; => removed to check all actor vacuuming state
            }
        }

        if (!locked)
        {
            startOpen();
            return;
        }

        updateSoftCollisions();
    }

    void Ray_ForceFieldComponent::updateOpen()
    {
        if (m_animComponent->isSubAnimFinished() && m_animComponent->getAlpha() > 0.0f)
            m_animComponent->setAlpha(0.0f);
    }

    void Ray_ForceFieldComponent::startOpen()
    {
        m_locked = bfalse;
        for (ITF_MAP<StringID, LinkEvent>::iterator it = m_linkEvents.begin(); it != m_linkEvents.end(); ++it)
        {
            LinkEvent & linkEvent = it->second;
            if (linkEvent.m_onDeactivateForceFieldEvent)
            {
                m_linkComponent->sendEventToChildren(linkEvent.m_onDeactivateForceFieldEvent,it->first);

                // Tell music manager that forcefield is deactivated
                MUSICMANAGER->setInput(MUSIC_INPUT_FORCE_FIELD, u32(0));
            }
        }

        AIUtils::disableCollision(m_actor);

        m_fxControllerComponent->stopFX(m_fxHandle);
        m_fxHandle = U32_INVALID;
        m_animComponent->setAnim(getTemplate()->getFadeAnim());
    }

    void Ray_ForceFieldComponent::updateSoftCollisions()
    {
        const f32 radius = getSoftCollisionRadius();
        if (radius < MTH_EPSILON)
        {
            return;
        }

        m_allActorsTmp.clear();


        const f32 exitSpeed = getTemplate()->getSoftCollisionExitSpeed();
        const f32 exitForce = getTemplate()->getSoftCollisionExitForce();
        const f32 detectionRadius = 2 * radius;

        PhysContactsContainer contacts;
        PhysShapeCircle circle;
        circle.setRadius(detectionRadius);
        PHYSWORLD->collidePhantoms(m_actor->get2DPos(), m_actor->get2DPos(), 0.0f,
            &circle, m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
            contacts);

        const u32 contactCount = contacts.size();

        for ( u32 i = 0; i < contactCount; i++ )
        {
            ActorRef otherActorRef = contacts[i].m_collidableUserData;

            if (otherActorRef == m_actor->getRef())
            {
                continue;
            }

            Actor* otherActor = otherActorRef.getActor();
            if (!otherActor)
            {
                continue;
            }

            if ((otherActor->get2DPos() - m_actor->get2DPos()).sqrnorm() > sqr(detectionRadius))
            {
                continue;
            }

            if (m_allActorsTmp.find(otherActor) != -1)
            {
                continue;
            }
            m_allActorsTmp.push_back(otherActor);


            const Vec2d force = Ray_AIUtils::getSoftCollForce(otherActor,m_actor->get2DPos(),radius,exitSpeed,exitForce);
            if (force != Vec2d::Zero)
            {
                EventAddForce addForce;
                addForce.setSender(m_actor->getRef());
                addForce.setForce(force);
                otherActor->onEvent(&addForce);
            }
        }
    }

    void Ray_ForceFieldComponent::clear()
    {

        for (ITF_MAP<StringID, LinkEvent>::iterator it = m_linkEvents.begin(); it != m_linkEvents.end(); ++it)
        {
            LinkEvent & linkEvent = it->second;
            SF_DEL(linkEvent.m_onActivateForceFieldEvent);
            SF_DEL(linkEvent.m_onDeactivateForceFieldEvent);
        }
        m_linkEvents.clear();

        m_lockData.clear();

    }

    void Ray_ForceFieldComponent::startLocked()
    {
        for (ITF_MAP<StringID, LinkEvent>::iterator it = m_linkEvents.begin(); it != m_linkEvents.end(); ++it)
        {
            LinkEvent & linkEvent = it->second;
            if (linkEvent.m_onActivateForceFieldEvent)
            {
                m_linkComponent->sendEventToChildren(linkEvent.m_onActivateForceFieldEvent,it->first);
                
                // Tell music manager that forcefield is activated
                MUSICMANAGER->setInput(MUSIC_INPUT_FORCE_FIELD, u32(1));
            }
        }
        m_fxHandle = m_fxControllerComponent->playFX(getTemplate()->getLockedFx());
        m_animComponent->setAlpha(1.0f);
        m_animComponent->setAnim(getTemplate()->getIdleAnim());

        AIUtils::enableCollision(m_actor);
    }

    bbool Ray_ForceFieldComponent::isEnemyAlive(Actor *_enemy)
    {
        if (!_enemy->isEnabled())
        {
            return bfalse;
        }

        if ( m_enemyVacuumedList.find( _enemy->getRef() ) >= 0 )
        {
            return bfalse;
        }

        if ( AIComponent* ai = _enemy->GetComponent<AIComponent>() )
        {
            return (ai->getHealth() > 0);
        }

        EventQueryIsDead query;
        query.setSender(m_actor->getRef());
        _enemy->onEvent(&query);
        if ( query.getIsDead() )
        {
            return bfalse;
        }
       
        return btrue;        
    }

    bbool Ray_ForceFieldComponent::isEnemyVacuumed(Actor *_enemy)
    {
        ITF_ASSERT( _enemy != NULL );
        if ( _enemy == NULL )
            return bfalse;

        if ( m_enemyVacuumedList.find( _enemy->getRef() ) >= 0 )
        {
            return btrue;
        }
        Ray_EventQueryHasBeenVacuumed vacuumedQuery;
        _enemy->onEvent( &vacuumedQuery );
        if ( vacuumedQuery.getHasBeenVacuumed() )
        {
            m_enemyVacuumedList.push_back( _enemy->getRef() );
        }

        return vacuumedQuery.getHasBeenVacuumed();
    }

    void Ray_ForceFieldComponent::Draw()
    {
        Super::Draw();

        if (!m_locked)
            return;

        const Vec2d pos = m_actor->get2DPos();

        for (ITF_VECTOR<LockData>::iterator it = m_lockData.begin(); it != m_lockData.end(); ++it)
        {
            LockData & lockData = *it;
            Actor * actor = lockData.m_target.getActor();
            if (!actor)
                continue;

            if (!isEnemyAlive(actor))
            {
                continue;
            }

            Vec2d dir = actor->get2DPos() - m_actor->get2DPos();
            dir.normalize();

            const Vec2d targetPos = actor->get2DPos();
            const f32 targetDist = (targetPos - pos).norm();

            const f32 radius = getSoftCollisionRadius();
            const f32 circlPointPct = Min(getTemplate()->getPatchMidPointPercent() * targetDist,getTemplate()->getPatchMidPointOffset()+radius);
            const Vec2d circlePoint = pos + dir * (circlPointPct);

            const Vec2d cP0 = pos + dir * getTemplate()->getPatchCenterOffset();
            const Vec2d cP3 = circlePoint;
            const f32 cAngle = f32_Sin((m_timer+ lockData.m_circleOffset)/getTemplate()->getPatchCircleTangeantRotationFrequency() ) * getTemplate()->getPatchCircleTangeantRotationOffset().Radians();
            const Vec2d cP1 = cP0 + dir.Rotate(cAngle) * 0.5f * (circlPointPct -getTemplate()->getPatchCenterOffset());
            const Vec2d cP2 = cP3 -dir.Rotate(-cAngle) * 0.5f * (circlPointPct -getTemplate()->getPatchCenterOffset());

            m_circleB.m_p0 = cP0;
            m_circleB.m_p1 = cP1;
            m_circleB.m_p2 = cP2;
            m_circleB.m_p3 = cP3;
            const f32 circlePatchLength = m_circleB.getLength(10);

            const f32 uvCircleEnd = (circlePatchLength / getTemplate()->getPatchTileLength()) + lockData.m_uvStart;

            Color colorA = getColorFactor();
            colorA.m_a = 0.f;
            Color colorB = getColorFactor();

            f32 tesselationRatio = f32_Inv(getTemplate()->getTesselationLength());

            {
                BezierPatchParams params;
                params.m_p0 = cP0;
                params.m_p1 = cP1;
                params.m_p2 = cP2;
                params.m_p3 = cP3;
                params.m_startUV.m_x = lockData.m_uvStart;
                params.m_endUV.m_x = uvCircleEnd;
                params.m_startWidth = getTemplate()->getPatchCenterWidth();
                params.m_endWidth = getTemplate()->getPatchCircleWidth();
                params.m_z = m_actor->getDepth();
                params.m_texture = (Texture*)m_texture.getResource();
                params.m_startColor = colorA;
                params.m_endColor = colorB;
                params.m_fogColor = getColorFog();
                params.m_blendMode = getTemplate()->getBlendMode();
                params.m_vdivToDraw = (u32)(circlePatchLength * tesselationRatio);
                lockData.m_circlePatch.drawPatch(params);
            }

            const Vec2d tP0 = circlePoint;
            const Vec2d tP3 = actor->get2DPos();
            const f32 tAngle = f32_Sin((m_timer + lockData.m_targetOffset)/getTemplate()->getPatchTargetTangeantRotationFrequency()) * getTemplate()->getPatchTargetTangeantRotationOffset().Radians();
            const Vec2d tP1 = tP0 + dir.Rotate(-cAngle) * 0.5f* (targetDist - radius);
            const Vec2d tP2 = tP3 -dir.Rotate(-tAngle) * 0.5f * radius;;

            m_targetB.m_p0 = tP0;
            m_targetB.m_p1 = tP1;
            m_targetB.m_p2 = tP2;
            m_targetB.m_p3 = tP3;

            const f32 targetPatchLength = m_targetB.getLength(10);

            const f32 uvTargetEnd = uvCircleEnd + (targetPatchLength / getTemplate()->getPatchTileLength());

            {
                BezierPatchParams params;
                params.m_p0 = tP0;
                params.m_p1 = tP1;
                params.m_p2 = tP2;
                params.m_p3 = tP3;
                params.m_startUV.m_x = uvCircleEnd;
                params.m_endUV.m_x = uvTargetEnd;
                params.m_startWidth = getTemplate()->getPatchCircleWidth();
                params.m_endWidth = getTemplate()->getPatchTargetWidth();
                params.m_z = m_actor->getDepth();
                params.m_texture = (Texture*)m_texture.getResource();
                params.m_startColor = colorB;
                params.m_endColor = colorA;
                params.m_fogColor = getColorFog();
                params.m_blendMode = getTemplate()->getBlendMode();
                params.m_vdivToDraw = (u32)(targetPatchLength * tesselationRatio);

                lockData.m_targetPatch.drawPatch(params);
            }

        }

    }

    void Ray_ForceFieldComponent::getLocks(SafeArray<Actor*> & _locks)
    {
        const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
        u32 numChildren = children.size();

        for ( u32 i = 0; i < numChildren; i++ )
        {
            Pickable* child;
            const ObjectPath& childPath = children[i].getPath();

            if (children[i].hasTag(ITF_GET_STRINGID_CRC(Lock, 3199059628)))
            {
                if ( childPath.getIsAbsolute() )
                {
                    child = SceneObjectPathUtils::getObjectFromAbsolutePath(childPath);
                }
                else
                {
                    child = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,childPath);
                }

                if ( child )
                {
                    Actor * actor = child->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                    if (actor)
                    {
                        _locks.push_back(actor);
                    }
                }
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_ForceFieldComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface,_flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::circle(m_actor->getPos(), getSoftCollisionRadius(),Color::white(),0.f,"softColl");
        DebugDraw::bezier(m_circleB, m_actor->getDepth(),Color::white(),1.0f,0.2f);
        DebugDraw::bezier(m_targetB, m_actor->getDepth(),Color::white(),1.0f,0.2f);
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ForceFieldComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ForceFieldComponent_Template)
    SERIALIZE_MEMBER("softCollisionRadius",m_softCollisionRadius );
    SERIALIZE_MEMBER("bounceRadius",m_bounceRadius );
    SERIALIZE_CONTAINER_OBJECT("linkEvents",m_linkEvents);
    SERIALIZE_MEMBER("softCollisionExitSpeed",m_softCollisionExitSpeed );
    SERIALIZE_MEMBER("softCollisionExitForce",m_softCollisionExitForce);
    SERIALIZE_MEMBER("lockedFx",m_lockedFx);

    SERIALIZE_MEMBER("patchCenterWidth",m_patchCenterWidth);
    SERIALIZE_MEMBER("patchCircleWidth",m_patchCircleWidth);
    SERIALIZE_MEMBER("patchTargetWidth",m_patchTargetWidth);
    SERIALIZE_MEMBER("patchCircleTangeantRotationOffset",m_patchCircleTangeantRotationOffset);
    SERIALIZE_MEMBER("patchTargetTangeantRotationOffset",m_patchTargetTangeantRotationOffset);
    SERIALIZE_MEMBER("patchCircleTangeantRotationFrequency",m_patchCircleTangeantRotationFrequency);
    SERIALIZE_MEMBER("patchTargetTangeantRotationFrequency",m_patchTargetTangeantRotationFrequency);
    SERIALIZE_MEMBER("texture",m_textureFile);
    SERIALIZE_MEMBER("patchTileLength",m_patchTileLength);
    SERIALIZE_MEMBER("patchScrollSpeed",m_patchScrollSpeed);
    SERIALIZE_MEMBER("idleAnim",m_idleAnim);
    SERIALIZE_MEMBER("fadeAnim",m_fadeAnim);
    SERIALIZE_MEMBER("tesselationLength",m_tesselationLength);
    SERIALIZE_MEMBER("patchMidPointOffset",m_patchMidPointOffset);
    SERIALIZE_MEMBER("patchMidPointPercent",m_patchMidPointPercent);
    SERIALIZE_MEMBER("patchCenterOffset",m_patchCenterOffset);

    SERIALIZE_ENUM_BEGIN("blendmode",m_blendMode);
    SERIALIZE_ENUM_VAR(GFX_BLEND_UNKNOWN);
    SERIALIZE_ENUM_VAR(GFX_BLEND_COPY);
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHA);
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAPREMULT);
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADEST);
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHADESTPREMULT);
    SERIALIZE_ENUM_VAR(GFX_BLEND_ADD);
    SERIALIZE_ENUM_VAR(GFX_BLEND_ADDALPHA);
    SERIALIZE_ENUM_VAR(GFX_BLEND_SUBALPHA);
    SERIALIZE_ENUM_VAR(GFX_BLEND_SUB);
    SERIALIZE_ENUM_VAR(GFX_BLEND_MUL);
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHAMUL);
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAMUL);
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHA);
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHAPREMULT);
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADEST);
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHADESTPREMULT);
    SERIALIZE_ENUM_VAR(GFX_BLEND_MUL2X);
    SERIALIZE_ENUM_VAR(GFX_BLEND_ALPHATOCOLOR);
    SERIALIZE_ENUM_VAR(GFX_BLEND_IALPHATOCOLOR);
    SERIALIZE_ENUM_END();

    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_ForceFieldComponent_Template,LinkEvent_Template)
    SERIALIZE_MEMBER("tag",m_tag);
    SERIALIZE_OBJECT_WITH_FACTORY("onActivateForceFieldEvent", m_onActivateForceFieldEvent, ACTORSMANAGER->getEventFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("onDeactivateForceFieldEvent", m_onDeactivateForceFieldEvent, ACTORSMANAGER->getEventFactory());
    END_SERIALIZATION()

    Ray_ForceFieldComponent_Template::Ray_ForceFieldComponent_Template()
    : m_softCollisionRadius(1.0f)
    , m_bounceRadius(2.0f)
    , m_softCollisionExitSpeed(3.0f)
    , m_softCollisionExitForce(800.0f)
    , m_patchCenterWidth(1.0f)
    , m_patchCircleWidth(0.5f)
    , m_patchTargetWidth(0.2f)
    , m_patchCircleTangeantRotationOffset(btrue,15.0f)
    , m_patchTargetTangeantRotationOffset(btrue,15.0f)
    , m_patchCircleTangeantRotationFrequency(1.0f)
    , m_patchTargetTangeantRotationFrequency(1.0f)
    , m_patchTileLength(1.0f)
    , m_patchScrollSpeed(1.0f)
    , m_blendMode(GFX_BLEND_ALPHA)
    , m_tesselationLength(0.1f)
    , m_patchMidPointOffset(0.0f)
    , m_patchCenterOffset(0.0f)
    , m_patchMidPointPercent(0.5f)
    {
    }

    Ray_ForceFieldComponent_Template::~Ray_ForceFieldComponent_Template()
    {
        for (u32 i = 0; i < m_linkEvents.size(); i++)
        {
            LinkEvent_Template& linkEvent = m_linkEvents[i];
            SF_DEL(linkEvent.m_onActivateForceFieldEvent);
            SF_DEL(linkEvent.m_onDeactivateForceFieldEvent);
        }
    }

}
