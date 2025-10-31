#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPolylinePunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIPerformHitPolylinePunchAction_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIPerformHitPolylinePunchAction_Template)
        SERIALIZE_MEMBER("dirOffset", m_dirOffset);
    SERIALIZE_MEMBER("hitEnvironment", m_hitEnvironment);
    SERIALIZE_MEMBER("memorizeHitTime", m_memorizeHitTime);
    END_SERIALIZATION()

    Ray_AIPerformHitPolylinePunchAction_Template::Ray_AIPerformHitPolylinePunchAction_Template()
    : m_dirOffset(bfalse,0.f)
    , m_hitEnvironment(bfalse)
    , m_memorizeHitTime(0.3f)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIPerformHitPolylinePunchAction)

    Ray_AIPerformHitPolylinePunchAction::Ray_AIPerformHitPolylinePunchAction()
    : Super()
    , m_performingHit(bfalse)
    , m_dirSetFromEvent(bfalse)
    {
    }

    Ray_AIPerformHitPolylinePunchAction::~Ray_AIPerformHitPolylinePunchAction()
    {
    }
    void Ray_AIPerformHitPolylinePunchAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetDirection,187560938),m_aiComponent);
    }

    void Ray_AIPerformHitPolylinePunchAction::onEvent(Event* _event)
    {
        Super::onEvent(_event);
        if (AnimPolylineEvent * polyEvent = _event->DynamicCast<AnimPolylineEvent>(ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773)))
        {
            if (polyEvent->getName() == MRK_Prepare_Hit)
            {
                m_shape.setShape(polyEvent,m_animComponent,m_actor);
                const Vec2d* poinst = polyEvent->getPoints();
                if (!m_dirSetFromEvent)
                {
                    m_dir = Vec2d::Zero;
                    ITF_ASSERT(polyEvent->getNumPoints() > 1);

                    if (m_actor->isFlipped())
                    {
                        for (u32 i =1;i< polyEvent->getNumPoints(); ++i)
                        {
                            m_dir += (poinst[i-1] - poinst[i]).getPerpendicular();
                        }
                    }
                    else
                    {
                        for (u32 i =1;i< polyEvent->getNumPoints(); ++i)
                        {
                            m_dir += (poinst[i] - poinst[i-1]).getPerpendicular();
                        }
                    }

                    m_dir = m_dir/f32(polyEvent->getNumPoints()-1);
                    m_dir.normalize();
                    m_dir = m_dir.Rotate(getTemplate()->getDirOffset().Radians());
                }
            }
            else if (polyEvent->getName() == getTemplate()->getPerformHitMarker())
            {
                m_shape.setShape(polyEvent,m_animComponent,m_actor);
                m_performHit = btrue;
            }
            else if (polyEvent->getName() == MRK_Perform_Hit_Start)
            {
                m_performingHit = btrue;
                m_polyID = polyEvent->getPolylineID();
            }
            else if (polyEvent->getName() == MRK_Perform_Hit_Stop)
            {
                m_performingHit = bfalse;
            }
        }
        else if (EventSetDirection* _dir = _event->DynamicCast<EventSetDirection>(ITF_GET_STRINGID_CRC(EventSetDirection,187560938)))
        {
            m_dir = _dir->getDirection();
            m_dirSetFromEvent = btrue;
        }
    }

    void Ray_AIPerformHitPolylinePunchAction::performHit()
    {
        const Vec2d& shapePos = m_actor->get2DPos();
        f32 shapeAngle = m_actor->getAngle();

        PunchStim punch;
        punch.setLevel(getTemplate()->getHitLevel());
        punch.setReceivedHitType(getTemplate()->getHitType());
        punch.setDirection(m_dir);
        punch.setFxPos(shapePos.to3d(m_actor->getPos().m_z));
        punch.setDepth(m_actor->getDepth());
        punch.setSender(m_actor->getRef());
        punch.setAngle(shapeAngle);
        punch.setPos(shapePos);
        punch.setPrevPos(shapePos);
        punch.setFaction(m_aiComponent->getFaction());

        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(shapePos, shapePos, shapeAngle,
            m_shape.getShape(),
            m_actor->getDepth(),
            ECOLLISIONFILTER_ALL,
            contacts);

        ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> > contactPoints;

        for ( u32 i = 0; i < contacts.size(); i++ )
        {
            SCollidableContact& contact = contacts[i];

            if ( contact.m_collidableUserData == m_actor->getRef() )
            {
                continue;
            }

            BaseObject* obj = contact.m_collidableUserData.getObject();
            if (!obj)
            {
                continue;
            }

            Actor* actor = NULL;
            switch (obj->getObjectType())
            {
            case BaseObject::eActor:
                actor = static_cast<Actor*>(obj);
                break;
            case BaseObject::ePolyLine:
                actor = static_cast<PolyLine*>(obj)->getOwnerActor();
                break;
            }

            if ( actor )
            {
                contactPoints[actor->getRef()].push_back(&contact);
            }
        }

        for (ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> >::iterator it = contactPoints.begin(); it != contactPoints.end(); ++it)
        {
            if (m_memorizedHits.find(it->first) == m_memorizedHits.end())
            {
                m_memorizedHits[it->first] = getTemplate()->getMemorizeHitTime();
                Actor * actor = it->first.getActor();
                if (actor)
                {
                    for (ITF_VECTOR<SCollidableContact*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
                        punch.addContact(*(*it2));
                    actor->onEvent(&punch);
                    punch.resetContacts();
                }
            }

        }

        // Check if we hit environment
        if (getTemplate()->getHitEnvironment())
        {
            PhysContactsContainer contacts;
            bbool hitEnvironment = PHYSWORLD->checkEncroachment(
                shapePos, shapePos, shapeAngle,
                m_shape.getShape(),
                ECOLLISIONFILTER_ENVIRONMENT,
                m_actor->getDepth(), 
                contacts);

            if (hitEnvironment)
            {
                bbool hitOne = bfalse;

                for ( u32 i = 0; i < contacts.size(); i++ )
                {
                    SCollidableContact& contact = contacts[i];

                    if ( contact.m_collidableUserData == m_actor->getRef() )
                    {
                        continue;
                    }

                    BaseObject* obj = contact.m_collidableUserData.getObject();
                    if (!obj)
                    {
                        continue;
                    }


                    if ( PolyLine* polyLine = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)) )
                    {
                        if (polyLine->getOwnerActor() == m_actor)
                        {
                            continue;
                        }
                        //GFX_ADAPTER->drawDBGCircle(contact.m_contactPoint.to3d(m_actor->getPos().m_z), 0.2f, Color::blue(), 1.f);
                        hitOne = btrue;
                        break;
                    }
                }

                if (hitOne)
                {
                    EventStimNotify eventHitPolyline;
                    eventHitPolyline.setSender(m_actor->getRef());
                    m_aiBehavior->onEvent(&eventHitPolyline);
                }
            }
        }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        if ( STIMSMANAGER->getDebugMode() )
        {
            f32 duration = (STIMSMANAGER->getDebugMode() == 1) ? 1.f : 0.f; // 1 second or 1 frame

            DebugDraw::shape(shapePos, m_actor->getDepth(), shapeAngle, m_shape.getShape(), Color::white(), duration);

            for ( u32 i = 0; i < contacts.size(); i++ )
            {
                SCollidableContact& contact = contacts[i];

                GFX_ADAPTER->drawDBGCircle(contact.m_contactPoint, 0.1f, 0.f, 0.f, 0.3f, duration);
            }
        }
