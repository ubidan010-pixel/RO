#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHRONOAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_ChronoAIComponent.h"
#endif //_ITF_RAY_CHRONOAICOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_RAY_TALKINGBUBBLECOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingBubbleComponent.h"
#endif //_ITF_RAY_TALKINGBUBBLECOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_TimeAttackHUDTimerComponent.h"
#endif //_ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_

#ifdef ITF_SUPPORT_CHEAT
#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_
#endif // ITF_SUPPORT_CHEAT

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifndef _ITF_RAY_MURPHYASSISTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MurphyAssistAIComponent.h"
#endif //_ITF_RAY_MURPHYASSISTAICOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_ChronoAIComponent)
BEGIN_SERIALIZATION_CHILD(Ray_ChronoAIComponent)

END_SERIALIZATION()

Ray_ChronoAIComponent::Ray_ChronoAIComponent()
: m_animatedComponent(NULL)
, m_state(CHRONOSTATE_IDLE)
, m_bubbleRef(ITF_INVALID_OBJREF)
, m_bubbleBoneIndex(U32_INVALID)
, m_moveCounter(0.f)
, m_shake(bfalse)
, m_murphyRef(ITF_INVALID_OBJREF)
{
}

Ray_ChronoAIComponent::~Ray_ChronoAIComponent()
{
}

void Ray_ChronoAIComponent::onActorClearComponents()
{
    Actor* bubbleActor = m_bubbleRef.getActor();

    if (bubbleActor && !bubbleActor->isDestructionRequested())
    {
        Scene* bubbleScene = bubbleActor->getScene();

        if (bubbleScene)
        {
            bubbleScene->unregisterPickable(bubbleActor);
        }
    }

    if ( RAY_GAMEMANAGER->getTimeAttackActorChrono() == m_actor->getRef() )
    {
        RAY_GAMEMANAGER->setTimeAttackActorChrono(ObjectRef::InvalidRef);
    }
}

void Ray_ChronoAIComponent::onBecomeInactive()
{
    Actor* bubbleActor = m_bubbleRef.getActor();

    if ( bubbleActor )
    {
        return bubbleActor->disable();
    }
}

void Ray_ChronoAIComponent::startTimeAttack()
{

    Actor* flag = RAY_GAMEMANAGER->getTimeAttackActorFlag().getActor();

    if ( flag )
    {
        flag->enable();
    }

    Actor* timer = RAY_GAMEMANAGER->getTimeAttackActorTimer().getActor();

    if ( timer )
    {
        timer->enable();
    }

    RAY_GAMEMANAGER->setIsTimeAttack(btrue);



#ifdef ITF_SUPPORT_ONLINETRACKING
				
	RAY_GAMEMANAGER->getOnlineTrackingManager()->m_currentLevelTime = 0.0f;


#endif

}

