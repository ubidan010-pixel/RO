#ifndef _ITF_UIPROFILESLOTCOMPONENT_H_
#define _ITF_UIPROFILESLOTCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_


namespace ITF
{
    class Actor;
    class UIProfileSlotComponent : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UIProfileSlotComponent, UIComponent, 1711782705);

    public:
        DECLARE_SERIALIZE()

        UIProfileSlotComponent();
        virtual ~UIProfileSlotComponent();

        virtual     bbool       needsUpdate         (       ) const             { return btrue; }
        virtual     bbool       needsDraw           (       ) const             { return bfalse; }
        virtual     bbool       needsDraw2D         (       ) const             { return bfalse; }
        virtual     void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     void        Update              ( f32 _deltaTime );

        virtual     void        onActorClearComponents();
        virtual     void        onBecomeActive();
        virtual     void        onBecomeInactive();

        virtual     void        onPressed           ();
        virtual     void        onReleased          ();
        virtual     void        onRollover          ();
        virtual     void        onRollout           ();
        virtual     void        onAction            (const StringID & action);
        ITF_INLINE u32          getPlayerIndex() const { return m_playerIndex; }
        void                    setPlayerIndex(u32 index) { m_playerIndex = index; }
        ITF_INLINE const String8& getResetButtonPath() const { return m_resetButtonPath; }
        ITF_INLINE const String8& getControlsPath() const { return m_controlsPath; }
        ITF_INLINE const String8& getPresetPath() const { return m_presetPath; }
        ITF_INLINE const String8& getPlayerTitlePath() const { return m_playerTitlePath; }
        ITF_INLINE const String8& getActionsBgPath() const { return m_actionsBgPath; }
        ITF_INLINE const String8& getActionUpPath() const { return m_actionUpPath; }
        ITF_INLINE const String8& getActionDownPath() const { return m_actionDownPath; }
        ITF_INLINE const String8& getActionLeftPath() const { return m_actionLeftPath; }
        ITF_INLINE const String8& getActionRightPath() const { return m_actionRightPath; }
        ITF_INLINE const String8& getActionRunPath() const { return m_actionRunPath; }
        ITF_INLINE const String8& getActionJumpPath() const { return m_actionJumpPath; }
        ITF_INLINE const String8& getActionHitPath() const { return m_actionHitPath; }
        ITF_INLINE const String8& getActionBackPath() const { return m_actionBackPath; }
        bbool                    isControllerConnected() const;
        InputAdapter::PadType    getControllerType() const;
        static u32               getConnectedControllersCount();
        static void              logAllControllersState();

        // IEventListener interface
        virtual void             onEvent(Event* _event) override;

    protected:
        ITF_INLINE const class UIProfileSlotComponent_Template* getTemplate() const;

    private:
        virtual void            clear();
        void                    registerEventListeners();
        void                    unregisterEventListeners();
        void                    setActorVisibility(const String8& actorPath, bbool visible);
        void                    setActorBackgroundTexture(const String8& actorPath, const Path& texturePath);
        void                    updateAllVisibility();
        void                    onControllerStateChanged(u32 padIndex, bbool connected, InputAdapter::PadType padType);

        u32                     m_playerIndex;
        String8                 m_resetButtonPath;
        String8                 m_controlsPath;
        String8                 m_presetPath;
        String8                 m_playerTitlePath;
        String8                 m_actionsBgPath;
        String8                 m_actionUpPath;
        String8                 m_actionDownPath;
        String8                 m_actionLeftPath;
        String8                 m_actionRightPath;
        String8                 m_actionRunPath;
        String8                 m_actionJumpPath;
        String8                 m_actionHitPath;
        String8                 m_actionBackPath;
        bbool                   m_isControllerConnected;
        InputAdapter::PadType   m_controllerType;
        bbool                   m_eventListenerRegistered;
    };

    class UIProfileSlotComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UIProfileSlotComponent_Template, UIComponent_Template, 4231517306);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UIProfileSlotComponent);

    public:
        UIProfileSlotComponent_Template();
        ~UIProfileSlotComponent_Template();

        ITF_INLINE const Path& getActionsBgTextureConnected() const { return m_actionsBgTextureConnected; }
        ITF_INLINE const Path& getActionsBgTextureDisconnected() const { return m_actionsBgTextureDisconnected; }

    private:
        Path                    m_actionsBgTextureConnected;
        Path                    m_actionsBgTextureDisconnected;
    };

    ITF_INLINE const UIProfileSlotComponent_Template* UIProfileSlotComponent::getTemplate() const
    {
        return static_cast<const UIProfileSlotComponent_Template*>(m_template);
    }

}
#endif // _ITF_UIPROFILESLOTCOMPONENT_H_
