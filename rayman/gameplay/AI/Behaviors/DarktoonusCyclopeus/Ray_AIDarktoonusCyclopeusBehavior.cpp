#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/DarktoonusCyclopeus/Ray_AIDarktoonusCyclopeusBehavior.h"
#endif //_ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPolylinePunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_


#define STATE_ATTACK_PHASE_1          0
#define STATE_ATTACK_PHASE_2          1
#define STATE_ATTACK_PHASE_3          2

#define STATE_HIT_NOIMPACT            0
#define STATE_HIT_IMPACT              1
#define STATE_HIT_IMPACTWEAK          2


#define STATE_WAIT_WAITING            0
#define STATE_WAIT_BACK               1

#define OFFSET_Y_AIM                  0.5f

#define LENGHT_NERVE_MIN              0.5f
#define LENGHT_NERVE_MAX              8.0f

// Pupil
#define PUPIL_VICIOUS_CLAMP_MIN       0.3f
#define PUPIL_VICIOUS_CLAMP_MAX       0.58f 
#define DISTANCE_MAX_PUPIL_VIEW       50.0f

#define OFFSET_MAX_BEFORE_SMOOTH      0.0001f


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonusCyclopeusBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIDarktoonusCyclopeusBehavior)
        VALIDATE_BEHAVIOR_PARAM("idle",      m_idleAction,       "this action is mandatory"); 
        VALIDATE_BEHAVIOR_PARAM("attack",    m_attackAction,     "this action is mandatory"); 
        VALIDATE_BEHAVIOR_PARAM("waitHit",   m_waitHitAction,    "this action is mandatory"); 
    END_VALIDATE_BEHAVIOR()

    //*****************************************************************************

    Ray_AIDarktoonusCyclopeusBehavior::Ray_AIDarktoonusCyclopeusBehavior()
    : m_idleAction(NULL)
    , m_prepareFocusAction(NULL)
    , m_focusAction(NULL)
    , m_attackAction(NULL)
    , m_waitHitAction(NULL)
    , m_stateAttack(STATE_ATTACK_PHASE_1)
    , m_stateImpact(STATE_HIT_NOIMPACT)
    , m_stateWait(STATE_WAIT_WAITING)
    , m_angleAttack(0.0f)
    , m_distAttack(0.0f)
    , m_distProgress(0.0f)
    , m_distAttackInput(0.0f)
    , m_hasContact(bfalse)
    , m_transitionIsFinish(bfalse)
    , m_countDownAttack(0.0f)
    , m_countDownWait(0.0f)
    , m_countDownFocus(0.0f)
    , m_prevFactorH(0.0f)
    , m_prevFactorV(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_AIDarktoonusCyclopeusBehavior::~Ray_AIDarktoonusCyclopeusBehavior()
    {
        // none
    }

    //*****************************************************************************

    void Ray_AIDarktoonusCyclopeusBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {        
        Super::onActorLoaded(_hotReload);

        // Create Actions
        m_idleAction            = createAiActionAndCast<AIPlayAnimAction>(getTemplate()->getIdleAction());
        m_prepareFocusAction    = createAiActionAndCast<AIPlayAnimAction>(getTemplate()->getPrepareFocusAction());
        m_focusAction           = createAiActionAndCast<AIPlayAnimAction>(getTemplate()->getFocusAction());
        m_attackAction          = createAiActionAndCast<Ray_AIPerformHitPolylinePunchAction>(getTemplate()->getAttackAction());
        m_waitHitAction         = createAiActionAndCast<AIPlayAnimAction>(getTemplate()->getWaitHitAction());

        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStimNotify,1337973749),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
    }

    //*****************************************************************************

    void Ray_AIDarktoonusCyclopeusBehavior::onActivate()
    {
        //Super::onActivate();
        setAction(m_idleAction); 
    }

    //*****************************************************************************

    void Ray_AIDarktoonusCyclopeusBehavior::onDeactivate()
    {
        Super::onDeactivate();
    }
 
    //*****************************************************************************

    void Ray_AIDarktoonusCyclopeusBehavior::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if (EventStimNotify* trigger = _event->DynamicCast<EventStimNotify>(ITF_GET_STRINGID_CRC(EventStimNotify,1337973749)))
        {
            m_hasContact = btrue;
        }
        else if (AnimGameplayEvent* trigger = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (trigger->getName() == MRK_Finish_Anim)
            {
                m_transitionIsFinish = btrue;
            }
        }  
    }

    //*****************************************************************************

    void Ray_AIDarktoonusCyclopeusBehavior::update(f32 _dt)
    {

        Super::update(_dt);

        if(m_currentAction == m_idleAction)
        {
            if (enemiesIsDetectForAttack())
            {
                if (m_countDownAttack <= 0.0f)
                {
                    m_countDownAttack   = getTemplate()->getCountDownAttack();
                    setAction(m_prepareFocusAction);
                }
                else
                {
                    m_countDownAttack -= _dt;
                    if (m_countDownAttack < 0.0f)
                    {
                        m_countDownAttack = 0.0f;
                    }
                }
            }
            else
            {
                m_countDownAttack = getTemplate()->getCountDownAttack();
            }
        }
        else if (m_currentAction == m_focusAction)
        {
            if (m_countDownFocus <= 0.0f)
            {
                m_countDownFocus = getTemplate()->getTimeFocus();
                setAction(m_attackAction);
            }
            else
            {
                m_countDownFocus -= _dt;
                if (m_countDownFocus < 0.0f)
                {
                    m_countDownFocus = 0.0f;
                }
            }
        }
        else if (m_currentAction == m_attackAction)
        {
            if (m_stateAttack == STATE_ATTACK_PHASE_1)
            {
                if (m_distProgress < LENGHT_NERVE_MAX)
                {
                    if (m_hasContact)
                    {
                        // Exit by contact
                        m_transitionIsFinish    = bfalse;

                        if (m_distProgress <= LENGHT_NERVE_MIN)
                        {
                            m_stateImpact = STATE_HIT_IMPACTWEAK;
                        }
                        else
                        {
                            m_stateImpact = STATE_HIT_IMPACT;
                        }

                        m_stateAttack = STATE_ATTACK_PHASE_2;
                    }
                    else
                    {
                        if (m_transitionIsFinish)
                        {
                            m_distProgress += getTemplate()->getSpeed() * _dt;
                            m_distAttackInput = m_distProgress;
                        }
                    }
                }
                else
                {
                    // Exit by end trajectory
                    m_transitionIsFinish    = bfalse;
                    m_stateImpact           = STATE_HIT_NOIMPACT;
                    m_stateAttack           = STATE_ATTACK_PHASE_2;
                }
            }
            else if (m_stateAttack == STATE_ATTACK_PHASE_2)
            {
                f32 newDist = m_distProgress - getTemplate()->getSpeedBack() * _dt;
                if (newDist > LENGHT_NERVE_MIN)
                {
                    if (m_transitionIsFinish)
                    {
                        m_distProgress = newDist;
                        m_distAttackInput = m_distProgress;
                    }
                }
                else
                {
                    m_stateAttack = STATE_ATTACK_PHASE_3;
                }
            }

        }
        else if (m_currentAction == m_waitHitAction)
        {
            if (m_countDownWait <= 0.0f && m_stateWait == STATE_WAIT_WAITING)
            {
                m_countDownWait = getTemplate()->getTimeWaiting();
                m_stateWait = STATE_WAIT_BACK;
            }
            else
            {
                m_countDownWait -= _dt;
                if (m_countDownWait < 0.0f)
                {
                    m_countDownWait = 0.0f;
                }
            }
        }
    }

    //*****************************************************************************

    void Ray_AIDarktoonusCyclopeusBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        // Update input state attack      
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(AttackState,1958569374), m_stateAttack);

        // Update input state attack      
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(WaitState,4164429204), m_stateWait);

        // Input nerve
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(LenghtNerve,2020219986), m_distAttackInput);

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(RotationNerve,3289575953), Angle::Radians(m_angleAttack).ToDegrees360());

        // Input hit
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(ImpactState,2110744201), m_stateImpact);

    }

    // ****************************************************************************

    void Ray_AIDarktoonusCyclopeusBehavior::onActionFinished()
    {
        if (m_currentAction == m_prepareFocusAction)
        {
            m_countDownFocus    = getTemplate()->getTimeFocus();
            setAction(m_focusAction);
        }
        else if (m_currentAction == m_focusAction)
        {
            m_stateAttack = STATE_ATTACK_PHASE_1;
        }
        else if (m_currentAction == m_attackAction)
        {
            // Reset
            m_distAttack            = 0.0f;
            m_distProgress          = 0.0f;
            m_transitionIsFinish    = bfalse;
            m_hasContact            = bfalse;
            m_stateAttack           = STATE_ATTACK_PHASE_1;
            m_stateImpact           = STATE_HIT_NOIMPACT;
            m_stateWait             = STATE_WAIT_WAITING;
            m_countDownWait         = getTemplate()->getTimeWaiting();
            m_countDownFocus        = getTemplate()->getTimeFocus();

            setAction(m_waitHitAction);
        }
        else if(m_currentAction == m_waitHitAction)
        {
            setAction(m_idleAction);
        }

    }

    //*****************************************************************************

    ActorRef Ray_AIDarktoonusCyclopeusBehavior::getClosestEnemies(f32 _range, bbool _mustStickOnPoly)
    {
        Actor* enemie = AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), _range);

        if (enemie)
        {
            return enemie->getRef();
        }

        return ObjectRef::InvalidRef; 

