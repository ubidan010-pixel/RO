#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBLOWFISHBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/BlowFish/Ray_AIBlowFishBehavior.h"
#endif //_ITF_RAY_AIBLOWFISHBEHAVIOR_H_

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


#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIBlowFishBehavior_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_AIBlowFishBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("detect", m_detect, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("inflatedIdle", m_inflatedIdle, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("inflate", m_inflate, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("deflate", m_deflate, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_MEMBER("detectionRange", m_detectionRange);
	SERIALIZE_MEMBER("detectionCloseRange", m_detectionCloseRange);
    SERIALIZE_MEMBER("inflateDuration", m_inflateDuration);
    SERIALIZE_MEMBER("inflatedScaleMultiplier", m_inflatedScaleMultiplier);
    SERIALIZE_MEMBER("deflateDuration", m_deflateDuration);
    SERIALIZE_MEMBER("minIdleDuration", m_minIdleDuration);
    SERIALIZE_MEMBER("minInflatedDuration", m_minInflatedDuration);
    SERIALIZE_MEMBER("repulsionForce", m_repulsionForce);
    SERIALIZE_MEMBER("attractionForce", m_attractionForce);
    SERIALIZE_MEMBER("friction", m_friction);
    SERIALIZE_MEMBER("memorizedHitTime", m_memorizedHitTime);
    END_SERIALIZATION()

        Ray_AIBlowFishBehavior_Template::Ray_AIBlowFishBehavior_Template()
        : m_idle(NULL)
        , m_detect(NULL)
        , m_inflatedIdle(NULL)
        , m_inflate(NULL)
        , m_deflate(NULL)
        , m_detectionRange(10.0f)
		, m_detectionCloseRange(1.0f)
        , m_inflateDuration(1.0f)
        , m_inflatedScaleMultiplier(10.0f)
        , m_deflateDuration(2.0f)
        , m_minIdleDuration(1.0f)
        , m_minInflatedDuration(1.0f)
        , m_repulsionForce(1.0f)
        , m_attractionForce(1.0f)
        , m_friction(0.9f)
        , m_memorizedHitTime(0.2f)
    {

    }

    Ray_AIBlowFishBehavior_Template::~Ray_AIBlowFishBehavior_Template()
    {
        SF_DEL(m_idle);        
        SF_DEL(m_detect);
        SF_DEL(m_inflatedIdle);
        SF_DEL(m_inflate);
        SF_DEL(m_deflate)

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIBlowFishBehavior)

        Ray_AIBlowFishBehavior::Ray_AIBlowFishBehavior()
        : m_idle(NULL)
        , m_detect(NULL)
        , m_inflatedIdle(NULL)
        , m_inflate(NULL)
        , m_deflate(NULL)
        , m_timer(0.0f)
        , m_active(btrue)
        , m_previouslyDetected(bfalse)
        , m_scaleCursor(1.0f)
        , m_phantomComponent(NULL)
        , m_phantomInitialScale(1.0f)
        , m_tweenComponent(NULL)
    {
    }

    Ray_AIBlowFishBehavior::~Ray_AIBlowFishBehavior()
    {
    }

    void Ray_AIBlowFishBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_idle = createAiAction(getTemplate()->getIdle());
        m_detect = createAiAction(getTemplate()->getDetect());
        m_inflatedIdle = createAiAction(getTemplate()->getInflatedIdle());
        m_inflate = createAiAction(getTemplate()->getInflate());
        m_deflate = createAiAction(getTemplate()->getDeflate());

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316),m_aiComponent);

        m_phantomComponent = m_actor->GetComponent<PhantomComponent>();
        m_tweenComponent = m_actor->GetComponent<TweenComponent>();

        m_phantomInitialScale = m_phantomComponent->getFactoryShapeScaled()->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168))->getRadius();

        m_active = btrue;
        startIdle();

    }

    void Ray_AIBlowFishBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
        if (m_currentAction == m_idle)
        {
            updateIdle();
        }
        else if (m_currentAction == m_detect)
        {
            updateDetect();
        }
        else if (m_currentAction == m_inflate)
        {
            updateInflate(_delta);
        }
        else if (m_currentAction == m_inflatedIdle)
        {
            updateInflatedIdle(_delta);
        }
        else if (m_currentAction == m_deflate)
        {
            updateDeflate(_delta);
        }
        if (!m_tweenComponent)
            updatePosition(_delta);
        m_timer = Max(0.0f, m_timer - _delta);
    }

    void Ray_AIBlowFishBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        if (EventActivate * onActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
        {
            m_active = onActivate->getActivated();
        }
        else if (PunchStim * punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
        {
            if (m_currentAction == m_inflatedIdle)
            {

                Actor * actor = ActorRef(punch->getSender()).getActor();
                if (actor && actor != m_actor && AIUtils::isEnemyHit(punch,m_aiComponent->getFaction()))
                {
                    if ( punch->getReceivedHitType() == RECEIVEDHITTYPE_SHOOTER )
                    {
                        EventHitSuccessful hitSuccess;
                        hitSuccess.setSender( m_actor->getRef() );
                        hitSuccess.setHasBeenDamaged( bfalse );
                        actor->onEvent(&hitSuccess);
                    }
                    else
                    {
                        PunchStim punchStim;

                        punchStim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
                        punchStim.setIsRadial(btrue);
                        Vec2d dir = actor->get2DPos()-m_actor->get2DPos();
                        dir.normalize();
                        punchStim.setDirection(dir);
                        punchStim.setLevel(1);
                        punchStim.setFaction(m_aiComponent->getFaction());

                        punchStim.setSender(m_actor->getRef());
                        punchStim.setAngle(m_actor->getAngle());
                        punchStim.setPos(m_actor->get2DPos());

                        actor->onEvent(&punchStim);
                    }
                }

            }
        }
        else if (EventQueryCanReceiveHit * onQuery = _event->DynamicCast<EventQueryCanReceiveHit>(ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316)))
        {
            onQuery->setCanReceiveHit(m_currentAction != m_inflatedIdle);
        }
    }

    void Ray_AIBlowFishBehavior::onActivate()
    {
        //Super::onActivate();
        startIdle();
    }

    void Ray_AIBlowFishBehavior::onDeactivate()
    {
        Super::onDeactivate();

    }

    void Ray_AIBlowFishBehavior::onActionFinished()
    {
        Super::onActionFinished();
        if (m_currentAction == m_idle)
        {
            onIdleFinished();
        }
        else if (m_currentAction == m_detect)
        {
            onDetectFinished();
        }
        else if (m_currentAction == m_inflate)
        {
            onInflateFinished();
        }
        else if (m_currentAction == m_inflatedIdle)
        {
            onInflatedIdleFinished();
        }
        else if (m_currentAction == m_deflate)
        {
            onDeflateFinished();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //  .oooooo.               oooooooooooo  o8o               o8o           oooo                        .o8  
    // d8P'  `Y8b              `888'     `8  `"'               `"'           `888                       "888  
    //888      888 ooo. .oo.    888         oooo  ooo. .oo.   oooo   .oooo.o  888 .oo.    .ooooo.   .oooo888  
    //888      888 `888P"Y88b   888oooo8    `888  `888P"Y88b  `888  d88(  "8  888P"Y88b  d88' `88b d88' `888  
    //888      888  888   888   888    "     888   888   888   888  `"Y88b.   888   888  888ooo888 888   888  
    //`88b    d88'  888   888   888          888   888   888   888  o.  )88b  888   888  888    .o 888   888  
    // `Y8bood8P'  o888o o888o o888o        o888o o888o o888o o888o 8""888P' o888o o888o `Y8bod8P' `Y8bod88P" 
    //////////////////////////////////////////////////////////////////////////
    void Ray_AIBlowFishBehavior::onIdleFinished()
    {
        //
    }

    void Ray_AIBlowFishBehavior::onDetectFinished()
    {
        startInflate();
    }

    void Ray_AIBlowFishBehavior::onInflateFinished()
    {
        startInflatedIdle();
    }

    void Ray_AIBlowFishBehavior::onInflatedIdleFinished()
    {
        startDeflate();
    }

    void Ray_AIBlowFishBehavior::onDeflateFinished()
    {
        if (m_tweenComponent)
            m_tweenComponent->resumeTweening();
        startIdle();
    }

    //////////////////////////////////////////////////////////////////////////
    //ooooo     ooo                  .o8                .             
    //`888'     `8'                 "888              .o8             
    // 888       8  oo.ooooo.   .oooo888   .oooo.   .o888oo  .ooooo.  
    // 888       8   888' `88b d88' `888  `P  )88b    888   d88' `88b 
    // 888       8   888   888 888   888   .oP"888    888   888ooo888 
    // `88.    .8'   888   888 888   888  d8(  888    888 . 888    .o 
    //   `YbodP'     888bod8P' `Y8bod88P" `Y888""8o   "888" `Y8bod8P' 
    //               888                                              
    //              o888o                                             
    //////////////////////////////////////////////////////////////////////////
    void Ray_AIBlowFishBehavior::updateIdle()
    {
        if (m_timer == 0.0f)
        {
            if (isPlayerDetected())
                startDetect();
        }
    }

    void Ray_AIBlowFishBehavior::updateDetect()
    {
		if (isPlayerDetected(btrue))
			startInflate();
    }

    void Ray_AIBlowFishBehavior::updateInflate(f32 _dt)
    {
        m_scaleCursor = 1.0f - m_timer/getTemplate()->getInflateDuration();
        updateScale();
        updateStim(_dt);
        if (m_timer == 0.0f)
            onInflateFinished();
    }

    void Ray_AIBlowFishBehavior::updateInflatedIdle(f32 _dt)
    {
        updateStim(_dt);

        const bbool playerDetected = isPlayerDetected();
        if (!playerDetected)
        {
            if ( m_previouslyDetected)
                m_timer = getTemplate()->getMinInflatedDuration();

            if (m_timer == 0.0f)
            {
                startDeflate();
            }
        }
        m_previouslyDetected = playerDetected;

    }

    void Ray_AIBlowFishBehavior::updateDeflate(f32 _dt)
    {
        m_scaleCursor = m_timer/getTemplate()->getDeflateDuration();
        updateScale();
        updateStim(_dt);
        if (m_timer == 0.0f)
            onDeflateFinished();

    }

    //////////////////////////////////////////////////////////////////////////
    // .oooooo..o     .                          .   
    //d8P'    `Y8   .o8                        .o8   
    //Y88bo.      .o888oo  .oooo.   oooo d8b .o888oo 
    // `"Y8888o.    888   `P  )88b  `888""8P   888   
    //     `"Y88b   888    .oP"888   888       888   
    //oo     .d8P   888 . d8(  888   888       888 . 
    //8""88888P'    "888" `Y888""8o d888b      "888" 
    //////////////////////////////////////////////////////////////////////////
    void Ray_AIBlowFishBehavior::startIdle()
    {
        setAction(m_idle);
    }

    void Ray_AIBlowFishBehavior::startDetect()
    {
        if (m_tweenComponent)
            m_tweenComponent->suspendTweening();
        setAction(m_detect);
        m_previouslyDetected = btrue;
    }

    void Ray_AIBlowFishBehavior::startInflate()
    {
        m_timer = getTemplate()->getInflateDuration();
        setAction(m_inflate);
        m_memorizedHits.clear();
    }

    void Ray_AIBlowFishBehavior::startInflatedIdle()
    {
        setAction(m_inflatedIdle);
    }

    void Ray_AIBlowFishBehavior::startDeflate()
    {
        m_timer = getTemplate()->getDeflateDuration();
        setAction(m_deflate);
    }


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void Ray_AIBlowFishBehavior::updatePosition(f32 _dt)
    {
        ITF_MAP<Actor*,ITF_VECTOR<SCollidableContact*> > actorCollisions;


        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(m_actor->get2DPos(), m_actor->get2DPos(), 0.0f,
            m_phantomComponent->getShape(), m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
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

            actorCollisions[otherActor].push_back(&contacts[i]);
        }

        const Faction myFaction = m_aiComponent->getFaction();
        const Vec2d myPos = m_actor->getLocal2DPos();
        const f32 myRadius = m_phantomComponent->getShape()->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168))->getRadius();
        Vec2d force;
        const f32 forceFactor = getTemplate()->getRepulsionForce();
        const f32 invDiameter = 1.0f / (2 * myRadius);
        for (ITF_MAP<Actor*,ITF_VECTOR<SCollidableContact*> >::iterator it = actorCollisions.begin(); it != actorCollisions.end(); ++it)
        {
            const Actor * other = it->first;
            const Faction faction = AIUtils::getFaction(other);
            if (faction == myFaction)
            {
                ITF_VECTOR<SCollidableContact*> & collContacts = it->second;
                //if (collContacts.size() == 1)
                //{

                //}
                //else
                //{
                const Vec2d contactPoint = collContacts[0]->m_contactPoint;
                const Vec2d otherPos = other->get2DPos();
                const f32 otherRadius = (otherPos - contactPoint).norm();
                Vec2d toMe = myPos - otherPos;
                const f32 distToOther  = (otherPos - myPos).norm();
                const f32 overlap = (distToOther - otherRadius) < 0.0f? (otherRadius - distToOther) + myRadius: (distToOther - otherRadius) + myRadius;
                const f32 ratio =  overlap * invDiameter;
                toMe.normalize();
                force += toMe * ratio * forceFactor;
                //}
            }
        }
        const Vec2d initPos = m_actor->getLocalInitialPos().truncateTo2D();
        Vec2d attractionForce = initPos - myPos;
        attractionForce.normalize();
        attractionForce *= getTemplate()->getAttractionForce();
        force += attractionForce;
        m_currentSpeed += force * _dt;
        m_currentSpeed *= getTemplate()->getFriction();
        m_actor->setLocal2DPos(m_actor->getLocal2DPos() + m_currentSpeed * _dt);
    }

    void Ray_AIBlowFishBehavior::updateAnimInputs()
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(scale,1221868919),m_scaleCursor);
    }

    bbool Ray_AIBlowFishBehavior::isPlayerDetected(bbool _closeDetect) const 
    {
		f32 detectRange = 0.0f;
		if (_closeDetect)
		{
			detectRange = getTemplate()->getDetectionCloseRange() * getTemplate()->getDetectionCloseRange();
		}
		else
		{
			detectRange = getTemplate()->getDetectionRange() * getTemplate()->getDetectionRange();
		}

        const i32 playerCount = GAMEMANAGER->getMaxPlayerCount();

        i32 playerToDetect = -1;
        for (i32 i = 0; i < playerCount && playerToDetect == -1; ++i)
        {
            Player* player = GAMEMANAGER->getPlayer(i);

            if (player->getActive() && !player->isDead() )
            {
                Actor * actor = player->getActor();
                if (actor)
                {
                    if (DepthRange(m_actor->getDepth()) == DepthRange(actor->getDepth()))
                    {
                        if ((m_actor->get2DPos() - actor->get2DPos()).sqrnorm() < detectRange)
                        {
                            return btrue;
                        }
                    }
                }
            }
        }
        return bfalse;
    }

    void Ray_AIBlowFishBehavior::updateScale()
    {
        const f32 scale = 1.0f  + m_scaleCursor * (getTemplate()->getInflatedScaleMultiplier() - 1.0f); 
        //m_physComponent->setRadiusMultiplier(scale);

        PhysShapeCircle * circle = m_phantomComponent->getShape()->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168));
        circle->setRadius(m_phantomInitialScale * scale);

        //m_actor->setScale(m_actor->getLocalInitialScale() * scale);
    }

    void Ray_AIBlowFishBehavior::updateStim(f32 _dt)
    {

        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(m_actor->get2DPos(),m_actor->get2DPos(), 0.0f,
            m_phantomComponent->getFactoryShapeScaled(),
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
        PunchStim punchStim;
        punchStim.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
        punchStim.setIsRadial(btrue);
        punchStim.setLevel(2);
        punchStim.setFaction(m_aiComponent->getFaction());

        punchStim.setSender(m_actor->getRef());
        punchStim.setAngle(m_actor->getAngle());
        punchStim.setPos(m_actor->get2DPos());
        punchStim.setPrevPos(m_actor->get2DPos());


        for (ITF_MAP<ActorRef,ITF_VECTOR<SCollidableContact*> >::iterator it = contactPoints.begin(); it != contactPoints.end(); ++it)
        {
            if (m_memorizedHits.find(it->first) == m_memorizedHits.end())
            {
                m_memorizedHits[it->first] = getTemplate()->getMemorizedHitTime();
                Actor * actor = it->first.getActor();
                if (actor)
                {
                    for (ITF_VECTOR<SCollidableContact*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
                        punchStim.addContact(*(*it2));
                    Vec2d dir = actor->get2DPos() - m_actor->get2DPos();
                    dir.normalize();
                    punchStim.setDirection(dir);

                    actor->onEvent(&punchStim);
                    punchStim.resetContacts();
                }
            }
        }

        ITF_MAP<ObjectRef,f32>::iterator it = m_memorizedHits.begin();
        while(it != m_memorizedHits.end())
        {
            it->second = Max(it->second-_dt,0.0f);
            if (it->second == 0.0f)
                it = m_memorizedHits.erase(it);
            else
                ++it;
        }

    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIBlowFishBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::circle(m_actor->getPos(),getTemplate()->getDetectionRange());
		DebugDraw::circle(m_actor->getPos(),getTemplate()->getDetectionCloseRange());
        DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),m_phantomComponent->getFactoryShapeScaled(),Color::green());
    }
#endif // ITF_SUPPORT_EDITOR
}