#endif // ITF_SUPPORT_DEBUGFEATURE
    }

    void Ray_AIPerformHitPolylinePunchAction::onActivate()
    {
        Super::onActivate();
        m_performingHit = bfalse;
        m_dirSetFromEvent = bfalse;
        m_memorizedHits.clear();

    }

    void Ray_AIPerformHitPolylinePunchAction::update( f32 _delta )
    {
        Super::update(_delta);
        if (m_performingHit)
        {
            m_shape.setShape(m_polyID,m_animComponent,m_actor);
            u32 numPoints;
            const Vec2d* points = m_animComponent->getCurrentPolylinePointBuffer(m_polyID, &numPoints);

            if (numPoints > 1)
            {
                m_dir = Vec2d::Zero;
                for (u32 i =1;i< numPoints; ++i)
                {
                    if (m_actor->isFlipped())
                    {
                        m_dir += (points[i-1] - points[i]).getPerpendicular();
                    }
                    else
                    {
                        m_dir += (points[i] - points[i-1]).getPerpendicular();
                    }         
                }
                m_dir = m_dir/f32(numPoints-1);
                m_dir.normalize();
                m_dir = m_dir.Rotate(getTemplate()->getDirOffset().Radians());
            }
            performHit();
        }
        
        ITF_MAP<ObjectRef,f32>::iterator it = m_memorizedHits.begin();
        while(it != m_memorizedHits.end())
        {
            it->second = Max(it->second-_delta,0.0f);
            if (it->second == 0.0f)
                it = m_memorizedHits.erase(it);
            else
                ++it;
        }
    }
}
