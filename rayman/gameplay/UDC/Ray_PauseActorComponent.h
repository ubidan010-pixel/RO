#ifndef _ITF_RAY_PAUSEACTORCOMPONENT_H_
#define _ITF_RAY_PAUSEACTORCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

namespace ITF
{
    class PlayerDetectorComponent;

    class Ray_PauseActorComponent : public TriggerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PauseActorComponent, TriggerComponent,1841835503)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_PauseActorComponent();
        virtual ~Ray_PauseActorComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _dt );

    private:

        virtual void                sendEvent( ActorRef _activator, bbool _activated, const bbool _fromRetriggerOnCheckpoint = bfalse );

        void                        pauseChildren( bbool _pause );

        PlayerDetectorComponent*    m_playerDetectorComponent;

        f32                         m_alhpa;
        bbool                       m_goingToPause, m_alphaRunning;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_PauseActorComponent_Template : public TriggerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PauseActorComponent_Template,TriggerComponent_Template,1478000500);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PauseActorComponent);

    public:

        Ray_PauseActorComponent_Template() {}
        ~Ray_PauseActorComponent_Template() {}

    private:

    };
}

#endif //_ITF_RAY_PAUSEACTORCOMPONENT_H_