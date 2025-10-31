#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_TimeAttackHUDTimerComponent.h"
#endif //_ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_TimeAttackHUDTimerComponent)
BEGIN_SERIALIZATION_CHILD(Ray_TimeAttackHUDTimerComponent)

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_TimeAttackHUDTimerComponent)

    VALIDATE_COMPONENT_PARAM("AnimatedComponent", m_animatedComponent != NULL, "AnimatedComponent is mandatory");
    VALIDATE_COMPONENT_PARAM("SoundComponent", m_soundComponent != NULL, "SoundComponent is mandatory");
    VALIDATE_COMPONENT_PARAM("textActorFile", !getTemplate()->getTextActorFile().isEmpty(), "A text actor file (.act) must be defined");

END_VALIDATE_COMPONENT()

Ray_TimeAttackHUDTimerComponent::Ray_TimeAttackHUDTimerComponent()
: m_animatedComponent(NULL)
, m_soundComponent(NULL)
, m_electoon(OBJECTIVESTATE_HIDE)
, m_cup(OBJECTIVESTATE_HIDE)
, m_electoonTime(0)
, m_cupTime(0)
, m_chronoBoneIndex(U32_INVALID)
, m_timerTextBoneIndex(U32_INVALID)
, m_electoonTextBoneIndex(U32_INVALID)
, m_cupTextBoneIndex(U32_INVALID)
, m_timerTextActor(ITF_INVALID_OBJREF)
, m_electoonTextActor(ITF_INVALID_OBJREF)
, m_cupTextActor(ITF_INVALID_OBJREF)
, m_playingCritic(ITF::SoundHandle::getInvalidHandle())
{
}

Ray_TimeAttackHUDTimerComponent::~Ray_TimeAttackHUDTimerComponent()
{
}

void Ray_TimeAttackHUDTimerComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if ( RAY_GAMEMANAGER->getTimeAttackActorTimer() == m_actor->getRef() )
    {
        RAY_GAMEMANAGER->setTimeAttackActorTimer(ObjectRef::InvalidRef);
    }

    deleteTextActors();
    stopCriticSound();
}

void Ray_TimeAttackHUDTimerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
        m_animatedComponent->setRank2D(UI_TEXTMANAGER->getScoreAnimDepthRank());
    }

    m_soundComponent = m_actor->GetComponent<SoundComponent>();

    RAY_GAMEMANAGER->setTimeAttackActorTimer(m_actor->getRef());

    m_textTimer.m_color = getTemplate()->getColorNormal().getAsU32();
    m_textTimer.m_size = getTemplate()->getTimerTextSize();
    m_textElectoon.m_color = getTemplate()->getColorNormal().getAsU32();
    m_textElectoon.m_size = getTemplate()->getPrizeTextSize();
    m_textCup.m_color = getTemplate()->getColorNormal().getAsU32();
    m_textCup.m_size = getTemplate()->getPrizeTextSize();

    spawnTextActors();
    updatePosition();

     m_actor->setLocalInitialPos(m_actor->getPos());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
}

void Ray_TimeAttackHUDTimerComponent::spawnTextActors()
{
    if ( !getTemplate()->getTextActorFile().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_spawner, getTemplate()->getTextActorFile());

        Actor* timerActor = m_spawner.getSpawnee( m_actor->getScene(),m_actor->getPos());

        if ( timerActor )
        {
            UITextBox* textBox = timerActor->GetComponent<UITextBox>();

            if ( textBox )
            {
                textBox->addTextFromStr(m_textTimer);
            }

            m_timerTextActor = timerActor->getRef();
            timerActor->disable();
        }

        Actor* electoonActor = m_spawner.getSpawnee( m_actor->getScene(),m_actor->getPos());

        if ( electoonActor )
        {
            UITextBox* textBox = electoonActor->GetComponent<UITextBox>();

            if ( textBox )
            {
                textBox->addTextFromStr(m_textElectoon);
            }

            m_electoonTextActor = electoonActor->getRef();
            electoonActor->disable();
        }

        Actor* cupActor = m_spawner.getSpawnee( m_actor->getScene(),m_actor->getPos());

        if ( cupActor )
        {
            UITextBox* textBox = cupActor->GetComponent<UITextBox>();

            if ( textBox )
            {
                textBox->addTextFromStr(m_textCup);
            }

            m_cupTextActor = cupActor->getRef();
            cupActor->disable();
        }
    }
}

