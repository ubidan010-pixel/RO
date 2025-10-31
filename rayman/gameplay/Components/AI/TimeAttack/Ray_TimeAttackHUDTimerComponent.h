
#ifndef _ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_
#define _ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

namespace ITF
{

class Ray_TimeAttackHUDTimerComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TimeAttackHUDTimerComponent,ActorComponent,2795731935)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_TimeAttackHUDTimerComponent();
    virtual ~Ray_TimeAttackHUDTimerComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onActorClearComponents();
    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        onBecomeActive();
    virtual void                        onBecomeInactive();
    virtual void                        onResourceReady();
    virtual void                        Update( f32 _dt );
    virtual void                        updateAnimInput();
    virtual void                        onEvent( class Event* _event );

    Vec2d                               getTimerBonePos() const;

protected:

    const class Ray_TimeAttackHUDTimerComponent_Template* getTemplate() const;

    enum ObjectiveState
    {
        OBJECTIVESTATE_HIDE = 0,
        OBJECTIVESTATE_SHOW = 1,
    };

    void                                spawnTextActors();
    void                                deleteTextActors();

    void                                updatePosition();
    void                                updateTimer();
    void                                updateTexts();

    void                                playCriticSound();
    void                                stopCriticSound();

    class AnimatedComponent*            m_animatedComponent;
    class SoundComponent*               m_soundComponent;

    ObjectiveState                      m_electoon;
    ObjectiveState                      m_cup;
    i32                                 m_electoonTime;
    i32                                 m_cupTime;
    u32                                 m_chronoBoneIndex;
    u32                                 m_timerTextBoneIndex;
    u32                                 m_electoonTextBoneIndex;
    u32                                 m_cupTextBoneIndex;

    ActorRef                            m_timerTextActor;
    ActorRef                            m_electoonTextActor;
    ActorRef                            m_cupTextActor;

    TextInfo                            m_textTimer;
    TextInfo                            m_textCup;
    TextInfo                            m_textElectoon;

    SpawneeGenerator                    m_spawner;

    ITF::SoundHandle         m_playingCritic;
};

class Ray_TimeAttackHUDTimerComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TimeAttackHUDTimerComponent_Template,TemplateActorComponent,2746967258);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_TimeAttackHUDTimerComponent);

public:

    Ray_TimeAttackHUDTimerComponent_Template();
    ~Ray_TimeAttackHUDTimerComponent_Template();

    f32                                 getScale() const { return m_scale; }
    f32                                 getScreenMarginX() const { return m_screenMarginX; }
    f32                                 getScreenMarginY() const { return m_screenMarginY; }
    const StringID&                     getBoneTimer() const { return m_boneTimer; }
    const StringID&                     getBoneCup() const { return m_boneCup; }
    const StringID&                     getBoneElectoons() const { return m_boneElectoons; }
    const StringID&                     getBoneChrono() const { return m_boneChrono; }
    const Path&                         getTextActorFile() const { return m_textActorFile; }
    
    f32                                 getTimerTextSize() const { return m_timerTextSize; }
    f32                                 getTimerTextCriticalSize() const { return m_timerTextCriticalSize; }
    f32                                 getPrizeTextSize() const { return m_prizeTextSize; }
    const Color&                        getColorNormal() const { return m_colorNormal; }
    const Color&                        getColorTimeCritical() const { return m_colorTimeCritical; }
    i32                                 getCriticalTimeMargin() const { return m_criticalTimeMargin; }
    f32                                 getCriticalTimeIntervals() const { return m_criticalTimeIntervals; }
    
    const StringID&                     getAppearAnim() const { return m_appearAnim; }
    const StringID&                     getStandAnim() const { return m_standAnim; }
    
    const StringID&                     getCriticalFX() const { return m_criticalFX; }
private:

    f32                                 m_scale;
    f32                                 m_screenMarginX;
    f32                                 m_screenMarginY;
    StringID                            m_boneTimer;
    StringID                            m_boneCup;
    StringID                            m_boneElectoons;
    StringID                            m_boneChrono;
    Path                                m_textActorFile;
    f32                                 m_timerTextSize;
    f32                                 m_timerTextCriticalSize;
    f32                                 m_prizeTextSize;
    Color                               m_colorNormal;
    Color                               m_colorTimeCritical;
    i32                                 m_criticalTimeMargin;
    f32                                 m_criticalTimeIntervals;

    StringID                            m_appearAnim;
    StringID                            m_standAnim;

    StringID                            m_criticalFX;
};

ITF_INLINE const class Ray_TimeAttackHUDTimerComponent_Template* Ray_TimeAttackHUDTimerComponent::getTemplate() const
{
    return static_cast<const class Ray_TimeAttackHUDTimerComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_