/*

        Actor* player = NULL;

        //if (_mustStickOnPoly)
        //{
            // Get the closet player sticky on polyline
            //ActorList playerListSorted;
            //AIUtils::getSortedPlayersInRange(m_actor, _range, playerListSorted);

            ActorList enemiesListSorted;
            AIUtils::getSortedEnemiesInRange(m_actor, _range, AIUtils::getFaction(m_actor), enemiesListSorted);


            for (u32 i = 0; i < enemiesListSorted.size(); i++)
            {
                //if (queryIsStickOnPolyline(playerListSorted[i]))
                //{

                //bbool playerIsValid = btrue;
                Actor* curPlayer = enemiesListSorted[i];

                // Ghost check
                //if (!getTemplate()->getCanAttackGhost() && queryIsDead(curPlayer))
                //{
                //    playerIsValid = bfalse;
                //}

                //if (playerIsValid)
                //{
                    player = curPlayer;
                    break;
                //}


                //}
            }
        //}
        //else
        //{
        //    // Just get the closet player
        //    player = AIUtils::getLeadPlayer(m_actor);
        //}
 

        if (player)
        {
            return player->getRef();
        }

        return ObjectRef::InvalidRef; 

        */

    }

    //*****************************************************************************

    bbool Ray_AIDarktoonusCyclopeusBehavior::enemiesIsDetectForAttack()
    {

        ActorRef playerDetect = getClosestEnemies(getTemplate()->getDistanceDetect(), getTemplate()->getMustBeStickToAttack());

        if (playerDetect.isValid())
        {
            Actor* player = playerDetect.getActor();

            if (player)
            {
                Vec2d playerPos = player->get2DPos();
                playerPos.m_y += OFFSET_Y_AIM;

                Vec2d vecToPlayer = (playerPos - m_actor->get2DPos());
                
                // Compute distance attack
                f32 distAttack = vecToPlayer.norm();
                           
                // Eye orientation
                Vec2d dirEye = Vec2d::Right.Rotate(m_actor->getAngle());


                vecToPlayer.normalize();
                dirEye.normalize();

                // Can See ?
                f32 dotDir;
                if (m_actor->isFlipped())
                {
                    dotDir = (-dirEye).dot(vecToPlayer);
                }
                else
                {
                     dotDir = dirEye.dot(vecToPlayer);
                }

                bbool canSee = bfalse;
                if (dotDir >= 0.0f)
                {
                    if (f32_ACos(dotDir) <= getTemplate()->getAngleMaxDetect().Radians())
                    {
                        canSee = btrue;
                    }
                }

                // Compute attack angle
                f32 angleAttack = vecToPlayer.getOrientedAngle(dirEye); 
                if (m_actor->isFlipped()) 
                {
                    angleAttack = MTH_PI - angleAttack;
                }

                // Check if this pos is valid for detection
                if (distAttack <= getTemplate()->getDistanceDetect() && canSee)
                {
                    m_distAttack    = distAttack;
                    m_angleAttack   = angleAttack;

                    return btrue;
                }
            
            }

        }

        return bfalse;
    }

    //*****************************************************************************

    void Ray_AIDarktoonusCyclopeusBehavior::UpdatePupil( f32 _dt)
    {

        if (m_currentAction == m_prepareFocusAction || m_currentAction == m_focusAction)
        {
            return;
        }

        ActorRef playerDetect = getClosestEnemies(DISTANCE_MAX_PUPIL_VIEW, bfalse);

        if (playerDetect.isValid())
        {
            Actor* player = playerDetect.getActor();

            if (player)
            {
                Vec2d eyePos = m_actor->get2DPos();

                Vec2d playerPos = player->get2DPos();
                playerPos.m_y += OFFSET_Y_AIM;

                Vec2d vecToPlayer = (playerPos - eyePos );

                f32 angle = vecToPlayer.normalize().getAngle();
                angle -= m_actor->getAngle();

                // Factor Horizontal
                NormalizeAnglePI( angle );
                f32 factorH = 1.0f - (f32_Abs( angle ) / MTH_PI); 

                // Factor Vertical
                angle -= MTH_PIBY2;
                NormalizeAnglePI( angle );
                f32 factorV = f32_Abs( angle ) / MTH_PI; 

                // If flipped
                if (m_actor->isFlipped())
                {
                    factorH = 1.0f - factorH;
                } 

                // Clamp for vicious stand
                if (m_currentAction == m_idleAction)
                {
                    factorV =  Clamp(factorV, PUPIL_VICIOUS_CLAMP_MIN, PUPIL_VICIOUS_CLAMP_MAX);
                }


                f32 diffPos = m_prevFactorV - factorV;
                if (f32_Abs(diffPos) > OFFSET_MAX_BEFORE_SMOOTH)
                {
                    if (diffPos < 0.f)
                    {
                        // Down
                        factorV = m_prevFactorV + (f32_Abs(diffPos) * 0.3f);
                    }
                    else
                    {
                        // Up
                        factorV = m_prevFactorV - (diffPos * 0.3f);
                    }
                }
           
                // save prev
                m_prevFactorH = factorH;
                m_prevFactorV = factorV;

                // Set input factor pupil
                m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(EyeFactorHorizontal,3166889457), factorH);   
                m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(EyeFactorVectical,2636890976), factorV);  


            }

        }

    }

    //*****************************************************************************

    bbool Ray_AIDarktoonusCyclopeusBehavior::queryIsStickOnPolyline(Actor* _player)
    {
        if (_player)
        {
            EventQueryPhysicsData query;
            _player->onEvent(&query);
            if (query.getStickyPolyline().isValid())
            {
                return btrue;
            }
        }

        return bfalse;
    }

    //*****************************************************************************

    bbool Ray_AIDarktoonusCyclopeusBehavior::queryIsDead(Actor* _player)
    {
        if (_player)
        {
            EventQueryIsDead query;
            _player->onEvent(&query);
            return query.getIsDead();
        }

        return bfalse;
    }

    //*****************************************************************************

    bbool Ray_AIDarktoonusCyclopeusBehavior::isHitable() const
    {
        if ((m_currentAction == m_waitHitAction && m_stateWait == STATE_WAIT_WAITING)
            || m_currentAction == m_prepareFocusAction
            || m_currentAction == m_focusAction) 
        {
            return btrue;
        }

        return bfalse;
    }