void Ray_TimeAttackHUDTimerComponent::deleteTextActors()
{
    Actor* timerActor = m_timerTextActor.getActor();

    if ( timerActor )
    {
        timerActor->requestDestruction();
    }

    Actor* electoonActor = m_electoonTextActor.getActor();

    if ( electoonActor )
    {
        electoonActor->requestDestruction();
    }

    Actor* cupActor = m_cupTextActor.getActor();

    if ( cupActor )
    {
        cupActor->requestDestruction();
    }
}

void Ray_TimeAttackHUDTimerComponent::onResourceReady()
{
    if ( m_animatedComponent )
    {
        m_chronoBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getBoneChrono());
        m_timerTextBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getBoneTimer());
        m_electoonTextBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getBoneElectoons());
        m_cupTextBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getBoneCup());
    }
}

void Ray_TimeAttackHUDTimerComponent::onBecomeActive()
{
    StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
    m_electoonTime = RAY_GAMEMANAGER->getTimeAttackThreshold(tag, bfalse);
    m_cupTime = RAY_GAMEMANAGER->getTimeAttackThreshold(tag, btrue);

    if ( m_electoonTime == -1 )
    {
        m_electoonTime = 90;
    }

    if ( m_cupTime == -1 )
    {
        m_cupTime = 60;
    }

    i32 totalMin = m_electoonTime/60;
    i32 totalSec = m_electoonTime-(60*totalMin);
    i32 cupMin = m_cupTime/60;
    i32 cupSec = m_cupTime-(60*cupMin);

    m_electoon = OBJECTIVESTATE_SHOW;
    m_cup = OBJECTIVESTATE_SHOW;

    m_textTimer.m_text = "0:00";
    m_textTimer.m_color = getTemplate()->getColorNormal().getAsU32();
    
    m_textElectoon.m_text.setTextFormat("%i:%02i",totalMin,totalSec);
    m_textElectoon.m_color = getTemplate()->getColorNormal().getAsU32();

    m_textCup.m_text.setTextFormat("%i:%02i",cupMin,cupSec);
    m_textCup.m_color = getTemplate()->getColorNormal().getAsU32();

    m_animatedComponent->setAnim(getTemplate()->getAppearAnim());

    Actor* timerActor = m_timerTextActor.getActor();

    if ( timerActor )
    {
        timerActor->enable();

        UITextBox* textBox = timerActor->GetComponent<UITextBox>();

        if ( textBox )
        {
            textBox->setTextAt(m_textTimer,0);
        }
    }

    Actor* electoonActor = m_electoonTextActor.getActor();

    if ( electoonActor )
    {
        electoonActor->enable();

        UITextBox* textBox = electoonActor->GetComponent<UITextBox>();

        if ( textBox )
        {
            textBox->setTextAt(m_textElectoon,0);
        }
    }

    Actor* cupActor = m_cupTextActor.getActor();

    if ( cupActor )
    {
        cupActor->enable();

        UITextBox* textBox = cupActor->GetComponent<UITextBox>();

        if ( textBox )
        {
            textBox->setTextAt(m_textCup,0);
        }
    }

    updatePosition();
}

void Ray_TimeAttackHUDTimerComponent::onBecomeInactive()
{
    Actor* timerActor = m_timerTextActor.getActor();

    if ( timerActor )
    {
        timerActor->disable();
    }

    Actor* electoonActor = m_electoonTextActor.getActor();

    if ( electoonActor )
    {
        electoonActor->disable();
    }

    Actor* cupActor = m_cupTextActor.getActor();

    if ( cupActor )
    {
        cupActor->disable();
    }

    stopCriticSound();
}

void Ray_TimeAttackHUDTimerComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    updateTimer();
    updatePosition();
    updateTexts();
}

