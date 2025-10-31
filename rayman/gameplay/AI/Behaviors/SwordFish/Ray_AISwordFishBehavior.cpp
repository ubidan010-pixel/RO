#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISWORDFISHBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/SwordFish/Ray_AISwordFishBehavior.h"
#endif //_ITF_RAY_AISWORDFISHBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AISwordFishBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AISwordFishBehavior_Template)
    SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("anticipation", m_anticipation, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("stuck", m_stuck, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("detectionShape", m_detectionShape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("anticipationDuration", m_anticipationDuration);
    SERIALIZE_MEMBER("force", m_force);
    SERIALIZE_MEMBER("maxSpeed", m_maxSpeed);
    SERIALIZE_MEMBER("minSpeed", m_minSpeed);
    SERIALIZE_MEMBER("offsetDist", m_offsetDist);
    END_SERIALIZATION()

    Ray_AISwordFishBehavior_Template::Ray_AISwordFishBehavior_Template()
    : m_idle(NULL)
    , m_anticipation(NULL)
    , m_attack(NULL)
    , m_stuck(NULL)
    , m_detectionShape(NULL)
    , m_anticipationDuration(1.0f)
    , m_force(100.0f)
    , m_maxSpeed(10.0f)
    , m_minSpeed(5.0f)
    , m_offsetDist(2.0f)
    {
        
    }

    Ray_AISwordFishBehavior_Template::~Ray_AISwordFishBehavior_Template()
    {
        SF_DEL(m_idle);        
        SF_DEL(m_anticipation);        
        SF_DEL(m_attack);        
        SF_DEL(m_stuck);        
        SF_DEL(m_detectionShape);        

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AISwordFishBehavior)

    Ray_AISwordFishBehavior::Ray_AISwordFishBehavior()
    : m_idle(NULL)
    , m_anticipation(NULL)
    , m_attack(NULL)
    , m_stuck(NULL)
    , m_detectionShape(NULL)
    , m_timer(0.0f)
    , m_prevGravityMultiplier(1.0f)
    , m_subAnchorComponent(NULL)
    , m_phantomComponent(NULL)
    , m_prevPos( Vec2d::Zero )
    {
    }

    Ray_AISwordFishBehavior::~Ray_AISwordFishBehavior()
    {
        SF_DEL(m_detectionShape);
    }

    void Ray_AISwordFishBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_idle = createAiAction(getTemplate()->getIdle());
        m_anticipation = createAiAction(getTemplate()->getAnticipation());
        m_attack = createAiAction(getTemplate()->getAttack());
        m_stuck = createAiAction(getTemplate()->getStuck());

        const PhysShape * template_shape = getTemplate()->getDetectionShape();
        if ( template_shape)
        {            
            m_detectionShape = static_cast<PhysShape*>(template_shape->CreateNewObject());
            BinaryClone<PhysShape>(const_cast<PhysShape*>(getTemplate()->getDetectionShape()),m_detectionShape);
        }
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773), m_aiComponent);
        m_subAnchorComponent = m_actor->GetComponent<Ray_SubAnchorComponent>();
        m_phantomComponent = m_actor->GetComponent<PhantomComponent>();

        m_phantomComponent->setDisabled(btrue);

        m_prevGravityMultiplier = m_physComponent->getGravityMultiplier();
        m_physComponent->setGravityMultiplier(0.0f);
        m_physComponent->setUpdateRotation(bfalse);
        m_physComponent->setDisabled(btrue);

        m_endPos = m_subAnchorComponent->getPos(ITF_GET_STRINGID_CRC(endPos,905958472));

        Vec2d dir = m_endPos.truncateTo2D() - m_actor->get2DPos();
        dir.normalize();
        f32 angle = dir.getAngle();
        angle = canonizeAngle(angle);

        if (angle > MTH_PIBY2 || angle < -MTH_PIBY2)
        {
            m_actor->setFlipped(btrue);
            m_actor->setAngle(angle + MTH_PI);
        }
        else
        {
            m_actor->setAngle(angle);
        }

        startIdle();
     
        m_memorizedHits.clear();
    }

    void Ray_AISwordFishBehavior::update( f32 _delta  )
    {
        Super::update(_delta);

        if (m_currentAction == m_idle)
        {
            if (checkDetection())
            {
                startAnticipation();
            }
        }
        else if (m_currentAction == m_anticipation)
        {
            if (m_timer == 0.0f)
            {
                startAttack();
            }
            m_timer = Max(m_timer - _delta,0.0f);
        }
        else if (m_currentAction == m_attack)
        {
            updateAttack(_delta);
        }

        if (m_currentAction != m_stuck)
           updateStim(_delta);
    }

    void Ray_AISwordFishBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        //if (AnimPolylineEvent * polyEvent = _event->DynamicCast<AnimPolylineEvent>(ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773)))
        //{
        //    if (polyEvent->getName() == MRK_Prepare_Hit)
        //    {
        //        m_shape.setShape(polyEvent,m_animComponent,m_actor);
        //    }
        //}

    }

    void Ray_AISwordFishBehavior::onActivate()
    {
        startIdle();
    }

    void Ray_AISwordFishBehavior::onActionFinished()
    {
        Super::onActionFinished();

    }

    void Ray_AISwordFishBehavior::startIdle()
    {
        setAction(m_idle);
    }

    bbool Ray_AISwordFishBehavior::checkDetection()
    {

        DepthRange depthRange(m_actor->getDepth());
        // flip shape if necessary
        if ( PhysShapePolygon* polygon = m_detectionShape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            polygon->setFlipped(m_actor->isFlipped());
        }
        ActorList players;
        AIUtils::getLivePlayers(m_actor->getDepth(),players);
        for (u32 i = 0; i < players.size(); ++i)
        {
            if (players[i])
            {
                EventCollide collide;
                collide.setSender(m_actor->getRef());
                collide.setShape( m_detectionShape );
                collide.setPos(m_actor->get2DPos());
                collide.setAngle(m_actor->getAngle());
                players[i]->onEvent(&collide);
                if (collide.isCollide())
                {
                    return btrue;
                }
            }
        }
        return bfalse;
    }

    void Ray_AISwordFishBehavior::startAnticipation()
    {
        m_timer = getTemplate()->getAnticipationDuration();
        setAction(m_anticipation);
    }

    void Ray_AISwordFishBehavior::startAttack()
    {
        setAction(m_attack);
    }

    void Ray_AISwordFishBehavior::updateAttack( f32 _delta )
    {
        m_prevPos = m_actor->get2DPos();

        Vec2d dir = m_endPos.truncateTo2D() - m_prevPos;
        const f32 dist = dir.norm();       
        if ( dist != 0.0f )
        {
            dir/=dist;
        }

        const f32 displacement = getTemplate()->getMaxSpeed() * _delta;
        if (m_actor->get2DPos() != (m_endPos.truncateTo2D() - dir * getTemplate()->getOffsetDist()))
        {
            if (dist  - getTemplate()->getOffsetDist() <= displacement)
            {
                m_actor->set2DPos(m_endPos.truncateTo2D() - dir * getTemplate()->getOffsetDist());
                startStuck();
            }
            else
                m_actor->set2DPos(m_actor->get2DPos() + dir * displacement);
        }

    }

    void Ray_AISwordFishBehavior::onDeactivate()
    {
        Super::onDeactivate();
        m_physComponent->setDisabled(bfalse);
        m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
        m_phantomComponent->setDisabled(bfalse);
    }

    void Ray_AISwordFishBehavior::updateStim(f32 _delta)
    {

        ITF_MAP<ObjectRef,f32>::iterator it = m_memorizedHits.begin();
        while(it != m_memorizedHits.end())
        {
            it->second = Max(it->second-_delta,0.0f);
            if (it->second == 0.0f)
                it = m_memorizedHits.erase(it);
            else
                ++it;
        }

        m_shape.setShape(ITF_GET_STRINGID_CRC(L_Hurt,4024858701),m_animComponent,m_actor);


        const Vec2d& shapePos = m_actor->get2DPos();
        f32 shapeAngle = m_actor->getAngle();

        PunchStim punch;
        Vec2d dir = AIUtils::getLookDir(m_actor);
        punch.setLevel(0);
        punch.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
        punch.setDirection(Vec2d::Up);
        punch.setFxPos(shapePos.to3d(m_actor->getPos().m_z));
        punch.setDepth(m_actor->getDepth());
        punch.setSender(m_actor->getRef());
        punch.setAngle(shapeAngle);
        punch.setPos(shapePos);
        punch.setPrevPos(m_prevPos.IsEqual( Vec2d::Zero ) ? shapePos : m_prevPos );
        punch.setFaction(m_aiComponent->getFaction());

        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(punch.getPrevPos(), shapePos, shapeAngle,
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
                if (actor == m_actor)
                    continue;
                break;
            }

            if ( actor )
            {
                contactPoints[actor->getRef()].push_back(&contact);
            }
        }

        for (ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> >::iterator it = contactPoints.begin(); it != contactPoints.end(); ++it)
        {
			Actor* actor = it->first.getActor();

			if (actor)
			{
				bbool isPlayer = GAMEMANAGER->isPlayerActor(actor->getRef());
				bbool sendPunch = bfalse;

				if (isPlayer && m_memorizedHits.find(it->first) == m_memorizedHits.end())
				{
					m_memorizedHits[it->first] = 0.5f;
					sendPunch = btrue;
				}

				if (sendPunch || !isPlayer)
				{
					for (ITF_VECTOR<SCollidableContact*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
						punch.addContact(*(*it2));
					actor->onEvent(&punch);
					punch.resetContacts();
				}
			}

        }
    }

    void Ray_AISwordFishBehavior::startStuck()
    {
        setAction(m_stuck);
    }

    void Ray_AISwordFishBehavior::onActorBecomeActive()
    {
        Super::onActorBecomeActive();

        Vec2d dir = m_endPos.truncateTo2D() - m_actor->get2DPos();
        dir.normalize();
        f32 angle = dir.getAngle();
        angle = canonizeAngle(angle);

        if (angle > MTH_PIBY2 || angle < -MTH_PIBY2)
        {
            m_actor->setFlipped(btrue);
            m_actor->setAngle(angle + MTH_PI);
        }
        else
        {
            m_actor->setAngle(angle);
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    void Ray_AISwordFishBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),m_detectionShape);
		DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),m_shape.getShape());

    }


#endif // ITF_SUPPORT_EDITOR
}

