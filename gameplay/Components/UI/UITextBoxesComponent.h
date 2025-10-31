#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#define _ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

namespace ITF
{
    struct  UITextField
    {
        DECLARE_SERIALIZE();

        UITextField()
            : m_textActorPath(Path::EmptyPath)
            , m_fontHeight(10.0f)
            , m_color(Color::white())
            , m_boneName(StringID::Invalid)
            , m_textRef(ActorRef::InvalidRef)
            , m_boneIndex(U32_INVALID)
            , m_lineId(LocalisationId::Invalid)
            , m_depthRank(-1)
        {}

        ~UITextField()
        {
            if (m_textRef.isValid() )
            {
                Actor * textBoxAct = m_textRef.getActor();
                if ( textBoxAct )
                    textBoxAct->requestDestruction();
            }
        }

        Path                    m_textActorPath;
        f32                     m_fontHeight;
        Color                   m_color;
        StringID                m_boneName;
        u32                     m_boneIndex;
        ActorRef                m_textRef;
        TextInfo                m_textInfo;
        LocalisationId          m_lineId;
        i32                     m_depthRank;
    };

    class UITextBoxesComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UITextBoxesComponent, ActorComponent,282676810);
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:

        UITextBoxesComponent();
        virtual ~UITextBoxesComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event * _event);

        void                setText( const StringID& _textFieldName, const String8& _text);
        void                setLineId( const StringID& _textFieldName, const LocalisationId& _lineId);

        void                enableRender( bbool _val );

    private:
        ITF_INLINE const class UITextBoxesComponent_Template* getTemplate() const;

        void        prepareSpawneeGenerators();

        ITF_MAP<StringID, u32>                  m_textFieldsMap;
        ITF_VECTOR<UITextField>                 m_textFields;
        ITF_MAP<StringID, SpawneeGenerator>     m_spawneeGenerators;

        AnimLightComponent*             m_animLightComponent;
        bbool                           m_enableRender;
    };


    //---------------------------------------------------------------------------------------------------

    class UITextBoxesComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UITextBoxesComponent_Template, TemplateActorComponent,565524987);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UITextBoxesComponent);

    public:

        UITextBoxesComponent_Template()
            : m_isDraw2d(bfalse)
            , m_usBoneAngle(bfalse)
            , m_useBoneScale (btrue)
            , m_useScreenRatio(bfalse)
            , m_disableActiveSync(bfalse)
        {}
        virtual ~UITextBoxesComponent_Template() {}

        ITF_INLINE const bbool                      getIsDraw2d() const { return m_isDraw2d; }
        ITF_INLINE const ITF_VECTOR<UITextField>&   getTextFields() const { return m_textFields; }
        ITF_INLINE bbool getUseBoneAngle() const { return m_usBoneAngle; }
        ITF_INLINE bbool getUseBoneScale() const { return m_useBoneScale; }
        ITF_INLINE bbool getUseScreenRatio() const { return m_useScreenRatio; }
        ITF_INLINE ITF::bbool getDisableActiveSync() const { return m_disableActiveSync; }
    private:

        bbool                       m_isDraw2d;
        bbool                       m_usBoneAngle;
        bbool                       m_useBoneScale;
        bbool                       m_useScreenRatio;
        bbool                       m_disableActiveSync;
        ITF_VECTOR<UITextField>     m_textFields;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const UITextBoxesComponent_Template* UITextBoxesComponent::getTemplate() const
    {
        return static_cast<const UITextBoxesComponent_Template*>(m_template);
    }
};

#endif //_ITF_UITEXTBOXESCOMPONENT_H_