void Ray_TimeAttackHUDTimerComponent::updatePosition()
{
    f32 screenWidth = static_cast<f32>(GFX_ADAPTER->getScreenWidth());
    f32 screenHeight = static_cast<f32>(GFX_ADAPTER->getScreenHeight());
    f32 ratioX = screenWidth/UI2D_WidthRef;
    f32 ratioY = screenHeight/UI2D_HeightRef;
    f32 screenMarginX = getTemplate()->getScreenMarginX()*ratioX;
    f32 screenMarginY = getTemplate()->getScreenMarginY()*ratioY;
    f32 scaleX = getTemplate()->getScale() * ratioX;
    f32 scaleY = getTemplate()->getScale() * ratioY;
    Vec2d p(screenWidth - screenMarginX, screenMarginY);
    Vec2d scale(scaleX,scaleY);

    m_actor->set2DPos(p);
    m_actor->setScale(scale);
}

void Ray_TimeAttackHUDTimerComponent::updateTimer()
{
    f32 timeF = RAY_GAMEMANAGER->getTimeAttackTimer();
    i32 timeI = static_cast<i32>(timeF);
    i32 timeMin = timeI/60;
    i32 timeSec = timeI-(60*timeMin);

    m_textTimer.m_text.setTextFormat("%i:%02i",timeMin,timeSec);

    if ( m_electoon == OBJECTIVESTATE_SHOW )
    {
        if ( m_electoonTime < timeI )
        {
            m_electoon = OBJECTIVESTATE_HIDE;
        }
    }

    if ( m_cup == OBJECTIVESTATE_SHOW )
    {
        if ( m_cupTime < timeI )
        {
            m_cup = OBJECTIVESTATE_HIDE;
        }
    }
}