void Ray_ChronoAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }

    if ( !getTemplate()->getBubblePath().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_bubbleSpawner, getTemplate()->getBubblePath());

        Actor* bubbleActor = m_bubbleSpawner.getSpawnee(m_actor->getScene(),m_actor->getPos(),m_actor->getAngle());

        if ( bubbleActor )
        {
            m_bubbleRef = bubbleActor->getRef();

            Ray_TalkingBubbleComponent* bubble = bubbleActor->GetComponent<Ray_TalkingBubbleComponent>();

            if ( bubble )
            {
                StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
                i32 timeAttackThreshold = RAY_GAMEMANAGER->getTimeAttackThreshold(tag, bfalse);
                i32 timeAttackThresholdForCup = RAY_GAMEMANAGER->getTimeAttackThreshold(tag, btrue);

                if ( timeAttackThreshold == -1 )
                {
                    timeAttackThreshold = 90;
                }

                if ( timeAttackThresholdForCup == -1 )
                {
                    timeAttackThresholdForCup = 60;
                }

                i32 totalMin = timeAttackThreshold/60;
                i32 totalSec = timeAttackThreshold-(60*totalMin);
                i32 cupMin = timeAttackThresholdForCup/60;
                i32 cupSec = timeAttackThresholdForCup-(60*cupMin);
                String8 totalTimeStr, cupTimeStr;
                PathString8_t cupPath, electoonPath;

                getTemplate()->getCupPath().getString(cupPath);
                getTemplate()->getElectoonPath().getString(electoonPath);

                ITF_VECTOR <TextInfo> textInfos;

                textInfos.resize(1);

                TextInfo& textInfo = textInfos[0];
                textInfo.m_color = getTemplate()->getTextColor().getAsU32();
                textInfo.m_size = getTemplate()->getInitialFontHeight();
                textInfo.m_text.setTextFormat("[actor:%s] %i:%02i \n [actor:%s] %i:%02i",electoonPath.cStr(),totalMin,totalSec,cupPath.cStr(),cupMin,cupSec);

                bubble->setSentencesStr(textInfos);
                bubble->setAppear();
            }

            bubbleActor->disable();
        }
    }

    RAY_GAMEMANAGER->setTimeAttackActorChrono(m_actor->getRef());

    if (!getTemplate()->getMurphyPath().isEmpty()) {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_murphySpawner, getTemplate()->getMurphyPath());

        Vec3d offset = getTemplate()->getMurphyOffset();
        Actor* murphyActor = m_murphySpawner.getSpawnee(m_actor->getScene(), m_actor->getPos() + offset, m_actor->getAngle());

        if (murphyActor)
        {
            m_murphyRef = murphyActor->getRef();

            RAY_GAMEMANAGER->setMurphyAssist(murphyActor->getRef());
        }
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
}

void Ray_ChronoAIComponent::onResourceReady()
{
    Super::onResourceReady();

    if ( getTemplate()->getBubbleBone().isValid() )
    {
        m_bubbleBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getBubbleBone());
    }
}

Ray_TalkingBubbleComponent* Ray_ChronoAIComponent::getBubbleComponent() const
{
    Actor* bubbleActor = m_bubbleRef.getActor();

    if ( bubbleActor )
    {
        return bubbleActor->GetComponent<Ray_TalkingBubbleComponent>();
    }

    return NULL;
}

void Ray_ChronoAIComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    updateBubble();
    updateShake(_dt);
}

void Ray_ChronoAIComponent::startMovement()
{
    m_state = CHRONOSTATE_SHAKE;

    Ray_TalkingBubbleComponent* bubble = getBubbleComponent();

    if ( bubble )
    {
        bubble->setDisappear();
    }

    m_splineMovement.clear();

    f32 moveDuration = getTemplate()->getMoveDuration();

    if ( moveDuration > 0.f )
    {
        Actor* timerActor = RAY_GAMEMANAGER->getTimeAttackActorTimer().getActor();

        if ( timerActor )
        {
            Ray_TimeAttackHUDTimerComponent* timer = timerActor->GetComponent<Ray_TimeAttackHUDTimerComponent>();
            Vec3d timerPos;
            Vec3d myPos2D;

            if ( timer )
            {
                timerPos = timer->getTimerBonePos().to3d(timerActor->getPos().m_z);
            }
            else
            {
                timerPos = timerActor->getPos();
            }
            
            GFX_ADAPTER->compute3DTo2D(m_actor->getPos(), myPos2D);

            m_splineMovement.AddSplinePoint(myPos2D,Spline::interp_spline,0.f);

            Vec3d dirToPos = timerActor->getPos() - myPos2D;
            Vec3d middle = myPos2D + ( dirToPos * 0.5f );

            dirToPos.normalize();

            middle += dirToPos.getPerpendicular() * getTemplate()->getMoveCurve();

            m_splineMovement.AddSplinePoint(middle,Spline::interp_spline,moveDuration*0.5f);
            m_splineMovement.AddSplinePoint(timerPos,Spline::interp_spline,moveDuration);
            m_moveCounter = 0.f;

            RAY_GAMEMANAGER->triggerStartTimeAttackSequence(moveDuration);
        }
    }
}

