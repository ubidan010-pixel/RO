
#ifndef _ITF_RAY_TIMEATTACKHUDRESULTSCOMPONENT_H_
#define _ITF_RAY_TIMEATTACKHUDRESULTSCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

namespace ITF
{

class Ray_TimeAttackHUDResultsComponent : public ActorComponent, IUpdateAnimInputCriteria, IInputListener
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TimeAttackHUDResultsComponent,ActorComponent,3725071531)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_TimeAttackHUDResultsComponent();
    virtual ~Ray_TimeAttackHUDResultsComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onBecomeActive();
    virtual void                        onBecomeInactive();
    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        onActorClearComponents();
    virtual void                        Update( f32 _dt );
    virtual void                        onEvent( Event* _event );
    virtual void                        updateAnimInput();

    virtual void                        Receive( u32 deviceID, f32 axis, const StringID& action );

protected:

    const class Ray_TimeAttackHUDResultsComponent_Template* getTemplate() const;

    enum Phase
    {
        Phase_Timer = 0,
        Phase_Electoon = 1,
        Phase_Cup = 2,
        Max_Phase,
    };

    enum PhaseState
    {
        Phase_Hidden = 0,
        Phase_Appear = 1,
        Phase_Fail = 2,
        Phase_Win = 3,
    };

    void                                enable();
    void                                disable();
    void                                updatePosition();
    void                                updateFinish();
    void                                playNext();

    class AnimatedComponent*            m_animatedComponent;
    class UITextBoxesComponent*         m_textBoxesComponent;

    bbool                               m_waitInput;
    u32                                 m_phase;
    FixedArray <PhaseState,Max_Phase>   m_phaseState;
    i32                                 m_electoonTime;
    i32                                 m_cupTime;
    i32                                 m_myTime;
    bbool                               m_receivedPlayNext;
};

class Ray_TimeAttackHUDResultsComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TimeAttackHUDResultsComponent_Template,TemplateActorComponent,900451392);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_TimeAttackHUDResultsComponent);

public:

    Ray_TimeAttackHUDResultsComponent_Template();
    ~Ray_TimeAttackHUDResultsComponent_Template();

    f32                                 getScale() const { return m_scale; }
    const StringID&                     getBoneTimer() const { return m_boneTimer; }
    const StringID&                     getBoneCup() const { return m_boneCup; }
    const StringID&                     getBoneElectoons() const { return m_boneElectoons; }
    
private:

    f32                                 m_scale;
    StringID                            m_boneTimer;
    StringID                            m_boneCup;
    StringID                            m_boneElectoons;
};

ITF_INLINE const class Ray_TimeAttackHUDResultsComponent_Template* Ray_TimeAttackHUDResultsComponent::getTemplate() const
{
    return static_cast<const class Ray_TimeAttackHUDResultsComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_TIMEATTACKHUDRESULTSCOMPONENT_H_