void Ray_TimeAttackHUDTimerComponent::updateTexts()
{
    f32 screenWidth = static_cast<f32>(GFX_ADAPTER->getScreenWidth());
    f32 screenHeight = static_cast<f32>(GFX_ADAPTER->getScreenHeight());
    f32 ratioX = screenWidth/UI2D_WidthRef;
    f32 ratioY = screenHeight/UI2D_HeightRef;
    Vec2d newScale(ratioX,ratioY);
    Vec3d textPos;
    transform2d xf;

    xf.m_pos = m_actor->get2DPos();
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

    Actor* timerActor = m_timerTextActor.getActor();

    if ( timerActor )
    {
        timerActor->setScale(newScale);

        if ( m_animatedComponent->getBonePos(m_timerTextBoneIndex,textPos) )
        {
            textPos.m_y *= -1.f;
            textPos = mulTransform2d(xf,textPos.truncateTo2D()*m_actor->getScale()).to3d(textPos.m_z);
            timerActor->set2DPos(textPos.truncateTo2D());
        }

        UITextBox* textBox = timerActor->GetComponent<UITextBox>();

        if ( textBox )
        {
            Color newColor = getTemplate()->getColorNormal();
            f32 textSize = getTemplate()->getTimerTextSize();

            f32 currentTimeF = RAY_GAMEMANAGER->getTimeAttackTimer();
            i32 currentTime = static_cast<i32>(currentTimeF);
            bbool playFx = bfalse;

            if ( !RAY_GAMEMANAGER->getTimeAttackFinished() && !GAMEMANAGER->isLoadingMap() )
            {
                if ( currentTime <= m_cupTime )
                {
                    i32 minTime = m_cupTime - getTemplate()->getCriticalTimeMargin();

                    if ( currentTime >= minTime )
                    {
                        f32 intervalTime = currentTimeF - minTime;
                        i32 numIntervals = static_cast<i32>(intervalTime/getTemplate()->getCriticalTimeIntervals());
                        bbool critical = (numIntervals%2)>0;
                        f32 t = intervalTime - (static_cast<f32>(numIntervals)*getTemplate()->getCriticalTimeIntervals());

                        t /= getTemplate()->getCriticalTimeIntervals();

                        if ( critical )
                        {
                            newColor = Color::Interpolate(getTemplate()->getColorTimeCritical(),getTemplate()->getColorNormal(),t);
                            textSize = Interpolate(getTemplate()->getTimerTextCriticalSize(),getTemplate()->getTimerTextSize(),t);
                        }
                        else
                        {
                            newColor = Color::Interpolate(getTemplate()->getColorNormal(),getTemplate()->getColorTimeCritical(),t);
                            textSize = Interpolate(getTemplate()->getTimerTextSize(),getTemplate()->getTimerTextCriticalSize(),t);
                        }

                        playFx = btrue;
                    }
                }
                else if ( currentTime <= m_electoonTime )
                {
                    i32 minTime = m_electoonTime - getTemplate()->getCriticalTimeMargin();

                    if ( currentTime >= minTime )
                    {
                        f32 intervalTime = currentTimeF - minTime;
                        i32 numIntervals = static_cast<i32>(intervalTime/getTemplate()->getCriticalTimeIntervals());
                        bbool critical = (numIntervals%2)>0;
                        f32 t = intervalTime - (static_cast<f32>(numIntervals)*getTemplate()->getCriticalTimeIntervals());

                        t /= getTemplate()->getCriticalTimeIntervals();

                        if ( critical )
                        {
                            newColor = Color::Interpolate(getTemplate()->getColorTimeCritical(),getTemplate()->getColorNormal(),t);
                            textSize = Interpolate(getTemplate()->getTimerTextCriticalSize(),getTemplate()->getTimerTextSize(),t);
                        }
                        else
                        {
                            newColor = Color::Interpolate(getTemplate()->getColorNormal(),getTemplate()->getColorTimeCritical(),t);
                            textSize = Interpolate(getTemplate()->getTimerTextSize(),getTemplate()->getTimerTextCriticalSize(),t);
                        }

                        playFx = btrue;
                    }
                }
            }

            if ( playFx &&
               ( CINEMATICMANAGER->getLockPlayers() ||
                 GAMEMANAGER->isInPause() ) )
            {
                playFx = bfalse;
            }

            if ( playFx )
            {
                playCriticSound();
            }
            else
            {
                stopCriticSound();
            }

            AnimBoneDyn * bone = m_animatedComponent->getBone(m_timerTextBoneIndex);

            if ( bone )
            {
                newColor.m_a = bone->m_Alpha;
            }

            m_textTimer.m_color = newColor.getAsU32();
            m_textTimer.m_size = textSize;

            textBox->setTextAt(m_textTimer,0);
        }
    }

    Actor* electoonActor = m_electoonTextActor.getActor();

    if ( electoonActor )
    {
        electoonActor->setScale(newScale);

        if ( m_animatedComponent->getBonePos(m_electoonTextBoneIndex,textPos) )
        {
            textPos.m_y *= -1.f;
            textPos = mulTransform2d(xf,textPos.truncateTo2D()*m_actor->getScale()).to3d(textPos.m_z);
            electoonActor->set2DPos(textPos.truncateTo2D());
        }

        UITextBox* textBox = electoonActor->GetComponent<UITextBox>();

        if ( textBox )
        {
            AnimBoneDyn * bone = m_animatedComponent->getBone(m_electoonTextBoneIndex);

            if ( bone )
            {
                Color newColor = m_textElectoon.m_color;
                newColor.m_a = bone->m_Alpha;
                m_textElectoon.m_color = newColor.getAsU32();
                textBox->setTextAt(m_textElectoon,0);
            }
        }
    }

    Actor* cupActor = m_cupTextActor.getActor();

    if ( cupActor )
    {
        cupActor->setScale(newScale);

        if ( m_animatedComponent->getBonePos(m_cupTextBoneIndex,textPos) )
        {
            textPos.m_y *= -1.f;
            textPos = mulTransform2d(xf,textPos.truncateTo2D()*m_actor->getScale()).to3d(textPos.m_z);
            cupActor->set2DPos(textPos.truncateTo2D());
        }

        UITextBox* textBox = cupActor->GetComponent<UITextBox>();

        if ( textBox )
        {
            AnimBoneDyn* bone = m_animatedComponent->getBone(m_cupTextBoneIndex);

            if ( bone )
            {
                Color newColor = m_textCup.m_color;
                newColor.m_a = bone->m_Alpha;
                m_textCup.m_color = newColor.getAsU32();
                textBox->setTextAt(m_textCup,0);
            }
        }
    }
}

