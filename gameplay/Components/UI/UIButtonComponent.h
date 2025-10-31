#ifndef _ITF_UIBUTTONCOMPONENT_H_
#define _ITF_UIBUTTONCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class UIButtonComponent : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIButtonComponent,UIComponent,3136744194);

    public:
        DECLARE_SERIALIZE()

        UIButtonComponent();
        ~UIButtonComponent();

        virtual     bbool           needsUpdate         (       ) const             { return btrue; }
        virtual     bbool           needsDraw           (       ) const             { return bfalse; }
        virtual     bbool           needsDraw2D         (       ) const             { return bfalse; }
        virtual     void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     void            Update              ( f32 _deltaTime );


        virtual     void            onActorClearComponents();
        virtual     void            onBecomeActive();


#ifdef ITF_SUPPORT_EDITOR
        virtual     void            drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const;
        virtual     void            onEditorSelected();
#endif // ITF_SUPPORT_EDITOR

        virtual     void            onPressed();
        virtual     void            onReleased();
        virtual     void            onRollover();
        virtual     void            onRollout();

        enum ButtonState
        {
            State_Idle,
            State_onRollover,
            State_onRollout,
            State_onReleased,
            State_onPressed,
            State_Idle_Selected
        };
        ITF_INLINE ButtonState getState() const { return m_state; }
    private:
        ITF_INLINE const class UIButtonComponent_Template * getTemplate() const;
        void                        updateTextFromAnim( Vec3d &textPos, Vec2d &textScale, f32 &textAngle );
        void                        updateState(Vec3d &textPos, Vec2d &textScale, f32 &textAngle);

        f32 updateMinActorScale(  );

        void                        updateRelativePosFromTarget(f32 _pos);

        void                        clear();
        void                        changeState(ButtonState newState);
        void                        changeAnimation(const StringID & AnimationID);
        f32                         getTargetPos();
        ITF_INLINE ITF::bbool getExtremity() const { return m_isExtremity; }
    protected:
        ButtonState     m_state;
        StringID        m_currentAnimationName;
        u32             m_currentAnimationIndex;

        bbool           m_isTextBoxActive;
        ActorRef        m_textBox;
        class LinkComponent * m_linkComponent;

        f32             m_offset1;
        f32             m_offset2;
        f32             m_offset3;

        Vec2d           m_initPos;
        SpawneeGenerator m_spawner;

        bbool           m_firstUpdate;

        f32             m_timer;
        f32             m_curScale;
        bbool           m_isExtremity;
        ActorRef        m_child1;
        ActorRef        m_child2;
        ActorRef        m_child3;

        f32             m_curActorScale;
    };

    class UIButtonComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIButtonComponent_Template,UIComponent_Template,684890016);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIButtonComponent);
    public:
        UIButtonComponent_Template();
        ITF_INLINE f32 getSpeed() const { return m_speed; }
        const Path& getTextPath() const { return m_textPath; }
        ITF_INLINE f32 getIdleSelectedScale() const { return m_idleSelectedScale; }
        ITF_INLINE f32 getIdleSelectedPulseFrequency() const { return m_idleSelectedPulseFrequency; }
        ITF_INLINE ITF::bbool getIs2dActor() const { return m_is2dActor; }
        ITF_INLINE f32 getActorScaleSmoothFactor() const { return m_actorScaleSmoothFactor; }
        ITF_INLINE f32 getMinActorScale() const { return m_minActorScale; }
    private:
        f32 m_speed;
        Path m_textPath;
        f32 m_idleSelectedScale;
        f32 m_idleSelectedPulseFrequency;
        bbool m_is2dActor;
        f32   m_actorScaleSmoothFactor;
        f32 m_minActorScale;
    };

    ITF_INLINE const UIButtonComponent_Template* UIButtonComponent::getTemplate() const
    {
        return static_cast<const UIButtonComponent_Template*>(m_template);
    }

}
#endif // _ITF_UIMENUITEMTEXT_H_