void Ray_ChronoAIComponent::updateShake( f32 _dt )
{
    if ( m_shake )
    {
        startMovement();
        m_shake = bfalse;
    }
    else if ( m_state == CHRONOSTATE_SHAKE )
    {
        Vec3d pos2D;
        Vec3d newPos3D;
        f32 moveDuration = getTemplate()->getMoveDuration();

        m_moveCounter = Min(m_moveCounter+_dt,moveDuration);

        m_splineMovement.GetInterpolatedAtTime(m_moveCounter,pos2D);

        GFX_ADAPTER->compute2DTo3D(pos2D.truncateTo2D(),m_actor->getPos().m_z,newPos3D);

        m_actor->setPos(newPos3D);

        if ( m_moveCounter >= moveDuration )
        {
            m_state = CHRONOSTATE_DISAPPEAR;
        }
    }
}

void Ray_ChronoAIComponent::updateBubble()
{
    Actor* bubble = m_bubbleRef.getActor();

    if ( bubble )
    {
        if ( !bubble->isEnabled() )
        {
            bubble->enable();
        }

        if ( m_bubbleBoneIndex != U32_INVALID )
        {
            Vec3d bonePos;
            Vec2d boneOrientation;

            if ( m_animatedComponent->getBonePos(m_bubbleBoneIndex,bonePos) )
            {
                bonePos.m_z = m_actor->getPos().m_z;
                bubble->setPos(bonePos);
            }

            if ( m_animatedComponent->getBoneOrientation(m_bubbleBoneIndex,boneOrientation) )
            {
                Ray_TalkingBubbleComponent* bubbleC = getBubbleComponent();

                if ( bubbleC )
                {
                    f32 angle = boneOrientation.getAngleOpt();

                    NormalizeAnglePI(angle);

                    f32 t = fabsf(angle)/MTH_PI;

                    bubbleC->setBubbleHeight(t);
                }
            }
        }
    }
}

void Ray_ChronoAIComponent::updateAnimInput()
{
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(state,1676610205),m_state);
}

void Ray_ChronoAIComponent::onEvent( Event* _event )
{
    if ( PunchStim* punchStim = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
    {
        if ( RAY_GAMEMANAGER->getPlayerFromActor(punchStim->getSender(),bfalse) != NULL &&
            !RAY_GAMEMANAGER->isPlayingPageSequence() )
        {
            m_shake = btrue;
        }
    }
    else if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Disappear,3214062102) )
        {
            startTimeAttack();
            m_actor->disable();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_ChronoAIComponent_Template)
BEGIN_SERIALIZATION(Ray_ChronoAIComponent_Template)

    SERIALIZE_MEMBER("bubbleBone",m_bubbleBone);
    SERIALIZE_MEMBER("bubblePath",m_bubbleAct);
    SERIALIZE_MEMBER("cupPath",m_cupPath);
    SERIALIZE_MEMBER("electoonPath",m_electoonPath);
    SERIALIZE_MEMBER("textColor",m_textColor);
    SERIALIZE_MEMBER("fontInitialHeight",m_fontInitialHeight);
    SERIALIZE_MEMBER("moveDuration",m_moveDuration);
    SERIALIZE_MEMBER("moveCurve",m_moveCurve);
    SERIALIZE_MEMBER("murphyPath", m_murphyAct);
    SERIALIZE_MEMBER("murphyOffset", m_murphyOffset);

END_SERIALIZATION()

Ray_ChronoAIComponent_Template::Ray_ChronoAIComponent_Template()
: m_textColor(Color::white())
, m_fontInitialHeight(1.f)
, m_moveDuration(0.5f)
, m_moveCurve(100.f)
, m_murphyOffset(0, 0, 0)
{
}

Ray_ChronoAIComponent_Template::~Ray_ChronoAIComponent_Template()
{
}

}
