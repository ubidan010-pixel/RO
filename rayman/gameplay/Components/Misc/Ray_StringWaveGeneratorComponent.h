#ifndef _ITF_RAY_STRINGWAVEGENERATORCOMPONENT_H_
#define _ITF_RAY_STRINGWAVEGENERATORCOMPONENT_H_

namespace ITF
{
    class FluidShaper;
    class EventTrigger;
    class FXControllerComponent;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_StringWaveGeneratorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StringWaveGeneratorComponent, ActorComponent,2754583878);
    public:
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

        Ray_StringWaveGeneratorComponent();
        ~Ray_StringWaveGeneratorComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual void                Update( f32 _deltaTime );
        virtual void                onEvent( Event* _event );
        virtual bbool               needsUpdate() const {return btrue;}
        virtual bbool               needsDraw() const {return bfalse;}
        virtual bbool               needsDraw2D() const {return bfalse;}

    protected:
        enum State_t {State_NA, State_Off, State_OffToOn, State_On};
        enum WaveType_t {Wave_Bouncer=0, Wave_Spikes=1, Wave_Pusher=2};
        enum WaveLayer_t {Layer_Normal=0, Layer_Bouncer=1, Layer_Spikes=2, Layer_Pusher=3};

        struct FluidShaperDesc
        {
            FluidShaperDesc() 
                : m_waveType(Wave_Bouncer)
                , m_shaper(NULL)
                , m_speed(0)
                , m_offset(0)
                , m_timeLeftBeforeMoving(0)
                , m_hasTemporalAnnouncement(bfalse)
                , m_waveFXHandle(U32_INVALID)
                , m_preparingWaveFXHandle(U32_INVALID)
                , m_minX(FLT_MAX)
                , m_maxX(-FLT_MAX)
                {}
            FluidShaperDesc(WaveType_t _type, FluidShaper *_shaper/*you need to manually increase ref count*/, f32 _speed, f32 _timeBeforeMoving)
                : m_waveType(_type)
                , m_shaper(_shaper)
                , m_offset(0)
                , m_speed(_speed)
                , m_timeLeftBeforeMoving(_timeBeforeMoving)
                , m_hasTemporalAnnouncement(bfalse) 
                , m_waveFXHandle(U32_INVALID)
                , m_preparingWaveFXHandle(U32_INVALID)
                , m_minX(FLT_MAX)
                , m_maxX(-FLT_MAX)
            {}
            
            WaveType_t m_waveType;
            FluidShaper * m_shaper;
            f32 m_offset;
            f32 m_timeLeftBeforeMoving;
            f32 m_speed;
            bbool m_hasTemporalAnnouncement;
            u32 m_waveFXHandle, m_preparingWaveFXHandle;
            f32 m_minX, m_maxX;

        };

        struct FluidShaperApplication
        {
            FluidShaperApplication() : m_layerIndex(0), m_shaper(NULL), m_lastFrame(U32_INVALID) {}
            ObjectRef m_friezeRef;
            i32 m_layerIndex;
            FluidShaper *m_shaper;
            u32 m_lastFrame;
        };

        bbool                       m_startsActivated;
        bbool                       m_activated;
        f32                         m_phantomRadius;
        SafeArray<FluidShaperDesc,8> m_fluidShapers;
        ObjectRef                   m_relatedFriezeRef;
        Vec2d                       m_normalizedEdgeDir;
        State_t                     m_state;
        f32                         m_timeInCurrentState;
        f32                         m_scaleBecauseOfTransition;
        bbool                       m_needsFrieze;
        FXControllerComponent       *m_fxControllerComponent;
        u32                         m_offToOnFX;
        Vec3d                       m_pointWithProjection0;
        SafeArray<FluidShaperApplication> m_registeredShaperApplications;

        i32                         projectOnPointList(Frise *frise, const Vec2d &pos, const PolyPointList& _pointList, f32 &_percentInSegment, Vec2d &_edgeDirection, f32 &_bestDistance, Vec3d &_posWithProjection0);
        void                        computeRelatedFrieze();
        void                        invalidateRelatedFrieze();
        class Frise*                getLinkedFrieze();
        void                        processTrigger( EventTrigger* _eventTrigger );
        void                        processSpawnWave( Ray_EventSpawnWave* _eventSpawnWave );
