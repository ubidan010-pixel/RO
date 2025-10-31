
#ifndef _ITF_PLAYANIMONEVENTRECEIVECOMPONENT_H_
#define _ITF_PLAYANIMONEVENTRECEIVECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_


// This component will play an animation when an event is received, and will go back to the idle one
// when the animation is finished
namespace ITF
{
    class AnimLightComponent;
    class AnimatedComponent;
    class FXControllerComponent;

    class PlayAnimOnEventReceiveComponent : public ActorComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(PlayAnimOnEventReceiveComponent,ActorComponent,2859296107);
        DECLARE_SERIALIZE()

        PlayAnimOnEventReceiveComponent();
        ~PlayAnimOnEventReceiveComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void                Update( f32 _dt );
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onCheckpointLoaded();
        virtual void                onEvent( Event* _event );

    private:

        ITF_INLINE const class PlayAnimOnEventReceiveComponent_Template*  getTemplate() const;


        AnimLightComponent*         m_animComponent;
        AnimatedComponent*          m_animatedComponent;
        FXControllerComponent*      m_fxController;
        bbool                       m_playingEvent;
        bbool                       m_playingEventCheckpointSave;
        bbool                       m_eventReceived;
        bbool                       m_disabledAfterEvent;
    };

    //---------------------------------------------------------------------------------------------------

    class PlayAnimOnEventReceiveComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayAnimOnEventReceiveComponent_Template,TemplateActorComponent,3614863395);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PlayAnimOnEventReceiveComponent);

    public:

        PlayAnimOnEventReceiveComponent_Template()
            : Super()
            , m_resetOnRetrigger(btrue)
            , m_stayInEvent(bfalse)
            , m_stayInEventCheckpointSave(bfalse)
            , m_disableAfterEvent(bfalse)
            , m_useAnimatedComponentForAnimsFinish(bfalse)
            {}
        ~PlayAnimOnEventReceiveComponent_Template();

        const SafeArray <Event*>&   getListenEvents() const { return m_listenEvents; }
        StringID                    getIdleAnim() const { return m_idleAnim; }
        StringID                    getEventAnim() const { return m_eventAnim; }
        StringID                    getEventIdleAnim() const { return m_eventIdleAnim; }
        bbool                       getResetOnRetrigger() const { return m_resetOnRetrigger; }
        bbool                       getStayOnEvent() const { return m_stayInEvent; }
        bbool                       getStayOnEventCheckpointSave() const { return m_stayInEventCheckpointSave; }
        bbool                       getDisableAfterEvent() const { return m_disableAfterEvent; }
        bbool                       getUseAnimatedComponentForAnimsFinish() const { return m_useAnimatedComponentForAnimsFinish; }

    private:

        SafeArray <Event*>          m_listenEvents;
        StringID                    m_idleAnim;
        StringID                    m_eventAnim;
        StringID                    m_eventIdleAnim;
        bbool                       m_resetOnRetrigger;
        bbool                       m_stayInEvent;
        bbool                       m_stayInEventCheckpointSave;
        bbool                       m_disableAfterEvent;
        bbool                       m_useAnimatedComponentForAnimsFinish;
    };


    ITF_INLINE const PlayAnimOnEventReceiveComponent_Template*  PlayAnimOnEventReceiveComponent::getTemplate() const {return static_cast<const PlayAnimOnEventReceiveComponent_Template*>(m_template);}
}

#endif // _ITF_PLAYANIMONEVENTRECEIVECOMPONENT_H_
