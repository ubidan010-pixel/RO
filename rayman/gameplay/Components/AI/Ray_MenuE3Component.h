#ifndef _ITF_RAYMENUE3COMPONENT_H_
#define _ITF_RAYMENUE3COMPONENT_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

namespace ITF
{
    class FXControllerComponent;

    class Ray_MenuE3Component : public ActorComponent, public IInputListener
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MenuE3Component, ActorComponent,786489758)
        DECLARE_SERIALIZE()

    public:

        Ray_MenuE3Component();
        virtual ~Ray_MenuE3Component();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        Update( f32 _dt );

        bbool               applyGamePadButton(InputAdapter::PressStatus* _buts, float* _pos);
        void                updateWiiInput(u32 playerIndex, InputAdapter::PressStatus* _buts, float* _pos);
        void                LaunchMap();
        void                ApplyMenuPauseButton();

        bbool               canReceive(); 

        virtual void        Receive( u32 deviceID, f32 axis, const StringID& action );

    private:
        ITF_INLINE const class Ray_MenuE3Component_Template* getTemplate() const;

        void playSound(const StringID& _sound);

        i32                     m_currentAnimIndex;
        u32                     m_subAnimCount;
        AnimLightComponent *    m_animLightComponent;
        FXControllerComponent * m_fxControllerComponent;

        bbool                       m_isInGameMenu;
        Vec2d                       m_offset2D;
        f32                         m_zOffset;
        bbool                   m_listenerRegistered;

        // Inputs
        f32                     m_repetitionTime;
        f32                     m_firstRepetitionTime;
        f32                     m_timer;
        bbool                   m_bJustPressed;
        bbool                   m_bPressed;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_MenuE3Component_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MenuE3Component_Template, TemplateActorComponent,3239430128);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MenuE3Component);

    public:

        Ray_MenuE3Component_Template()
            : m_music(StringID::Invalid)
            , m_validationSound(StringID::Invalid)
            , m_inGameMenu(false)
            , m_offset2D(Vec2d::Zero)
            , m_zOffset(0.0f)
            , m_pauseAnimSize(Vec2d::One)
        {}
        virtual ~Ray_MenuE3Component_Template() {}

        typedef ITF_VECTOR<Path>     MapPaths;
        ITF_INLINE const MapPaths&  getMaps() const { return m_maps; }
        ITF_INLINE const StringID&  getMusicID() const { return m_music; }
        ITF_INLINE const StringID&  getValidationsoundID() const { return m_validationSound; }
        ITF_INLINE const StringID&  getSelectionsoundID() const { return m_selectionSound; }
        ITF_INLINE const bbool      getInGameMenu() const { return m_inGameMenu; }
        ITF_INLINE const Vec2d      getOffset2D() const { return m_offset2D;}
        ITF_INLINE const f32        getZOffset() const { return m_zOffset;}
        ITF_INLINE const Vec2d      getPauseAnimSize() const { return m_pauseAnimSize;}

    private:

        MapPaths                    m_maps;
        StringID                    m_music;
        StringID                    m_validationSound;
        StringID                    m_selectionSound;
        bbool                       m_inGameMenu;
        Vec2d                       m_offset2D;
        f32                         m_zOffset;
        Vec2d                       m_pauseAnimSize;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_MenuE3Component_Template* Ray_MenuE3Component::getTemplate() const
    {
        return static_cast<const Ray_MenuE3Component_Template*>(m_template);
    }
};

#endif //_ITF_RAYMENUE3COMPONENT_H_