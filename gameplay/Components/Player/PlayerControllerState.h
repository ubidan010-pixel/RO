#ifndef _ITF_PLAYERCONTROLLERSTATE_H_
#define _ITF_PLAYERCONTROLLERSTATE_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

namespace ITF
{
    class PlayerControllerComponent;
    class AnimatedComponent;
    class StickToPolylinePhysComponent;
    class FXControllerComponent;

    class PlayerControllerState : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(PlayerControllerState, IRTTIObject,3021798211);

    public:
        PlayerControllerState();
        virtual ~PlayerControllerState() {}

        virtual void        init( PlayerControllerComponent* _parent,
                                  AnimatedComponent* _animComponent,
                                  StickToPolylinePhysComponent* _characterPhys,
                                  FXControllerComponent* _fxController );

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/) {}
#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue) {}
#endif // ITF_SUPPORT_EDITOR
        virtual void        onEnter();
        virtual void        onExit() {}
        virtual bbool       checkStateChange( f32 _dt ) { return bfalse; }
        virtual void        update( f32 _dt ) {}
        virtual void        onEvent(Event * _event) {}
        virtual void        onBecomeActive() {}
        virtual void        onBecomeInactive() {}
        virtual void        updateAnimInputs() {}
        virtual void        updateSoundInputs() {}
        virtual void        updateFxInputs() {}
        virtual StringID    getAnimAction() const = 0;
        bbool               isCurrentAnimFinished() const;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void        drawDebug() {}
#endif // ITF_SUPPORT_DEBUGFEATURE

    protected:

        Actor*                              m_actor;
        PlayerControllerComponent*          m_parent;
        AnimatedComponent*                  m_animComponent;
        StickToPolylinePhysComponent*       m_characterPhys;
        FXControllerComponent*              m_fxController;
    };
};

#endif //_ITF_PLAYERCONTROLLERSTATE_H_