#ifdef ITF_SUPPORT_EDITOR
        void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        void                        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
        void                        setShaperDynamicParameters(u32 _index, f32 _dt);
        bbool                       getFriezeProjectionOnEdge(Frise *_frieze, f32 &_minX, f32 &_maxX, Vec2d &_center);
        void                        setState(State_t _newState);
        void                        createNewDefaultWave();
        void                        createNewWave(WaveType_t _type, f32 _speed, f32 _timeBeforeMoving, bbool _offsetFromRadius);
        void                        applyStringWave(u32 index, Frise *_frieze);
        void                        popOldestWave(bbool _killSounds=btrue);
        ITF_INLINE u32              getLayerIndexFromType(WaveType_t _type);
        bbool                       decideIfMustSendExciterMessageToWaveBall(Frise *_linkedFrieze, FluidShaperDesc &_shaperDesc, f32 _previousOffset, bbool &_isPreparing);
        void                        sendWaveGeneratorNotification(FluidShaperDesc &_shaperDesc, bbool _exciterState, bbool _isPreparing_ExicterMessage);
        void                        stopOffToOnFXIfNeeded();
        void                        stopWaveFX(FluidShaperDesc *_shaperDesc);
        void                        stopPreparingWaveFX(FluidShaperDesc *_shaperDesc);
        void                        updateWaveFX(u32 _index, bbool _forcedDisable);
        const StringID &            getOffToOnFX(WaveType_t _type);
        const StringID &            getPreparingWaveFX(WaveType_t _type);
        const StringID &            getWaveFX(WaveType_t _type);
        bbool                       getPositionFromLinear(f32 _linear, Vec3d &_position);
        virtual void                onBecomeInactive();

        //because of a technical change, we use a Shaper Application list(m_registeredShaperApplications) : we register active fluid shapers each frame. Then, at update() end, we unregister those which were not updated
        FluidShaperApplication*     findWaveApplication(FluidShaper *_shaper, const ObjectRef &_friezeRef, i32 _layerIndex);
        void                        removeObsoleteRegisteredWaves(bbool _removeAll=bfalse);
        void                        registerWaveApplication(Frise *_frieze, FluidShaperDesc &_shaperDesc);

        //
        ITF_INLINE const class Ray_StringWaveGeneratorComponent_Template * getTemplate() const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_StringWaveGeneratorComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StringWaveGeneratorComponent_Template, TemplateActorComponent,2558825937);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_StringWaveGeneratorComponent);

    public:
        Ray_StringWaveGeneratorComponent_Template();

        bbool getIsSpike() const {return m_isSpike;}
        f32 getSyncOffset() const {return m_syncOffset;}
        bbool getSyncWithMetronome() const {return m_syncWithMetronome;}
        f32 getYScale() const {return m_yScale;}
        f32 getSyncRatio() const { return m_syncRatio; }
        f32 getSyncActivePercent() const {return m_syncActivePercent;}
        f32 getActorYScaleThreshold() const {return m_actorYScaleThreshold;}
        bbool getIsFullLength() const {return m_isFullLength;}
        f32 getGrowthTransitionDuration() const {return m_growthTransitionDuration;}
        f32 getSpikeMarginLength() const {return m_spikeMarginLength;}
        f32 getGrowthTransition_TimeStartsRed() const {return m_growthTransition_TimeStartsRed;}
        bbool getIsSpawner() const {return m_isSpawner;}
        u32 getSpawneeLimit() const {return m_spawneeLimit;}
        bbool getOnOffAffectsAllWaves() const {return m_onOffAffectsAllWaves;}
        f32 getSmallScaleWhenBlackInTransition() const {return m_smallScaleWhenBlackInTransition;}
        bbool getLimitWaveToBorders() const {return m_limitWaveToBorders;}
        f32 getColoredSpawningCueWidth() const {return m_coloredSpawningCueWidth;}
        f32 getPulseScaleWhenWaiting() const {return m_pulseScaleWhenWaiting;}
        f32 getPulseFreqWhenWaiting() const {return m_pulseFreqWhenWaiting;}
        const StringID &getOffToOnFX_Spikes() const {return m_offToOnFX_Spikes;}
        const StringID &getOffToOnFX_Bouncer() const {return m_offToOnFX_Bouncer;}
        const StringID &getWaveFX_Spikes() const {return m_waveFX_Spikes;}
        const StringID &getWaveFX_Bouncer() const {return m_waveFX_Bouncer;}
        const StringID &getPreparingWaveFX_Spikes() const {return m_preparingWaveFX_Spikes;}
        const StringID &getPreparingWaveFX_Bouncer() const {return m_preparingWaveFX_Bouncer;}

    private:

        bbool m_isSpike;
        f32 m_syncOffset;
        f32 m_syncRatio;
        bbool m_syncWithMetronome;
        f32 m_yScale;
        f32 m_syncActivePercent;
        f32 m_actorYScaleThreshold;
        bbool m_isFullLength;
        f32 m_growthTransitionDuration;
        f32 m_growthTransition_TimeStartsRed;
        f32 m_spikeMarginLength;
        bbool m_isSpawner;
        u32 m_spawneeLimit;
        bbool m_onOffAffectsAllWaves;
        f32 m_smallScaleWhenBlackInTransition;
        bbool m_limitWaveToBorders;
        f32 m_coloredSpawningCueWidth;
        f32 m_pulseScaleWhenWaiting;
        f32 m_pulseFreqWhenWaiting;
        StringID m_offToOnFX_Spikes, m_offToOnFX_Bouncer;
        StringID m_waveFX_Spikes, m_waveFX_Bouncer;
        StringID m_preparingWaveFX_Spikes, m_preparingWaveFX_Bouncer;

    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    ITF_INLINE const Ray_StringWaveGeneratorComponent_Template * Ray_StringWaveGeneratorComponent::getTemplate() const
    {
        return static_cast<const class Ray_StringWaveGeneratorComponent_Template *>(m_template);
    }

}

#endif
