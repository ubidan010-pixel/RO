
#ifndef _ITF_RAY_TALKINGHATAICOMPONENT_H_
#define _ITF_RAY_TALKINGHATAICOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

namespace ITF
{

class Ray_TalkingHatAIComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TalkingHatAIComponent,ActorComponent,3439430031)
    DECLARE_SERIALIZE()

public:

    Ray_TalkingHatAIComponent();
    virtual ~Ray_TalkingHatAIComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        onResourceReady();
    virtual void                        Update( f32 _dt );
    virtual void                        onEvent( Event * _event);
    virtual void                        updateAnimInput();

protected:

    enum ArrowState
    {
        ARROWSTATE_HIDDEN,
        ARROWSTATE_APPEAR,
        ARROWSTATE_IDLE,
        ARROWSTATE_DISAPPEAR,
    };

    const class Ray_TalkingHatAIComponent_Template* getTemplate() const;

    void                                updateDetectPlayer();
    void                                updateBubble();
    void                                processBounce();

    void                                setAppear();
    void                                setDisappear();

    class Ray_TalkingBubbleComponent*   getBubbleComponent() const;

    typedef SafeArray <LocalisationId> SentenceList;

    class AnimatedComponent*            m_animatedComponent;
    bbool                               m_bounceTriggered;
    bbool                               m_visible;
    bbool                               m_bounce;
    bbool                               m_showText;
    ArrowState                          m_pointingArrow;
    SentenceList                        m_sentences;
    ActorRef                            m_bubbleRef;
    SpawneeGenerator                    m_bubbleSpawner;
    u32                                 m_bubbleBoneIndex;
};

class Ray_TalkingHatAIComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TalkingHatAIComponent_Template,TemplateActorComponent,4160581289);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_TalkingHatAIComponent);

public:

    Ray_TalkingHatAIComponent_Template();
    ~Ray_TalkingHatAIComponent_Template();

    
    f32                                     getPlayerDetectRadiusAppear() const { return m_playerDetectRadiusAppear; }
    f32                                     getPlayerDetectRadiusDisappear() const { return m_playerDetectRadiusDisappear; }
    const StringID&                         getBubbleBone() const { return m_bubbleBone; }
    const Path&                             getBubblePath() const { return m_bubbleAct; }
    f32                                     getBubbleZOffset() const { return m_bubbleZOffset; }

private:

    f32                                     m_playerDetectRadiusAppear;
    f32                                     m_playerDetectRadiusDisappear;
    StringID                                m_bubbleBone;
    Path                                    m_bubbleAct;
    f32                                     m_bubbleZOffset;
};

ITF_INLINE const class Ray_TalkingHatAIComponent_Template* Ray_TalkingHatAIComponent::getTemplate() const
{
    return static_cast<const class Ray_TalkingHatAIComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_TALKINGHATAICOMPONENT_H_

