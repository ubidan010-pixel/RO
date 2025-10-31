
#ifndef _ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_
#define _ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_


// This component listens for a specific stim and when it arrives it in increases the position of an animation
// If no stim is received it will go back to the start
namespace ITF
{
    class EventReceivePlayAnimComponent : public ActorComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(EventReceivePlayAnimComponent,ActorComponent,3645229715);
        DECLARE_SERIALIZE()

        EventReceivePlayAnimComponent();
        ~EventReceivePlayAnimComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void                Update( f32 _dt );
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();
        virtual void                onEvent( Event* _event );

    protected:

        ITF_INLINE const class EventReceivePlayAnimComponent_Template*  getTemplate() const;
        void                        clear();
        void                        updateSpeed( f32 _dt );
        void                        updatePos( f32 _dt );
        void                        setResetTime();

        ///

        class AnimLightComponent*   m_animComponent;
        class PhysPhantom*          m_phantom;
        class SoundComponent *      m_soundComponent;
        class FXControllerComponent *m_fxControllerComponent;
        f32                         m_resetTimeLength;
        f32                         m_resetTimeCounter;
        f32                         m_resetTimeStartPos;
        f32                         m_currentSpeed;
        f32                         m_currentPos;
        f32                         m_resetDelayCounter;
        u32                         m_eventsReceived;
        bbool                       m_registered;
        bbool                       m_locked;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        bbool                       m_displayPhantom;
#endif // ITF_SUPPORT_DEBUGFEATURE
        f32                         m_startValue;

        u32                         m_fxHandle;
    };

    //---------------------------------------------------------------------------------------------------

    class EventReceivePlayAnimComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(EventReceivePlayAnimComponent_Template,TemplateActorComponent,2878807208);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(EventReceivePlayAnimComponent);

    public:

        EventReceivePlayAnimComponent_Template();
        ~EventReceivePlayAnimComponent_Template(); 

        const class PhysShape*      getPhantomShape() const { return m_phantomShape; }
        const class Event*          getEventWatch() const { return m_eventWatch; }
        const class Event*          getResetStartValueEvent() const { return m_resetStartValueEvent; }
        f32                         getAcceleration() const { return m_acceleration; }
        f32                         getDeceleration() const { return m_deceleration; }
        f32                         getMaxSpeed() const { return m_maxSpeed; }
        f32                         getMinSpeed() const { return m_minSpeed; }
        f32                         getResetTimeMin() const { return m_resetTimeMin; }
        f32                         getResetTimeMax() const { return m_resetTimeMax; }
        f32                         getResetDelay() const { return m_resetDelay; }
        bbool                       getLockOnEnd() const { return m_lockOnEnd; }
        bbool                       getUseSingleEvent() const { return m_useSingleEvent; }
        ITF_INLINE const Vec2d &    getPhantomOffset() const  { return m_phantomOffset; }
        ITF_INLINE const f32 &      getBounceFactor() const  { return m_bounceFactor; }

    private:

        class PhysShape*            m_phantomShape;
        class Event*                m_eventWatch;
        class Event*                m_resetStartValueEvent;
        f32                         m_acceleration;
        f32                         m_deceleration;
        f32                         m_maxSpeed;
        f32                         m_minSpeed;
        f32                         m_resetTimeMin;
        f32                         m_resetTimeMax;
        f32                         m_resetDelay;
        bbool                       m_lockOnEnd;
        bbool                       m_useSingleEvent;
        Vec2d                       m_phantomOffset;
        f32                         m_bounceFactor;
    };



    ITF_INLINE const EventReceivePlayAnimComponent_Template*  EventReceivePlayAnimComponent::getTemplate() const {return static_cast<const EventReceivePlayAnimComponent_Template*>(m_template);}

}

#endif // _ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_
