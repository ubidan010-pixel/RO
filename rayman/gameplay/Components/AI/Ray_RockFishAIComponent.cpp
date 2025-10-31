#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ROCKFISHAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_RockFishAIComponent.h"
#endif //_ITF_RAY_ROCKFISHAICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_RockFishAIComponent_Template)

        BEGIN_SERIALIZATION(Ray_RockFishAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("attackDetectionShape", m_attackDetectionShape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("detectionRadius",m_detectionRadius);
    SERIALIZE_MEMBER("minDetectTime",m_minDetectTime);
    SERIALIZE_MEMBER("minIdleTime",m_minIdleTime);
    SERIALIZE_MEMBER("sleepAnim",m_sleepAnim);
    SERIALIZE_MEMBER("detectAnim",m_detectAnim);
    SERIALIZE_MEMBER("openAnim",m_openAnim);
    SERIALIZE_MEMBER("closeAnim",m_closeAnim);
    SERIALIZE_MEMBER("openTime",m_openTime);
    SERIALIZE_CONTAINER("points",m_pointList);
    SERIALIZE_MEMBER("regionType",m_regionType);
    SERIALIZE_MEMBER("gameMaterial",m_gameMaterial);
    END_SERIALIZATION()

        Ray_RockFishAIComponent_Template::Ray_RockFishAIComponent_Template()
        : m_attackDetectionShape(NULL)
        , m_detectionRadius(10.0f)
        , m_minDetectTime(1.0f)
        , m_minIdleTime(1.0f)
        , m_openTime(5.0f)
    {

    }

    Ray_RockFishAIComponent_Template::~Ray_RockFishAIComponent_Template()
    {
        SF_DEL(m_attackDetectionShape);        

    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_RockFishAIComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_RockFishAIComponent)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_RockFishAIComponent)
        END_VALIDATE_COMPONENT()

        Ray_RockFishAIComponent::Ray_RockFishAIComponent()
        : m_detectorComponent(NULL)
        , m_animComponent(NULL)
        , m_detectionShape(NULL)
        , m_state(State_idle)
        , m_polyline(NULL)
        , m_timer(0.0f)
        , m_previouslyDetected(bfalse)
    {
    }

    Ray_RockFishAIComponent::~Ray_RockFishAIComponent()
    {
        SF_DEL(m_detectionShape);
        SF_DEL(m_polyline);
    }


    void Ray_RockFishAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_detectorComponent = m_actor->GetComponent<DetectorComponent>();
        m_animComponent= m_actor->GetComponent<AnimatedComponent>(); 

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);

        const PhysShape * template_shape = getTemplate()->getAttackDetectionShape();
        if ( template_shape)
        {            
            m_detectionShape = static_cast<PhysShape*>(template_shape->CreateNewObject());
            BinaryClone<PhysShape>(const_cast<PhysShape*>(getTemplate()->getAttackDetectionShape()),m_detectionShape);
        }


        u32 numPoints = getTemplate()->getPointList().size();

        if ( numPoints < 2 )
        {
            m_polyline = NULL;
        }

        m_polyline = newAlloc(mId_Gameplay, PolyLine);
        m_polyline->setOwnerActor(m_actor);

        for ( u32 i = 0; i < numPoints; i++ )
        {
            m_polyline->addPoint(getTemplate()->getPointList()[i] + m_actor->get2DPos());
        }
        if ( getTemplate()->getGameMaterial().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
        {
            World::addGameMaterialFromFile(getTemplate()->getGameMaterial(), m_actor->getResourceGroup());
        }
        for ( u32 edgeIndex = 0; edgeIndex < numPoints; edgeIndex++ )
        {
            m_polyline->setGameMaterialAt(getTemplate()->getGameMaterial().getStringID(),edgeIndex);
        }

        m_polyline->setLoop(btrue);


        startIdle();
    }

    bbool Ray_RockFishAIComponent::processNewCatch( Actor* actor )
    {
        PhysComponent* physComponent = actor->GetComponent<PhysComponent>();
        if (!physComponent)
        {
            return bfalse;
        }

        bbool isPlayer = GAMEMANAGER->isPlayerActor(actor->getRef());
        if (isPlayer)
        {            
            EventQueryIsDead isDead;
            actor->onEvent(&isDead);
            if (!isDead.getIsDead())
            {
                Ray_EventInstantKill kill;
                kill.setSender(m_actor->getRef());
                actor->onEvent(&kill);
                return btrue;
            }
        }
        return bfalse;
    }

    void Ray_RockFishAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);


        if (m_state == State_idle)
        {
            updateIdle(_dt);

        }
        else if (m_state == State_detected)
        {
            updateDetected(_dt);

        }
        else if (m_state == State_open)
        {
            updateOpen(_dt);

        }
        else if (m_state == State_close)
        {
            updateClose(_dt);

        }

        u32 numPoints = getTemplate()->getPointList().size();
        if ( m_actor->isFlipped() )
        {
            for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
            {
                u32 arrayIndex = numPoints - pointIndex - 1;
                Vec2d p = getTemplate()->getPointList()[arrayIndex];

                p.m_x *= -1.f;

                p += m_actor->get2DPos();

                m_polyline->setPosAt(p,pointIndex);
            }
        }
        else
        {
            for ( u32 pointIndex = 0; pointIndex < numPoints; pointIndex++ )
            {
                Vec2d pos = getTemplate()->getPointList()[pointIndex] + m_actor->get2DPos();

                m_polyline->setPosAt(pos,pointIndex);
            }
        }
        m_polyline->update(_dt);

    }

    bbool Ray_RockFishAIComponent::checkAttackDetection()
    {
        u32 playerCount = GAMEMANAGER->getMaxPlayerCount();
        DepthRange depthRange(m_actor->getDepth());
        // flip shape if necessary
        if ( PhysShapePolygon* polygon = m_detectionShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            polygon->setFlipped(m_actor->isFlipped());
        }
        for (u32 i = 0; i < playerCount; ++i)
        {
            Actor* pActor = GAMEMANAGER->getActivePlayer(i);

            if ( !pActor || depthRange != DepthRange(pActor->getDepth()) )
            {
                continue;
            }
            EventQueryIsDead isDead;
            pActor->onEvent(&isDead);
            if (isDead.getIsDead())
            {
                continue;
            }
            EventCollide collide;
            collide.setSender(m_actor->getRef());
            collide.setShape( m_detectionShape );
            collide.setPos(m_actor->get2DPos());
            collide.setAngle(m_actor->getAngle());
            pActor->onEvent(&collide);
            if (collide.isCollide())
            {
                return btrue;
            }
        }
        return bfalse;
    }
    bbool Ray_RockFishAIComponent::checkDetected()
    {
        u32 playerCount = GAMEMANAGER->getMaxPlayerCount();
        DepthRange depthRange(m_actor->getDepth());

        for (u32 i = 0; i < playerCount; ++i)
        {
            Actor* pActor = GAMEMANAGER->getActivePlayer(i);

            if ( !pActor || depthRange != DepthRange(pActor->getDepth()) )
            {
                continue;
            }
            EventQueryIsDead isDead;
            pActor->onEvent(&isDead);
            if (!isDead.getIsDead())
            {
                if ((pActor->get2DPos() - m_actor->get2DPos()).sqrnorm() < getTemplate()->getDetectionRadius() * getTemplate()->getDetectionRadius())
                {
                    return btrue;
                }
            }
        }
        return bfalse;
    }
    bbool Ray_RockFishAIComponent::checkAttack()
    {
        ActorRefList& actorsInsideThisFrame = m_detectorComponent->getActorsInside();

        bbool ret = bfalse;
        for (u32 i = 0; i < actorsInsideThisFrame.size(); i++)
        {
            ActorRef& actorInside = actorsInsideThisFrame[i];
            if ( m_actorsInsideLastFrame.find(actorInside) == -1 )
            {
                if ( Actor* travelerActor = actorInside.getActor() )
                {
                    ret = processNewCatch(travelerActor) || ret;
                }
            }
        }

        m_actorsInsideLastFrame = actorsInsideThisFrame;
        return ret;
    }
    void Ray_RockFishAIComponent::updateIdle(f32 _dt)
    {
        if (checkDetected() )
        {
            if (m_timer == 0.0f)
                startDetected();
        }
        m_timer = Max(m_timer - _dt, 0.0f);
    }

    void Ray_RockFishAIComponent::updateDetected(f32 _dt)
    {
        if (checkAttackDetection() && m_timer == 0.0f)
        {
            startOpen();
        }
        else
        {
            if (checkDetected())
            {
                if (!m_previouslyDetected)
                    m_timer = getTemplate()->getMinDetectTime();
                m_previouslyDetected = btrue;
            }
            else
            {
                if (m_timer == 0.0f)
                {
                    startIdle();
                }
                m_previouslyDetected = bfalse;
            }

        }
        m_timer = Max(m_timer - _dt,0.0f);
    }
    void Ray_RockFishAIComponent::updateOpen(f32 _dt)
    {
        if (checkAttack())
        {
            startClose();

        }
        else
        {
            if (m_timer == 0.0f)
                startClose();
        }

        m_timer = Max(m_timer - _dt, 0.0f);
    }

    void Ray_RockFishAIComponent::updateClose(f32 _dt)
    {
        checkAttack();
        if (m_animComponent->isMainNodeFinished() ) //finished
        {
            startDetected();
        }
    }

    void Ray_RockFishAIComponent::startDetected()
    {
        m_state = State_detected;
        m_animComponent->setAnim(getTemplate()->getDetectAnim());
        m_timer = getTemplate()->getMinDetectTime();
        m_previouslyDetected = btrue;
    }

    void Ray_RockFishAIComponent::startOpen()
    {
        m_state = State_open;
        m_animComponent->setAnim(getTemplate()->getOpenAnim());
        m_timer = getTemplate()->getOpenTime();
    }

    void Ray_RockFishAIComponent::startClose()
    {
        m_animComponent->setAnim(getTemplate()->getCloseAnim());
        m_state = State_close;
    }

    void Ray_RockFishAIComponent::startIdle()
    {
        m_previouslyDetected = bfalse;
        m_timer = getTemplate()->getMinIdleTime();
        m_state = State_idle;
        m_animComponent->setAnim(getTemplate()->getSleepAnim());
    }

    void Ray_RockFishAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (AnimGameplayEvent * gEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (gEvent->getName() == ITF_GET_STRINGID_CRC(MRK_StartFx, 4034159921))
            {
                if (m_polyline)
                    REGIONS_MANAGER->addRegion(m_polyline->getRef(),getTemplate()->getRegionType(),m_actor->getDepth());

            }
            else if(gEvent->getName() == ITF_GET_STRINGID_CRC(MRK_StopFx, 2915074225))
            {
                if (m_polyline)
                    REGIONS_MANAGER->removeRegion(m_polyline->getRef());
            }
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void Ray_RockFishAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),m_detectionShape);
        const RegionsManager::RegionDataList * regionList = REGIONS_MANAGER->getRegions(m_actor->getDepth());
        if (regionList)
        {
            for (u32 i = 0; i < regionList->size(); ++i)
            {
                if ((*regionList)[i].m_ref == m_polyline->getRef())
                    DebugDraw::polyline(*m_polyline);
            }
        }
    }


#endif // ITF_SUPPORT_EDITOR
};