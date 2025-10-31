#ifndef _ITF_RAY_AINOTEPIAFBEHAVIOR_H_
#define _ITF_RAY_AINOTEPIAFBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_MUSICSCORESNAPCOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreSnapComponent.h"
#endif //_ITF_RAY_MUSICSCORESNAPCOMPONENT_H_

namespace ITF {

class AIAction;
class AIAction_Template;
class PolylineComponent;
class Ray_AINotePiafBehavior_Template;
class Ray_ChildLaunchComponent;
class Ray_MusicScoreOpenEvent;
class Ray_MusicScoreCloseEvent;

//------------------------------------------------------------------------------
/// Note piaf behavior: platforms, resists a bit to players weight then break
class Ray_AINotePiafBehavior : public AIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AINotePiafBehavior, AIBehavior,618719773)

public:
    DECLARE_VALIDATE_BEHAVIOR()

    Ray_AINotePiafBehavior();
    ~Ray_AINotePiafBehavior();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate() {}
    virtual void onDeactivate() {}
    virtual void update(f32 _dt);
    virtual void onEvent(Event* _event);
    virtual void updateAnimInputs();
    virtual void onActionFinished();

private:
    enum State_t
    {
        State_NA,
        State_Stand,
        State_FallResist,
        State_Catch,
        State_FallBreak,
        State_Appear3d,
        State_Disappear3d,
        State_FlyBack,
        State_Land,
        State_PseudoDeath,
    };

    ITF_INLINE const Ray_AINotePiafBehavior_Template * getTemplate() const;
    
    void processStick(const EventStickOnPolyline* _stick);
    void processHang(const EventHanging* _hang);
    void processTriggerBounce(const Ray_EventTriggerBounce* _bounce);
    void processMusicScoreOpen(const Ray_MusicScoreOpenEvent* _open);
    void processMusicScoreClose(const Ray_MusicScoreCloseEvent* _close);
    
    bbool detectsWeight();
    void emitNote();
    void carryActor(bbool _onOff, const ActorRef& _actorRef, f32 _weight);
    bbool canCatch() const;
    f32 getNoteOffset(u32 _note) const;
    bbool noteCaught();
    void changeState(State_t _newState);
    void updateState(f32 _dt);

    State_t m_state;
    u32 m_note;
    Ray_MusicScoreOpenEvent m_openEvent;
    f32 m_standTimer;
    f32 m_fallBreakTimer;
    f32 m_carriedWeight;
    ActorRefList m_carriedActors;
    bbool m_triggerBounceReceived;
    AIAction* m_standAction;
    AIAction* m_fallResistAction;
    AIAction* m_catchAction;
    AIAction* m_fallBreakAction;
    AIAction* m_appear3dAction;
    AIAction* m_disappear3dAction;
    AIAction* m_flyBackAction;
    AIAction* m_landAction;
    AIAction* m_pseudoDeathAction;
    PolylineComponent* m_polylineComponent;
    Ray_ChildLaunchComponent* m_childLaunchComponent;
};

//------------------------------------------------------------------------------
class Ray_AINotePiafBehavior_Template : public TemplateAIBehavior
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AINotePiafBehavior_Template,TemplateAIBehavior,1272807503);
    DECLARE_SERIALIZE();
    DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AINotePiafBehavior);

public:
    Ray_AINotePiafBehavior_Template();
    ~Ray_AINotePiafBehavior_Template();

    ITF_INLINE f32 getWeightThreshold() const { return m_weightThreshold; }
    ITF_INLINE f32 getFlyBackDelay() const { return m_flyBackDelay; }
    ITF_INLINE f32 getFallBreakDelay() const { return m_fallBreakDelay; }
    ITF_INLINE const Vec3d& getAppear3dOffset() const { return m_appear3dOffset; }
    ITF_INLINE bbool getBumper() const { return m_isBumper; }
    ITF_INLINE const SafeArray<StringID>& getNoteFxNames() const { return m_noteFxNames; }
    ITF_INLINE const AIAction_Template* getStandAction() const { return m_standAction; }
    ITF_INLINE const AIAction_Template* getFallResistAction() const { return m_fallResistAction; }
    ITF_INLINE const AIAction_Template* getCatchAction() const { return m_catchAction; }
    ITF_INLINE const AIAction_Template* getFallBreakAction() const { return m_fallBreakAction; }
    ITF_INLINE const AIAction_Template* getAppear3dAction() const { return m_appear3dAction; }
    ITF_INLINE const AIAction_Template* getDisappear3dAction() const { return m_disappear3dAction; }
    ITF_INLINE const AIAction_Template* getFlyBackAction() const { return m_flyBackAction; }
    ITF_INLINE const AIAction_Template* getLandAction() const { return m_landAction; }
    ITF_INLINE const AIAction_Template* getPseudoDeathAction() const { return m_pseudoDeathAction; }

private:
    f32 m_weightThreshold;
    f32 m_flyBackDelay;
    f32 m_fallBreakDelay;
    Vec3d m_appear3dOffset;
    bbool m_isBumper;
    SafeArray<StringID> m_noteFxNames;
    AIAction_Template* m_standAction;
    AIAction_Template* m_fallResistAction;
    AIAction_Template* m_catchAction;
    AIAction_Template* m_fallBreakAction;
    AIAction_Template* m_appear3dAction;
    AIAction_Template* m_disappear3dAction;
    AIAction_Template* m_flyBackAction;
    AIAction_Template* m_landAction;
    AIAction_Template* m_pseudoDeathAction;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AINotePiafBehavior_Template*
Ray_AINotePiafBehavior::getTemplate() const
{
    return static_cast<const class Ray_AINotePiafBehavior_Template *>(m_template);
}

} // namespace ITF

#endif // _ITF_RAY_AINOTEPIAFBEHAVIOR_H_
