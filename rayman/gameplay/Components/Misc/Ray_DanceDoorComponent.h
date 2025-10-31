#ifndef _ITF_RAY_DANCEDOORCOMPONENT_H_
#define _ITF_RAY_DANCEDOORCOMPONENT_H_

namespace ITF
{

    class AnimLightComponent;
    class Ray_SubAnchorComponent;
    class LinkComponent;

    class Ray_DanceDoorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DanceDoorComponent, ActorComponent,3935928756)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT();

    public:

        Ray_DanceDoorComponent();
        virtual ~Ray_DanceDoorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        Update( f32 _dt );
        virtual void        onEvent(Event * _event);

                void        registerEye(ObjectRef _objRef);
                bbool       isReadyForOpen() const;
                bbool       isOpen() const;
                bbool       danceIsStarted() const;
                bbool       warmupIsStarted() const;


    private:

        void                onCheckpointLoaded();

        void                init();

        void                processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty );
        void                processTrigger( EventTrigger* _trigger );

        ObjectRef           searchEmptyStone();
        void                udpateWarmupAct( f32 _time );

        bbool               playAnim( const StringID& _anim );

    private:
        ITF_INLINE const class Ray_DanceDoorComponent_Template * getTemplate() const;

        AnimLightComponent*     m_animLightComponent;
        Ray_SubAnchorComponent* m_subAnchorComponent;
        LinkComponent*          m_linkComponent;

        u32                     m_state;
        f32                     m_time;
        bbool                   m_isInit;

        ActorRef                m_warmupRef;
        u32                     m_stateWarmup;

        struct Slot
        {

            Slot()
            : m_eye(ITF_INVALID_OBJREF)
            , m_isReserved(bfalse)
            {
                //none
            }

            Slot(ActorRef _eyeAct)
            : m_eye(_eyeAct)
            , m_isReserved(bfalse)
            {
                // none
            }

            ActorRef    m_eye;
            bbool       m_isReserved;
        };

        SafeArray<Slot, 5>  m_ArraySlots;
        bbool               m_musicPlaying;

    };

    //-------------------------------------------------------------------------------------
    class Ray_DanceDoorComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DanceDoorComponent_Template,TemplateActorComponent,2116776617);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DanceDoorComponent);

    public:

        Ray_DanceDoorComponent_Template();
        ~Ray_DanceDoorComponent_Template() {}

        ITF_INLINE StringID getClosedAnim() const               { return m_animClosed; }
        ITF_INLINE StringID getClosedWithMusicAnim() const      { return m_animClosedWithMusic; }
        ITF_INLINE StringID getOpeningAnim() const              { return m_animOpening; }

        ITF_INLINE f32      getDanceTime() const                { return m_danceTime; }
        ITF_INLINE f32      getWarmupTime() const                { return m_warmupTime; }


    private:

        StringID    m_animClosed;
        StringID    m_animClosedWithMusic;
        StringID    m_animOpening;

        f32         m_warmupTime;
        f32         m_danceTime;

    };

ITF_INLINE const class Ray_DanceDoorComponent_Template * Ray_DanceDoorComponent::getTemplate() const
{
    return static_cast<const class Ray_DanceDoorComponent_Template *>(m_template);
}


} // namespace

#endif //_ITF_RAY_DANCEDOORCOMPONENT_H_