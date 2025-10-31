#ifndef _ITF_RAY_ROCKFISHAICOMPONENT_H_
#define _ITF_RAY_ROCKFISHAICOMPONENT_H_

namespace ITF
{
    class DetectorComponent;

    class Ray_RockFishAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RockFishAIComponent, ActorComponent,1602147762)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_RockFishAIComponent();
        virtual ~Ray_RockFishAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onEvent(Event * _event);


#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_RockFishAIComponent_Template* getTemplate() const;
        bbool               processNewCatch( Actor* travelerActor );
        bbool               checkAttackDetection();
        bbool               checkAttack();
        bbool               checkDetected();

        void startClose();
        void startOpen();
        void startDetected();
        void startIdle();

        void updateClose(f32 _dt);
        void updateOpen(f32 _dt);
        void updateDetected(f32 _dt);
        void updateIdle(f32 _dt);

        DetectorComponent*  m_detectorComponent;
        AnimatedComponent * m_animComponent;

        ActorRefList        m_actorsInsideLastFrame;

        PhysShape * m_detectionShape;

        enum State
        {
            State_idle,
            State_detected,
            State_open,
            State_close,
        };

        State m_state;

        f32 m_timer;
        bbool m_previouslyDetected;
        PolyLine * m_polyline;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_RockFishAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RockFishAIComponent_Template, TemplateActorComponent,1700857009);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_RockFishAIComponent);

    public:

        Ray_RockFishAIComponent_Template();
        virtual ~Ray_RockFishAIComponent_Template();
        ITF_INLINE const PhysShape * getAttackDetectionShape() const { return m_attackDetectionShape; }

        ITF_INLINE ITF::StringID getSleepAnim() const { return m_sleepAnim; }
        ITF_INLINE ITF::StringID getDetectAnim() const { return m_detectAnim; }
        ITF_INLINE ITF::StringID getOpenAnim() const { return m_openAnim; }
        ITF_INLINE ITF::StringID getCloseAnim() const { return m_closeAnim; }
        ITF_INLINE f32 getDetectionRadius() const { return m_detectionRadius; }
        ITF_INLINE f32 getMinDetectTime() const { return m_minDetectTime; }
        ITF_INLINE f32 getMinIdleTime() const { return m_minIdleTime; }
        ITF_INLINE f32 getOpenTime() const { return m_openTime; }

        ITF_INLINE const SafeArray<Vec2d> & getPointList() const { return m_pointList; }
        ITF_INLINE ITF::StringID getRegionType() const { return m_regionType; }
        ITF_INLINE const ITF::Path & getGameMaterial() const { return m_gameMaterial; }
    private:
        PhysShape * m_attackDetectionShape;
        f32 m_detectionRadius;

        StringID m_sleepAnim;
        StringID m_detectAnim;
        StringID m_openAnim;
        StringID m_closeAnim;

        f32 m_minDetectTime;
        f32 m_minIdleTime;
        f32 m_openTime;
        StringID                        m_regionType;
        SafeArray <Vec2d>               m_pointList;
        Path                            m_gameMaterial;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_RockFishAIComponent_Template* Ray_RockFishAIComponent::getTemplate() const
    {
        return static_cast<const Ray_RockFishAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_ROCKFISHAICOMPONENT_H_