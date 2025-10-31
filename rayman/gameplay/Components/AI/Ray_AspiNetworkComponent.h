#ifndef _ITF_RAY_ASPINETWORKCOMPONENT_H_
#define _ITF_RAY_ASPINETWORKCOMPONENT_H_

namespace ITF
{
    class DetectorComponent;
    class Ray_GeyserPlatformAIComponent;
    class FXControllerComponent;

    class Ray_AspiNetworkComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AspiNetworkComponent, ActorComponent,4207808234)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_AspiNetworkComponent();
        virtual ~Ray_AspiNetworkComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onEvent( class Event* _event );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_AspiNetworkComponent_Template* getTemplate() const;

        class FriseFollower
        {
        public:
            FriseFollower( const Frise* _frise, const Vec2d& _from, const Vec2d& _to );

            ITF_INLINE f32  getLength() const { return m_length; }

            Vec3d           getPos( f32 _dist );
            //Vec2d           getDir( f32 _dist );
            Vec3d           getEntryPoint();
            Vec2d           getEntryDir();
            Vec3d           getExitPoint();
            Vec2d           getExitDir();

#ifdef ITF_SUPPORT_DEBUGFEATURE
            void            drawDebug();
#endif // ITF_SUPPORT_DEBUGFEATURE

        private:
            bbool           parseEdge( const PolyLineEdge& _edge, const f32 _edgeStart, const f32 _edgeStop, f32& _dist, Vec3d& _pos );
            bbool           parseEdgeReverse( const PolyLineEdge& _edge, const f32 _edgeStart, const f32 _edgeStop, f32& _dist, Vec3d& _pos );

            const Frise*    m_frise;
            Matrix44        m_matrix;
            f32             m_length;

            PolyLineEdge    m_inEdge;
            i32             m_inEdgeIndex;
            f32             m_inEdgeRatio;

            PolyLineEdge    m_outEdge;
            i32             m_outEdgeIndex;
            f32             m_outEdgeRatio;
        };


        enum State
        {
            State_Enter,
            State_Travel,
            State_Exit,
        };

        struct Traveler
        {
            Traveler()
                : m_isPlayer(bfalse)
                , m_state(State_Enter)
                , m_timer(0.f)
                , m_initialZ(0.f)
                , m_fxInHandle(U32_INVALID)
            {
            }

            ActorRef    m_actor;
            bbool       m_isPlayer;
            State       m_state;
            f32         m_timer;
            Bezier4<Vec3d> m_enterBezier;
            f32         m_initialZ;
            u32         m_fxInHandle;
        };
        typedef SafeArray<Traveler> TravelerList;


        void                processNewTraveler( Actor* travelerActor, FriseFollower& friseFollower );

        LinkComponent*      m_linkComponent;
        DetectorComponent*  m_detectorComponent;
        Ray_GeyserPlatformAIComponent* m_geyserComponent;
        FXControllerComponent* m_fxController;

        ActorRefList        m_actorsInsideLastFrame;
        TravelerList        m_travelers;

        void playFx( Traveler& _traveler, Actor* _travelerActor );
        void updateFx( Traveler& _traveler, Actor* _travelerActor );
        void stopFx( Traveler& _traveler, Actor* _travelerActor );

#ifdef ITF_SUPPORT_EDITOR
        void                drawEditFrise() const;
#endif // ITF_SUPPORT_EDITOR
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_AspiNetworkComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AspiNetworkComponent_Template, TemplateActorComponent,502739156);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AspiNetworkComponent);

    public:

        class FxData
        {
            DECLARE_SERIALIZE();
        public:
            String8 m_playerFamily;
            StringID m_fxIn;
            StringID m_fxOut;
        };
        typedef ITF_VECTOR<FxData> FxDataList;

        const FxData* getFxData( Actor* _travelerActor ) const;


        Ray_AspiNetworkComponent_Template()
            : m_enterDuration(1.f)
            , m_enterBezierMultiplier(1.f)
            , m_speed(1.f)
            , m_exitSpeed(1.f)
            , m_exitDist(3.f)
            , m_exitRestoreZDist(1.f)
        {
        }
        virtual ~Ray_AspiNetworkComponent_Template() {}

        ITF_INLINE f32 getEnterDuration() const { return m_enterDuration; }
        ITF_INLINE f32 getEnterBezierMultiplier() const { return m_enterBezierMultiplier; }
        ITF_INLINE f32 getSpeed() const { return m_speed; }
        ITF_INLINE f32 getExitSpeed() const { return m_exitSpeed; }
        ITF_INLINE f32 getExitDist() const { return m_exitDist; }
        ITF_INLINE f32 getExitRestoreZDist() const { return m_exitRestoreZDist; }

    private:

        f32 m_enterDuration;
        f32 m_enterBezierMultiplier;
        f32 m_speed;
        f32 m_exitSpeed;
        f32 m_exitDist;
        f32 m_exitRestoreZDist;
        FxDataList m_fxData;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_AspiNetworkComponent_Template* Ray_AspiNetworkComponent::getTemplate() const
    {
        return static_cast<const Ray_AspiNetworkComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_ASPINETWORKCOMPONENT_H_