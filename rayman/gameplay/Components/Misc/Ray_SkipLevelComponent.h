#ifndef _ITF_RAY_SKIPLEVELCOMPONENT_H_
#define _ITF_RAY_SKIPLEVELCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF {

//------------------------------------------------------------------------------
class Ray_SkipLevelComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_SkipLevelComponent, ActorComponent,1847282724)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_SkipLevelComponent();
    virtual ~Ray_SkipLevelComponent();

    virtual bbool                               needsUpdate() const { return btrue; }
    virtual bbool                               needsDraw() const { return bfalse; }
    virtual bbool                               needsDraw2D() const { return bfalse; }

    virtual void                                onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void                                onActorClearComponents();
    virtual void                                onBecomeActive();
    virtual void                                onBecomeInactive();
    virtual void                                Update( f32 _dt );


    virtual void                                onEvent( class Event* _event );
    virtual void                                onCheckpointLoaded();

#ifdef ITF_SUPPORT_EDITOR
    virtual void                                onEditorCreated( class Actor* _original );
    virtual void                                onEditorMove( bbool _modifyInitialPos );
#endif
    enum State
    {
        State_Inactive,
        State_Prepare,
        State_Appearing,
        State_Talking,
        State_Disappearing,
    };

    bbool                                       isDone() const { return m_state == State_Inactive; }
    void                                        start() { changeState(State_Prepare);}
    void                                        stop(bbool _force) { if (_force) changeState(State_Inactive);else changeState(State_Disappearing);}
private:


    ITF_INLINE const class Ray_SkipLevelComponent_Template* getTemplate() const;
    void                                        updatePosition();
    void changeState( State _state );


    Vec2d               m_relativePos;
    AnimLightComponent * m_animComponent;

    State m_state;
};

//------------------------------------------------------------------------------
class Ray_SkipLevelComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_SkipLevelComponent_Template, TemplateActorComponent,2063844644)
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SkipLevelComponent);
    DECLARE_SERIALIZE();

public:
    Ray_SkipLevelComponent_Template();
    virtual ~Ray_SkipLevelComponent_Template();


    ITF_INLINE const Path & getTextPath() const { return m_textPath; }
    ITF_INLINE const Vec2d & getAnimSize() const { return m_animSize; }
    ITF_INLINE const Vec2d & getTextAnimSize() const { return m_textAnimSize; }
    ITF_INLINE ITF::Vec2d getScreenPos() const { return m_screenPos; }
    ITF_INLINE ITF::Vec2d getTextScreenPos() const { return m_textScreenPos; }
    ITF_INLINE ITF::StringID getAppearAnim() const { return m_appearAnim; }
    ITF_INLINE ITF::StringID getDisappearAnim() const { return m_disappearAnim; }
    ITF_INLINE ITF::StringID getTalkAnim() const { return m_talkAnim; }
    ITF_INLINE ITF::LocalisationId getLocId() const { return m_locId; }
    ITF_INLINE u32 getTextRank() const { return m_textRank; }
    ITF_INLINE u32 getRank() const { return m_rank; }
    ITF_INLINE u32 getContextIconRank() const { return m_contextIconRank; }

private:

    Path m_textPath;
    Vec2d m_animSize;
    Vec2d m_textAnimSize;
    Vec2d m_screenPos;
    Vec2d m_textScreenPos;
    StringID m_appearAnim;
    StringID m_disappearAnim;
    StringID m_talkAnim;
    LocalisationId m_locId;
    u32 m_textRank;
    u32 m_rank;
    u32 m_contextIconRank;
};

ITF_INLINE const Ray_SkipLevelComponent_Template* Ray_SkipLevelComponent::getTemplate() const { return static_cast<const Ray_SkipLevelComponent_Template*>(m_template); }

} // namespace ITF

#endif //_ITF_Ray_SkipLevelComponent_H_
