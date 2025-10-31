
#ifndef _ITF_RAY_MURPHYAICOMPONENT_H_
#define _ITF_RAY_MURPHYAICOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{

class Ray_MurphyAIComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_MurphyAIComponent,ActorComponent,4141462231)
    DECLARE_SERIALIZE()

public:

    Ray_MurphyAIComponent();
    virtual ~Ray_MurphyAIComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        onResourceReady();
    virtual void                        Update( f32 _dt );
    virtual void                        onEvent( Event * _event);
    virtual void                        updateAnimInput();

    virtual void                        onActorClearComponents();
protected:


    const class Ray_MurphyAIComponent_Template* getTemplate() const;

    void                                updateBubble();


    class Ray_TalkingBubbleComponent*   getBubbleComponent() const;

    enum State
    {
        State_Idle,
        State_Appear,
        State_Talk,
        State_Disappear
    };

    void                                setState(State _state);

    typedef SafeArray <LocalisationId> SentenceList;

    class AnimatedComponent*            m_animatedComponent;
    SentenceList                        m_sentences;
    ActorRef                            m_bubbleRef;
    SpawneeGenerator                    m_bubbleSpawner;
    u32                                 m_bubbleBoneIndex;
    bbool                               m_appearRequested;
    bbool                               m_disappearRequested;


    State m_state;
};

class Ray_MurphyAIComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_MurphyAIComponent_Template,TemplateActorComponent,654353777);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MurphyAIComponent);

public:

    Ray_MurphyAIComponent_Template();
    ~Ray_MurphyAIComponent_Template();

    
    const StringID&                         getBubbleBone() const { return m_bubbleBone; }
    const Path&                             getBubblePath() const { return m_bubbleAct; }
    f32                                     getBubbleZOffset() const { return m_bubbleZOffset; }
    StringID                                getAppearAnim() const { return m_appearAnim; }
    StringID                                getTalkAnim() const { return m_talkAnim; }
    StringID                                getDisappearAnim() const { return m_disappearAnim; }
    StringID                                getIdleAnim() const { return m_idleAnim; }

private:

    f32                                     m_playerDetectRadiusAppear;
    f32                                     m_playerDetectRadiusDisappear;
    StringID                                m_bubbleBone;
    Path                                    m_bubbleAct;
    f32                                     m_bubbleZOffset;

    StringID                                m_appearAnim;
    StringID                                m_talkAnim;
    StringID                                m_disappearAnim;
    StringID                                m_idleAnim;
};

ITF_INLINE const class Ray_MurphyAIComponent_Template* Ray_MurphyAIComponent::getTemplate() const
{
    return static_cast<const class Ray_MurphyAIComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_MURPHYAICOMPONENT_H_

