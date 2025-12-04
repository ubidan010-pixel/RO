#ifndef _ITF_TUTORIALTEXTCOMPONENT_H_
#define _ITF_TUTORIALTEXTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

namespace ITF {

class TutorialTextComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TutorialTextComponent, ActorComponent,2460478030)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    TutorialTextComponent();
    virtual ~TutorialTextComponent();

    virtual bbool                               needsUpdate() const { return btrue; }
    virtual bbool                               needsDraw() const { return bfalse; }
    virtual bbool                               needsDraw2D() const { return bfalse; }

    virtual void                                onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void                                onActorClearComponents();
    virtual void                                onBecomeActive();
    virtual void                                onBecomeInactive();
    virtual void                                Update( f32 _dt );

    void                                        show( const LocalisationId& _id );
    void                                        hide() { m_receiveHide = btrue; m_receiveShow = bfalse; m_locIdToShow = LocalisationId::Invalid; m_receiveForceHide = bfalse; }
    void                                        forceHide() { m_receiveForceHide = btrue; }
    bbool                                       isFinishHiding() const;

#ifdef ITF_SUPPORT_EDITOR
    virtual void                                onEditorCreated( class Actor* _original );
    virtual void                                onEditorMove( bbool _modifyInitialPos );
#endif

private:

    ITF_INLINE const class TutorialTextComponent_Template* getTemplate() const;

    void                                        updatePosition();

    class InGameTextComponent*                  m_inGameTextComponent;
    class AnimatedComponent*                    m_animatedComponent;

    Vec2d                                       m_relativePos;
    LocalisationId                              m_locIdToShow;
    LocalisationId                              m_currentLocId;
    bbool                                       m_hiding;
    bbool                                       m_receiveHide;
    bbool                                       m_receiveShow;
    bbool                                       m_receiveForceHide;
    bbool                                       m_isShown;
    bbool                                       m_firstUpdate;
};

//------------------------------------------------------------------------------
class TutorialTextComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(TutorialTextComponent_Template, TemplateActorComponent,386828231)
    DECLARE_ACTORCOMPONENT_TEMPLATE(TutorialTextComponent);
    DECLARE_SERIALIZE();

public:
    TutorialTextComponent_Template();
    virtual ~TutorialTextComponent_Template();

    const Vec2d&                                getAnimSize() const { return m_animSize; }

private:

    Vec2d                                       m_animSize;
};

ITF_INLINE const TutorialTextComponent_Template* TutorialTextComponent::getTemplate() const { return static_cast<const TutorialTextComponent_Template*>(m_template); }

} // namespace ITF

#endif //_ITF_TUTORIALTEXTCOMPONENT_H_