Vec2d Ray_TimeAttackHUDTimerComponent::getTimerBonePos() const
{
    Vec3d bonePos;

    if ( m_animatedComponent->getBonePos(m_chronoBoneIndex,bonePos) )
    {
        transform2d xf;

        xf.m_pos = m_actor->get2DPos();
        xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
        xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

        bonePos.m_y *= -1.f;

        return mulTransform2d(xf,bonePos.truncateTo2D()*m_actor->getScale());
    }
    else
    {
        return m_actor->get2DPos();
    }
}

void Ray_TimeAttackHUDTimerComponent::updateAnimInput()
{
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(cup,962018055),m_cup);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon,1900755119),m_electoon);
}

void Ray_TimeAttackHUDTimerComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Start_TimeAttack,713959317) )
        {
            m_animatedComponent->setAnim(getTemplate()->getStandAnim());
        }
    }
}

void Ray_TimeAttackHUDTimerComponent::playCriticSound()
{
    if ( m_playingCritic == ITF::SoundHandle::getInvalidHandle() )
    {
        m_playingCritic = m_soundComponent->playSound(getTemplate()->getCriticalFX());
    }
}

void Ray_TimeAttackHUDTimerComponent::stopCriticSound()
{
    if ( m_playingCritic != ITF::SoundHandle::getInvalidHandle())
    {
        m_soundComponent->stopSound(m_playingCritic);
        m_playingCritic = ITF::SoundHandle::getInvalidHandle();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_TimeAttackHUDTimerComponent_Template)
BEGIN_SERIALIZATION(Ray_TimeAttackHUDTimerComponent_Template)

    SERIALIZE_MEMBER("scale",m_scale);
    SERIALIZE_MEMBER("screenMarginX",m_screenMarginX);
    SERIALIZE_MEMBER("screenMarginY",m_screenMarginY);
    SERIALIZE_MEMBER("boneTimer",m_boneTimer);
    SERIALIZE_MEMBER("boneCup",m_boneCup);
    SERIALIZE_MEMBER("boneElectoons",m_boneElectoons);
    SERIALIZE_MEMBER("boneChrono",m_boneChrono);
    SERIALIZE_MEMBER("textActorFile",m_textActorFile);
    SERIALIZE_MEMBER("timerTextSize",m_timerTextSize);
    SERIALIZE_MEMBER("timerTextCriticalSize",m_timerTextCriticalSize);
    SERIALIZE_MEMBER("prizeTextSize",m_prizeTextSize);
    SERIALIZE_MEMBER("colorNormal",m_colorNormal);
    SERIALIZE_MEMBER("colorTimeCritical",m_colorTimeCritical);
    SERIALIZE_MEMBER("criticalTimeMargin",m_criticalTimeMargin);
    SERIALIZE_MEMBER("criticalTimeIntervals",m_criticalTimeIntervals);
    SERIALIZE_MEMBER("appearAnim",m_appearAnim);
    SERIALIZE_MEMBER("standAnim",m_standAnim);
    SERIALIZE_MEMBER("criticalFX",m_criticalFX);

END_SERIALIZATION()

Ray_TimeAttackHUDTimerComponent_Template::Ray_TimeAttackHUDTimerComponent_Template()
: m_scale(15.f)
, m_screenMarginX(300.f)
, m_screenMarginY(0.f)
, m_textActorFile("World/Common/Friendly/Timer/TimerTextBox.act")
, m_timerTextSize(80.f)
, m_timerTextCriticalSize(100.f)
, m_prizeTextSize(60.f)
, m_colorNormal(Color::white())
, m_colorTimeCritical(Color::red())
, m_criticalTimeMargin(5)
, m_criticalTimeIntervals(0.5f)
{
}

Ray_TimeAttackHUDTimerComponent_Template::~Ray_TimeAttackHUDTimerComponent_Template()
{
}

}
