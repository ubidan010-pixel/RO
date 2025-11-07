
#ifndef _ITF_RAY_CHRONOAICOMPONENT_H_
#define _ITF_RAY_CHRONOAICOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

namespace ITF
{

class Ray_ChronoAIComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ChronoAIComponent,ActorComponent,1392119711)
    DECLARE_SERIALIZE()

public:

    Ray_ChronoAIComponent();
    virtual ~Ray_ChronoAIComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onBecomeInactive();

    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        onActorClearComponents();
    virtual void                        onResourceReady();
    virtual void                        onEvent( class Event* _event );
    virtual void                        Update( f32 _dt );
    virtual void                        updateAnimInput();

protected:

    enum ChronoState
    {
        CHRONOSTATE_IDLE = 0,
        CHRONOSTATE_OPEN = 1,
        CHRONOSTATE_SHAKE = 2,
        CHRONOSTATE_DISAPPEAR = 3,
    };

    const class Ray_ChronoAIComponent_Template* getTemplate() const;

    class Ray_TalkingBubbleComponent*   getBubbleComponent() const;

    void                                startTimeAttack();

    void                                updateBubble();
    void                                updateShake( f32 _dt );

    void                                startMovement();

    class AnimatedComponent*            m_animatedComponent;
    ChronoState                         m_state;
    ActorRef                            m_bubbleRef;
    SpawneeGenerator                    m_bubbleSpawner;
    u32                                 m_bubbleBoneIndex;
    Spline                              m_splineMovement;
    f32                                 m_moveCounter;
    bbool                               m_shake;
    ActorRef                            m_murphyRef;
    SpawneeGenerator                    m_murphySpawner;
};

class Ray_ChronoAIComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ChronoAIComponent_Template,TemplateActorComponent,2735339634);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ChronoAIComponent);

public:

    Ray_ChronoAIComponent_Template();
    ~Ray_ChronoAIComponent_Template();

    
    const StringID&                         getBubbleBone() const { return m_bubbleBone; }
    const Path&                             getBubblePath() const { return m_bubbleAct; }
    const Path&                             getCupPath() const { return m_cupPath; }
    const Path&                             getElectoonPath() const { return m_electoonPath; }
    const Color&                            getTextColor() const { return m_textColor; }
    f32                                     getInitialFontHeight() const { return m_fontInitialHeight; }
    f32                                     getMoveDuration() const { return m_moveDuration; }
    f32                                     getMoveCurve() const { return m_moveCurve; }
    const Path&                             getMurphyPath() const { return m_murphyAct; }

private:

    StringID                                m_bubbleBone;
    Path                                    m_bubbleAct;
    Path                                    m_cupPath;
    Path                                    m_electoonPath;
    Color                                   m_textColor;
    f32                                     m_fontInitialHeight;
    f32                                     m_moveDuration;
    f32                                     m_moveCurve;
    Path                                    m_murphyAct;
};

ITF_INLINE const class Ray_ChronoAIComponent_Template* Ray_ChronoAIComponent::getTemplate() const
{
    return static_cast<const class Ray_ChronoAIComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_CHRONOAICOMPONENT_H_

