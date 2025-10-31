#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GOLEMAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_GolemAIComponent.h"
#endif //_ITF_RAY_GOLEMAICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_RAY_BOSSBUBOAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBuboAIComponent.h"
#endif //_ITF_RAY_BOSSBUBOAICOMPONENT_H_
#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

    namespace ITF
    {
    
    IMPLEMENT_OBJECT_RTTI(Ray_GolemAIComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_GolemAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_GolemAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "This component requires an LinkComponent.");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_GolemAIComponent::Ray_GolemAIComponent()
    : m_animatedComponent(NULL)
    , m_polylineComponent(NULL)
    , m_state(STATE_GOLEM_SLEEP)
    , m_countHit_left(0)
    , m_countHit_right(0)
    , m_golemIsInit(bfalse)
    , m_phaseState(GOLEM_PHASE_1)
    , m_countDownAttack(0.0f)
    , m_golemIsWakeUp(bfalse)
    , m_timeAnticip(0.0f)
    , m_indexBone_BubonLeft(0)
    , m_indexBone_BubonRight(0)
    , m_indexBone_BubonUp(0)
    , m_bossBubonCanMove(btrue)
	, m_sequenceIndex(U32_INVALID)
    {
        // none
    }

    //*****************************************************************************

    Ray_GolemAIComponent::~Ray_GolemAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get components
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        ITF_ASSERT_MSG(m_animatedComponent, "The door MUST have an AnimatedComponent.");
        m_polylineComponent = m_actor->GetComponent<PolylineComponent>();
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        ITF_ASSERT_MSG(m_linkComponent, "The door MUST have an LinkComponent.");

        // Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172), this);


        // Spawner
        SPAWNER->declareNeedsSpawnee(m_actor, &m_eyeBrowLeft_Spawner,  getTemplate()->getActEyeBrow_Left());
        SPAWNER->declareNeedsSpawnee(m_actor, &m_eyeBrowRight_Spawner, getTemplate()->getActEyeBrow_Right());
        SPAWNER->declareNeedsSpawnee(m_actor, &m_eyeLeft_Spawner,      getTemplate()->getActEye_Left());
        SPAWNER->declareNeedsSpawnee(m_actor, &m_eyeRight_Spawner,     getTemplate()->getActEye_Right());
        SPAWNER->declareNeedsSpawnee(m_actor, &m_nose_Spawner,         getTemplate()->getActNose());



    }

    //*****************************************************************************

    void Ray_GolemAIComponent::onBecomeActive( )
    {
        Super::onBecomeActive();

        // Init
        //init();

    }

    //*****************************************************************************

    void Ray_GolemAIComponent::init()
    {

        if (!m_golemIsInit)
        {
            // Init BossBubon
            if (m_linkComponent)
            {
                const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();

                const StringID s_left = ITF_GET_STRINGID_CRC(left,2307792581);
                const StringID s_right = ITF_GET_STRINGID_CRC(right,289272363);
                const StringID s_up = ITF_GET_STRINGID_CRC(up,3605073068);
                for (LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it)
                {
                    const ChildEntry & child = *it;

                    const ObjectPath& obj = child.getPath();
                    Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,obj);
                    if (pickable)
                    {
                        if (Actor * actor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                        {
                            if (child.hasTag(s_left))
                            {
                                if (actor)
                                {
                                    m_bossBubonLeft = actor->getRef();
                                }
                            }
                            else if (child.hasTag(s_right))
                            {
                                if (actor)
                                {
                                    m_bossBubonRight = actor->getRef();
                                }
                            }
                            else if (child.hasTag(s_up))
                            {
                                if (actor)
                                {
                                    m_bossBubonUp = actor->getRef();
                                }
                            }
                        }
                    }
                }
            }
            ITF_ASSERT(m_bossBubonUp.isValid() && m_bossBubonLeft.isValid() && m_bossBubonRight.isValid());

            // Init Elements
            if (m_animatedComponent)
            {
                // Eyes
                addElement(INDEX_EYE_LEFT, &m_eyeLeft_Spawner, getTemplate()->getBoneEye_Left(), getTemplate()->getZOffsetEye_Left());
                addElement(INDEX_EYE_RIGHT, &m_eyeRight_Spawner, getTemplate()->getBoneEye_Right(), getTemplate()->getZOffsetEye_Right());

                // Arcades     
                addElement(INDEX_EYEBROW_LEFT, &m_eyeBrowLeft_Spawner, getTemplate()->getBoneEyeBrow_Left(), getTemplate()->getZOffsetEyeBrow_Left());
                addElement(INDEX_EYEBROW_RIGHT, &m_eyeBrowRight_Spawner, getTemplate()->getBoneEyeBrow_Right(), getTemplate()->getZOffsetEyeBrow_Right());

                // Nose
                addElement(INDEX_NOSE, &m_nose_Spawner, getTemplate()->getBoneNose(), getTemplate()->getZOffsetNose());

                // BossBubon
                const StringID boneNameBubonLeft = ITF_GET_STRINGID_CRC(B_BuboLeft,765297799);
                m_indexBone_BubonLeft  = m_animatedComponent->getBoneIndex(boneNameBubonLeft);

                const StringID boneNameBubonRight = ITF_GET_STRINGID_CRC(B_BuboRight,2832381573);
                m_indexBone_BubonRight  = m_animatedComponent->getBoneIndex(boneNameBubonRight);

                const StringID boneNameBubonUp = ITF_GET_STRINGID_CRC(B_BuboUp,2799290629);
                m_indexBone_BubonUp  = m_animatedComponent->getBoneIndex(boneNameBubonUp);

            }


            m_golemIsInit = btrue;
        }
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::addElement(u32 _index, SpawneeGenerator* spawnee, StringID _boneName, f32 _zOffset)
    {
        Golelement elem;

        elem.m_indexBone  = m_animatedComponent->getBoneIndex(_boneName);
        if (AnimBoneDyn* bone = m_animComponent->getBone(elem.m_indexBone))
        {
            if (Actor* actElem    = spawnee->getSpawnee(m_actor->getScene(), bone->m_Pos.to3d()))
            {
                elem.m_actor      = actElem->getRef();
                elem.m_index      = _index;
                elem.m_zOffset    = _zOffset;

                actElem->disable();

                m_listGolelement.push_back(elem);
            }
        }
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::onEvent(Event * _event)
    {
        if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            processTrigger(trigger);
        }
        else if (EventBossBubonHit* evtBossBubonHit = _event->DynamicCast<EventBossBubonHit>(ITF_GET_STRINGID_CRC(EventBossBubonHit,1018185172)))
        {
			processHit();
        }
       
    }

	//*****************************************************************************

	void Ray_GolemAIComponent::playMusic()
	{
		if (m_sequenceIndex < getTemplate()->getMusics().size())
		{
			EventPlayMusic& music = const_cast<EventPlayMusic&>(getTemplate()->getMusics()[m_sequenceIndex]);
			EVENTMANAGER->broadcastEvent(&music);
		}
	}

    //*****************************************************************************

    void Ray_GolemAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (!m_golemIsInit)
        {
            init();
        }

        if (m_golemIsWakeUp)
        {
            m_countDownAttack -= _dt;
        }


        switch (m_state)
        {
            case STATE_GOLEM_WAKEUP:
            {
                // Wait the end of wake up
                if (m_animatedComponent->isMainNodeFinished())
                {
                    // Reactive all element
                    for (u32 i = 0; i < m_listGolelement.size(); i++)
                    {
                        Golelement& curElement = m_listGolelement[i];
                        if (Actor* elemAct =  curElement.m_actor.getActor())
                        {
                            elemAct->enable();
                        }
                    }
                    m_golemIsWakeUp = btrue;
                    changeState(STATE_GOLEM_STAND);
                }
                break;
            }

            case STATE_GOLEM_STAND:
            {
                UpdatePupil(_dt);

                if (m_countDownAttack <= 0.0f)
                {
                    changeState(STATE_GOLEM_ANTICIP_ATTACK);
                }

                break;
            }


            case STATE_GOLEM_ANTICIP_ATTACK:
            {
                UpdatePupil(_dt);

                m_timeAnticip -= _dt; 

                f32 offsetTimeDisapearBubon = 1.5f; // TODO : expose ??
                ITF_ASSERT(getTemplate()->getTimeAnticipation() > offsetTimeDisapearBubon);
                if (m_timeAnticip <= offsetTimeDisapearBubon) 
                {
                    triggerBubon(bfalse);
                }


                if (m_timeAnticip <= 0.0f)
                {
                    changeState(STATE_GOLEM_ATTACK);
                }
                break;
            }


            case STATE_GOLEM_ATTACK:
            {

                UpdatePupil(_dt);

                // Wait the end of hit
                if (m_animatedComponent->isMainNodeFinished())
                {
                    changeState(STATE_GOLEM_STAND);
                }
                break;
            }

            case STATE_GOLEM_STAND_BROKEN:
                {
                    UpdatePupil(_dt);

                    //if (m_countDownAttack <= 0.0f)
                    //{
                    //    changeState(STATE_GOLEM_ANTICIP_ATTACK_BROKEN);
                    //}

                    break;
                }


            case STATE_GOLEM_ANTICIP_ATTACK_BROKEN:
            {
                UpdatePupil(_dt);

                m_timeAnticip -= _dt; 
                if (m_timeAnticip <= 0.0f)
                {
                    changeState(STATE_GOLEM_ATTACK_BROKEN);
                }
                break;
            }


            case STATE_GOLEM_ATTACK_BROKEN:
            {
                UpdatePupil(_dt);

                // Wait the end of hit
                if (m_animatedComponent->isMainNodeFinished())
                {
                    changeState(STATE_GOLEM_STAND_BROKEN);
                }
                break;
            }


            case STATE_GOLEM_HURTING:
                {
                    // Wait the end of hit
                    if (m_animatedComponent->isMainNodeFinished())
                    {
                        changeState(STATE_GOLEM_STAND);
                    }
                    break;
                }

            case STATE_GOLEM_LAST_HURTING:
                {
                    // Wait the end of last hit
                    if (m_animatedComponent->isMainNodeFinished())
                    {
                        changeState(STATE_GOLEM_STAND_BROKEN);
                    }
                    break;
                }
               

        }


       updateElement(_dt);
       updateBossBubon();


    }
  
    //*****************************************************************************

    void Ray_GolemAIComponent::setBossBubonPos(u32 _indexBone, ActorRef _bubon)
    {
        if (AnimBoneDyn* bone = m_animComponent->getBone(_indexBone))
        {
            if(Actor* bossBubonAct = _bubon.getActor())
            {
                f32 z = m_actor->getDepth() - 0.01f;
                bossBubonAct->setPos(bone->m_Pos.to3d(z));

                f32 newAngle = bone->getAngle() - MTH_PIBY2;

                bossBubonAct->setAngle(newAngle);
            }
        }
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::updateBossBubon()
    {
        setBossBubonPos(m_indexBone_BubonLeft, m_bossBubonLeft);
        setBossBubonPos(m_indexBone_BubonRight, m_bossBubonRight);
        setBossBubonPos(m_indexBone_BubonUp, m_bossBubonUp);
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::updateElement( f32 _dt )
    {
        // Update Element
        for (u32 i = 0; i < m_listGolelement.size(); i++)
        {
            Golelement& curElement = m_listGolelement[i];

            if (curElement.m_disabled)
            {
                continue;
            }

            AnimBoneDyn* bone = m_animComponent->getBone(curElement.m_indexBone);
            ITF_ASSERT(bone);
            Actor* elemAct =  curElement.m_actor.getActor();
            ITF_ASSERT(elemAct);

            f32 z = m_actor->getDepth() + curElement.m_zOffset;

            if (curElement.m_expulsed)
            {
                bbool mustDestroy = updatePosBallistics(curElement, _dt);
                z +=  curElement.m_zOffsetExpulse;
                elemAct->setPos(curElement.m_curPos.to3d(z));
                elemAct->setAngle(curElement.m_curAngle);

                if(AnimLightComponent* animLightComponent = elemAct->GetComponent<AnimLightComponent>())
                {
                    animLightComponent->setAlpha(curElement.m_alpha);
                }

                if(mustDestroy)
                {
                    elemAct->requestDestruction();
                    curElement.m_disabled = btrue;
                }

            }
            else
            {        

                elemAct->setPos(bone->m_Pos.to3d(z));
                elemAct->setAngle(bone->getAngle());

                const Vec2d& scaleGolem = m_actor->getScale();

                elemAct->setScale(scaleGolem * bone->m_Scale);                

            }


        }
    }

    //*****************************************************************************


    void Ray_GolemAIComponent::changeState( u32 _newState )
    {
 
        switch (_newState)
        {
            case STATE_GOLEM_WAKEUP:
            {
                triggerBubon(btrue);
                const StringID animWakeUp = ITF_GET_STRINGID_CRC(WakeUp,1279695844);
                playAnim(animWakeUp);
                break;
            }

            case STATE_GOLEM_STAND:
            {
                triggerBubon(btrue);
                const StringID animStand = ITF_GET_STRINGID_CRC(Stand,2617849515);
                playAnim(animStand);
                break;
            }

           
            case STATE_GOLEM_HURTING:
            {
                const StringID animHit = ITF_GET_STRINGID_CRC(Hit,2856859763);
                playAnim(animHit);
                break;
            }

            case STATE_GOLEM_ANTICIP_ATTACK:
                {
                    m_countDownAttack = getTemplate()->getCountDownAttack();
                    m_timeAnticip = getTemplate()->getTimeAnticipation();

                    const StringID animAnticip = ITF_GET_STRINGID_CRC(Anticip,2700414117);
                    playAnim(animAnticip);
                    break;
                }

            case STATE_GOLEM_ATTACK:
                {
                    const StringID animAttack = ITF_GET_STRINGID_CRC(Attack,3643922889);
                    playAnim(animAttack);
                    break;
                }

            case STATE_GOLEM_DEATH:
                {
                    // Send event for plateforme tree
                    const StringID s_Open = ITF_GET_STRINGID_CRC(Open,812432275);
                    EventGeneric genericEvent;
                    genericEvent.setId(s_Open);
                    m_actor->onEvent(&genericEvent);

                    const StringID animDeath = ITF_GET_STRINGID_CRC(Death,2142372323);
                    playAnim(animDeath);
                    break;
                }

        }

        m_state = _newState;

    }

    //*****************************************************************************

    void Ray_GolemAIComponent::UpdatePupil( f32 _dt)
    {
        Actor* closestPlayer = AIUtils::getClosestLivePlayer(m_actor);

        f32 smoothFactor = getTemplate()->getFactorSmoothPupil();

        if (closestPlayer)
        {
            Vec2d playerPos = closestPlayer->get2DPos();

            for (u32 i = 0; i < m_listGolelement.size(); i++)
            {
                Golelement& curElement = m_listGolelement[i];
            
                if (curElement.m_disabled)
                {
                    continue;
                }

                if (curElement.m_index == INDEX_EYE_LEFT || curElement.m_index == INDEX_EYE_RIGHT)
                {
                    Actor* elemAct =  curElement.m_actor.getActor();
                    ITF_ASSERT(elemAct);

                    AnimBoneDyn* bone = m_animComponent->getBone(curElement.m_indexBone);
                    ITF_ASSERT(bone);

                    Vec2d posBone = bone->m_Pos;
                    Vec2d factorEye = getPupilFactor(posBone, playerPos);

                    // Smooth
                    f32 diffPosX = curElement.m_factorEye.m_x - factorEye.m_x;
                    if (f32_Abs(diffPosX) > smoothFactor)
                    {
                        if (diffPosX < 0.f)
                        {
                            // Down
                            factorEye.m_x = curElement.m_factorEye.m_x + (f32_Abs(diffPosX) * smoothFactor);
                        }
                        else
                        {
                            // Up
                            factorEye.m_x = curElement.m_factorEye.m_x - (diffPosX * smoothFactor);
                        }
                    }

                    f32 diffPosY = curElement.m_factorEye.m_y - factorEye.m_y;
                    if (f32_Abs(diffPosY) > smoothFactor)
                    {
                        if (diffPosY < 0.f)
                        {
                            // Left
                            factorEye.m_y = curElement.m_factorEye.m_y + (f32_Abs(diffPosY) * smoothFactor);
                        }
                        else
                        {
                            // Right
                            factorEye.m_y = curElement.m_factorEye.m_y - (diffPosY * smoothFactor);
                        }
                    }



                    const StringID  inputLeftFactorH = ITF_GET_STRINGID_CRC(EyeFactorHorizontal,3166889457);
                    const StringID  inputLeftFactorV = ITF_GET_STRINGID_CRC(EyeFactorVectical,2636890976);

                    if(AnimatedComponent* animatedComponent = elemAct->GetComponent<AnimatedComponent>())
                    {
                        animatedComponent->setInput(inputLeftFactorH, factorEye.m_x);   
                        animatedComponent->setInput(inputLeftFactorV, factorEye.m_y);  

                        curElement.m_factorEye = factorEye;

                        //DebugDraw::text("H : %f / V : %f", factorEye.m_x, factorEye.m_y);

                    }

                }

            }

        }


    }

    //*****************************************************************************

    void Ray_GolemAIComponent::triggerBubon(bbool _open)
    {
        const StringID s_Open = ITF_GET_STRINGID_CRC(Open,812432275);
        const StringID s_Close = ITF_GET_STRINGID_CRC(Close,3883599349);
        ActorRef bossBubon;
        switch (m_phaseState)
        {

        case GOLEM_PHASE_1:
            {
                bossBubon = m_bossBubonRight;
                break;
            }

        case GOLEM_PHASE_2:
            {
                bossBubon = m_bossBubonLeft;
                break;
            }


        case GOLEM_PHASE_3:
            {
                bossBubon = m_bossBubonUp;
                break;
            }

        } // end switch
        if (m_linkComponent)
        {
            EventGeneric eventGen;

            if (_open)
            {
                eventGen.setId(s_Open);
            }
            else
            {
                eventGen.setId(s_Close);
            }

            eventGen.setSender(m_actor->getRef());

            if (Actor* bossBubonAct = bossBubon.getActor())
            {
                bossBubonAct->onEvent(&eventGen);
            }

            //m_linkComponent->sendEventToChildren(&eventGen);
        }

    }

    //*****************************************************************************

    Vec2d Ray_GolemAIComponent::getPupilFactor(Vec2d _from, Vec2d _to)
    {
        Vec2d vecToPlayer = ( _to - _from );

        f32 angle = vecToPlayer.normalize().getAngle();
        angle -= m_actor->getAngle();

        // Factor Horizontal
        NormalizeAnglePI( angle );
        f32 factorH = 1.0f - (f32_Abs( angle ) / MTH_PI); 

        // Factor Vertical
        angle -= MTH_PIBY2;
        NormalizeAnglePI( angle );
        f32 factorV = f32_Abs( angle ) / MTH_PI; 

        return Vec2d(factorH, factorV);
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::playAnim( const StringID _anim)
    {
        if (m_animatedComponent && _anim.isValid())
        {
            m_animatedComponent->setAnim(_anim);
        }
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::processTrigger( EventTrigger* _trigger )
    {
        if (m_state == STATE_GOLEM_SLEEP)
        {
            ObjectRef activator = _trigger->getActivator();

            if (GAMEMANAGER->isPlayerActor(activator))
            {
				m_sequenceIndex = 0;
				playMusic();

                changeState(STATE_GOLEM_WAKEUP);
            }
        }
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::processHit( )
    {

        if (m_state == STATE_GOLEM_WAKEUP && m_state == STATE_GOLEM_HURTING && m_state == STATE_GOLEM_LAST_HURTING)
        {
            return;
        }

        m_bossBubonCanMove = bfalse;

        // Set (normalize) dir expulse
        Vec2d dirExpulse = Vec2d::Right; // _evtOnHit->getDirection();
        f32 dotImpulse = dirExpulse.dot(Vec2d::Right);

        if (dotImpulse > 0.0f && dotImpulse <= 1.0f)
        {
            dirExpulse = Vec2d::Right;
            dirExpulse = dirExpulse.Rotate(GOLEM_EXPULSE_ANGLE);
        }
        else
        {
            dirExpulse = -Vec2d::Right;
            dirExpulse = dirExpulse.Rotate(-GOLEM_EXPULSE_ANGLE);
        }

        // random power
        dirExpulse *= getTemplate()->getPowerExpulse();

		m_sequenceIndex++;
		playMusic();

        switch (m_phaseState)
        {
        // -------- PHASE 1 ---------------
        case GOLEM_PHASE_1:
            {
                startExpulse(INDEX_EYEBROW_RIGHT, dirExpulse);                 
                startExpulse(INDEX_EYE_RIGHT, dirExpulse);                   

                changeState(STATE_GOLEM_HURTING);
                m_phaseState = GOLEM_PHASE_2;
                break;
            }
        
        // -------- PHASE 2 ---------------
        case GOLEM_PHASE_2:
            {
                startExpulse(INDEX_EYEBROW_LEFT, dirExpulse);                 
                startExpulse(INDEX_EYE_LEFT, dirExpulse);                   
                startExpulse(INDEX_NOSE, dirExpulse);

                changeState(STATE_GOLEM_HURTING);
                m_phaseState = GOLEM_PHASE_3;
                break;
            }

        // -------- PHASE 3 ---------------
        case GOLEM_PHASE_3:
            {
                changeState(STATE_GOLEM_DEATH);
                m_phaseState = GOLEM_PHASE_4;
                break;
            }

        }// end switch
      
    }

    //*****************************************************************************

    void Ray_GolemAIComponent::startExpulse(u32 _indexElement, Vec2d _dirExpulse)
    {

        f32 dotImpulse = _dirExpulse.dot(Vec2d::Right);
        bbool expulseToRight = bfalse;
        if (dotImpulse > 0.0f && dotImpulse <= 1.0f)
        {
            expulseToRight = btrue;
        }

        for (u32 i = 0; i < m_listGolelement.size(); i++)
        {
            Golelement& curElement = m_listGolelement[i];

            if (_indexElement == curElement.m_index && !curElement.m_expulsed)
            {
                curElement.m_originPos      = curElement.m_actor.getActor()->get2DPos();
                curElement.m_dirExpulse     = _dirExpulse;
                curElement.m_curAngle       = curElement.m_actor.getActor()->getAngle();
                curElement.m_expulsed       = btrue;
                curElement.m_expulseToRight = expulseToRight;
                return;
            }

        }

        ITF_ASSERT(0);
    }

    //*****************************************************************************

    bbool Ray_GolemAIComponent::updatePosBallistics(Golelement& _element, f32 _dt )
    {
        bbool isFinished = btrue;
        f32 m_gravity = getTemplate()->getGravityBallistics();
        f32 rotate = getTemplate()->getAngleRotateExpulse().Radians(); // MTH_PI * 0.05f; // piBy20

        Vec2d dirImpulse(1.0f, 0.0f);

        // Set ballistics pos
        if ((_element.m_timeExpulse += _dt) < getTemplate()->getTimeExpulse())
        {
            // progress
            f32 progress = _element.m_timeExpulse /  getTemplate()->getTimeExpulse();
            f32 progressAlpha = (progress - GOLEM_PERCENT_START_ALPHA) / (1.0f - GOLEM_PERCENT_START_ALPHA);
            progressAlpha = Max(0.0f, progressAlpha);
                  
            // update fade alpha
            _element.m_alpha = 1.0f - progressAlpha;
            if (_element.m_alpha < 0.0f)
            {
                _element.m_alpha = 0.0f;
            }

            // update Z
            _element.m_zOffsetExpulse = getTemplate()->getZOffsetExpulse() * progress;

            // Trajectory
            _element.m_curPos.m_x = _element.m_originPos.m_x + _element.m_dirExpulse.m_x * _element.m_timeExpulse;
            _element.m_curPos.m_y = _element.m_originPos.m_y + _element.m_dirExpulse.m_y * _element.m_timeExpulse + 0.5f * m_gravity * _element.m_timeExpulse * _element.m_timeExpulse;

            // Rotate
            if (_element.m_expulseToRight)
            {
                _element.m_curAngle -= rotate;
            }
            else
            {
                _element.m_curAngle += rotate;
            }


            //DebugDraw::circle(_element.m_curPos, 0.0f, 0.1f, Color::red(), 2.0f);

            // Z
            //curBox.m_currentExpulseZ = curBox.m_targetExpulseZ * progress;

            isFinished = bfalse;
                    
         }
 
        return isFinished;

    }


    //*****************************************************************************


#ifdef ITF_SUPPORT_EDITOR

    void Ray_GolemAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

    }

#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_GolemAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_GolemAIComponent_Template)

        SERIALIZE_MEMBER("countDownAttack",      m_countDownAttack);
        SERIALIZE_MEMBER("timeAnticipation",     m_timeAnticipation);

        SERIALIZE_MEMBER("gravityBallistics",    m_gravityBallistics);
        SERIALIZE_MEMBER("timeExpulse",          m_timeExpulse);
        SERIALIZE_MEMBER("powerExpulse",         m_powerExpulse);
        SERIALIZE_MEMBER("zOffsetExpulse",       m_zOffsetExpulse);
        SERIALIZE_MEMBER("angleRotateExpulse",   m_angleRotateExpulse);

        SERIALIZE_MEMBER("factorSmoothPupil",    m_factorSmoothPupil);

        SERIALIZE_MEMBER("actEyeBrow_Left",     m_actEyeBrow_Left);
        SERIALIZE_MEMBER("boneEyeBrow_Left",    m_boneEyeBrow_Left);
        SERIALIZE_MEMBER("zOffsetEyeBrow_Left", m_zOffsetEyeBrow_Left);

        SERIALIZE_MEMBER("actEyeBrow_Right",  m_actEyeBrow_Right);
        SERIALIZE_MEMBER("boneEyeBrow_Right", m_boneEyeBrow_Right);
        SERIALIZE_MEMBER("zOffsetEyeBrow_Right", m_zOffsetEyeBrow_Right);

        SERIALIZE_MEMBER("actEye_Left",  m_actEye_Left);
        SERIALIZE_MEMBER("boneEye_Left", m_boneEye_Left);
        SERIALIZE_MEMBER("zOffsetEye_Left", m_zOffsetEye_Left);

        SERIALIZE_MEMBER("actEye_Right",  m_actEye_Right);
        SERIALIZE_MEMBER("boneEye_Right", m_boneEye_Right);
        SERIALIZE_MEMBER("zOffsetEye_Right", m_zOffsetEye_Right);

        SERIALIZE_MEMBER("actNose",  m_actNose);
        SERIALIZE_MEMBER("boneNose", m_boneNose);
        SERIALIZE_MEMBER("zOffsetNose", m_zOffsetNose);

		SERIALIZE_CONTAINER_OBJECT("musics", m_musics);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_GolemAIComponent_Template::Ray_GolemAIComponent_Template()
    : m_gravityBallistics(-15.0)
    , m_timeExpulse(3.0)
    , m_powerExpulse(15.0f)
    , m_zOffsetEyeBrow_Left(0.01f)
    , m_zOffsetEyeBrow_Right(0.01f)
    , m_zOffsetEye_Left(0.01f)
    , m_zOffsetEye_Right(0.01f)
    , m_zOffsetNose(0.01f)
    , m_factorSmoothPupil(0.0001f)
    , m_countDownAttack(10.0f)
    , m_timeAnticipation(2.0f)
    , m_zOffsetExpulse(1.0f)
    , m_angleRotateExpulse(btrue, 10.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_GolemAIComponent_Template::~Ray_GolemAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************



}
