#ifndef _ITF_INGAMETEXTCOMPONENT_H_
#define _ITF_INGAMETEXTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

namespace ITF {

//------------------------------------------------------------------------------
class InGameTextComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(InGameTextComponent, ActorComponent,3492037591)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    InGameTextComponent();
    virtual ~InGameTextComponent();

    virtual bbool                               needsUpdate() const { return btrue; }
    virtual bbool                               needsDraw() const { return bfalse; }
    virtual bbool                               needsDraw2D() const { return bfalse; }

    virtual void                                onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void                                onBecomeActive();
    virtual void                                Update( f32 _dt );
    virtual void                                updateAnimInput();

    void                                        setText( const LocalisationId& _id );
    void                                        playAppear();
    void                                        playDisappear();
    bbool                                       isDisappearFinished();

private:

    ITF_INLINE const class InGameTextComponent_Template* getTemplate() const;

    void                                        prepareText();
    void                                        updateScale( f32 _dt );

    class AnimatedComponent*                    m_animComponent;
    class UITextBoxesComponent*                 m_textBoxComponent;
    LocalisationId                              m_locId;
    f32                                         m_currentScaleX;
    f32                                         m_targetScaleX;
    f32                                         m_velScaleX;
    bbool                                       m_updateText;
    bbool                                       m_appear;
};

//------------------------------------------------------------------------------
class InGameTextComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(InGameTextComponent_Template, TemplateActorComponent,4085259041)
    DECLARE_ACTORCOMPONENT_TEMPLATE(InGameTextComponent);
    DECLARE_SERIALIZE();

public:
    InGameTextComponent_Template();
    virtual ~InGameTextComponent_Template();

    const StringID&         getTextBone() const { return m_textBone; }
    f32                     getScaleK() const { return m_scaleXK; }
    f32                     getScaleD() const { return m_scaleXD; }
    u32                     getMinScaleNumCharacters() const { return m_minScaleNumCharacters; }
    u32                     getMaxScaleNumCharacters() const { return m_maxScaleNumCharacters; }

private:

    StringID                m_textBone;
    f32                     m_scaleXK;
    f32                     m_scaleXD;
    u32                     m_minScaleNumCharacters;
    u32                     m_maxScaleNumCharacters;
};

ITF_INLINE const InGameTextComponent_Template* InGameTextComponent::getTemplate() const { return static_cast<const InGameTextComponent_Template*>(m_template); }

} // namespace ITF

#endif //_ITF_INGAMETEXTCOMPONENT_H_