#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_AIDarktoonusCyclopeusBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (isHitable())
        {
            DebugDraw::circle(m_actor->getPos(), 2.0f, Color::green(), 0.0f);
        }

        DebugDraw::circle(m_actor->getPos(), getTemplate()->getDistanceDetect(), Color::red(), 0.0f);
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonusCyclopeusBehavior_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_AIDarktoonusCyclopeusBehavior_Template)

        SERIALIZE_OBJECT_WITH_FACTORY("idle",           m_idleAction,           ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("prepareFocus",   m_prepareFocusAction,   ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("focus",          m_focusAction,          ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("attack",         m_attackAction,         ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("waitHit",        m_waitHitAction,        ACTORSMANAGER->getAIActionsFactory());

        SERIALIZE_MEMBER("distanceDetect",      m_distanceDetect);
        SERIALIZE_MEMBER("angleMaxDetect",      m_angleMaxDetect);
        SERIALIZE_MEMBER("countDownAttack",     m_countDownAttack);
        SERIALIZE_MEMBER("speed",               m_speed);
        SERIALIZE_MEMBER("speedBack",           m_speedBack);
        SERIALIZE_MEMBER("timeWaiting",         m_timeWaiting);
        SERIALIZE_MEMBER("timeFocus",           m_timeFocus);


        SERIALIZE_MEMBER("mustBeStickToAttack", m_mustBeStickToAttack);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_AIDarktoonusCyclopeusBehavior_Template::Ray_AIDarktoonusCyclopeusBehavior_Template()
    : m_idleAction(NULL)
    , m_prepareFocusAction(NULL)
    , m_focusAction(NULL)
    , m_attackAction(NULL)
    , m_waitHitAction(NULL)
    , m_distanceDetect(10.0f)
    , m_angleMaxDetect(btrue, 70.0f)
    , m_countDownAttack(2.0f)
    , m_speed(30.0f)
    , m_speedBack(40.0f)
    , m_timeWaiting(2.0f)
    , m_timeFocus(0.5f)
    , m_mustBeStickToAttack(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_AIDarktoonusCyclopeusBehavior_Template::~Ray_AIDarktoonusCyclopeusBehavior_Template()
    {
        SF_DEL(m_idleAction);
        SF_DEL(m_prepareFocusAction);
        SF_DEL(m_focusAction);
        SF_DEL(m_attackAction);
        SF_DEL(m_waitHitAction);
    }


}


