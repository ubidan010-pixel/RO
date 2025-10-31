#ifndef _ITF_RAY_FIREFLYKRILLAICOMPONENT_H_
#define _ITF_RAY_FIREFLYKRILLAICOMPONENT_H_

namespace ITF
{
    //forward
    class Ray_AbyssLightComponent;
    class Ray_FireFlyKrillAIComponent_Template;

    class Ray_FireFlyKrillAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FireFlyKrillAIComponent, ActorComponent,737561430)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    private:
        enum KrillState
        {
            KrillState_UNDEFINED = -1,
            KrillState_IDLE = 0,
            KrillState_FOLLOW
        };

    public:
        //static members used to manage players <-> krill association
        static bbool isPlayerMatched(const ActorRef & _playerRef);

        Ray_FireFlyKrillAIComponent();
        
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return btrue; }
        virtual bbool needsDraw2D() const { return bfalse; }

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onActorClearComponents();
        virtual void onBecomeActive();
        virtual void onBecomeInactive();
        virtual void Update(f32 _dt);
        virtual void Draw();

        void setTarget(const ActorRef & playerRef);

    private :
        //static members used to manage players <-> krill association
        static void registerKrill(const ActorRef & _krillRef);
        static void unregisterKrill(const ActorRef & _krillRef);
        static void matchPlayerWithKrill(const ActorRef & _playerRef, const ActorRef & _krillRef);
        static void removePlayerMatchingFromKrill(const ActorRef & _krillRef);

		//get actor template
		ITF_INLINE const Ray_FireFlyKrillAIComponent_Template * const getTemplate() const;

        //registration management
        void registerInstance();
        void unregisterInstance();

        //krill spawner detection
        void updateSpawnerDetection(const f32 _dt);

        //target positions management
        void initTargets(const u32 _nbTargets);
        void updateTargets(const f32 _dt);
        Vec2d getTargetAbsolutePosition(const u32 _particleIndex) const;
        Vec2d getTargetRelativePosition(const u32 _particleIndex) const;

        //particles management
        void loadResource();
        void unloadResource();
        void initParticles(const u32 _nbParticles);
        void updateParticles(const f32 _dt);
        void computeSeparation(const u32 _index, const Vec2d & _particlePosition, const f32 _factor, const f32 _distance, Vec2d & _acceleration) const;
        f32 computeAttraction(const u32 _index, const Vec2d & _particlePosition, const f32 _factor, Vec2d & _acceleration) const;
        void computeFriction(const Vec2d &_particleSpeed, const Vec2d _wantedAcceleration, const f32 _distanceFromTarget, const f32 _factor, Vec2d & _acceleration) const;
        void checkParticleLifeDuration(const u32 _index, const bbool _outsideVent, const f32 _currentTime, const f32 _deathDurationInv, const f32 _dt);
        Vec2d getParticleSpawningPosition() const;

        //player detection management
        bbool isPlayerDetected() const;

        //player protection management
        void computeProtectionRadius(const f32 _dt);
        //update light offset from particles barycenter
        void updateLightOffset();

        //state management
        void changeState(const KrillState _state);
        void updateState(const KrillState _state, const f32 _dt);
        void startIdle();
        void updateIdle(const f32 _dt);
        void startFollow();
        void updateFollow(const f32 _dt);
        void stopFollow();

        //mesh management
        void createMesh();
        void buildMesh();
        void drawMesh();
        void destroyMesh();

    private:

        static const f32 KRILL_MIN_INTRA_DISTANCE;
        //static members used to manage players <-> krill association
        static ActorRefList ms_krills;
        typedef ITF_MAP<ActorRef, ActorRef> KrillPlayerMatching;
        static KrillPlayerMatching ms_playerMatching;

        //Mesh
        ITF_Mesh m_mesh;
        SafeArray<VertexPCT> m_vertexBufferCache;
        ResourceID m_resource;

        bbool m_instanceRegistered;

        struct KrillParticleData
        {
            KrillParticleData()
            : m_position(Vec2d::Zero)
            , m_speed(Vec2d::Zero)
            , m_lifeDuration(0.0f)
            , m_deathRatio(0.0f)
            , m_angle(0.0f)
            , m_size(0.0f)
            , m_active(bfalse)
            {
            }
            Vec2d m_position;
            Vec2d m_speed;
            f32 m_lifeDuration;
            f32 m_deathRatio;
            f32 m_angle;
            f32 m_size;
            bbool m_active;
        };
        SafeArray<KrillParticleData> m_krillParticleData;
        u32 m_nbParticles;

        enum KrillTargetMode
        {
            KrillTargetMode_UNDEFINED = -1,
            KrillTargetMode_ROTATION,
            KrillTargetMode_DOCKED
        };
        KrillTargetMode m_targetsMode;

        struct KrillTarget
        {
            KrillTarget()
            : m_position(Vec2d::Zero)
            , m_rotationSpeedFactor(1.0f)
            , m_currentRotation(0.0f)
            {
            }
            Vec2d m_position;
            f32 m_rotationSpeedFactor;
            f32 m_currentRotation;
        };
        SafeArray<KrillTarget> m_targets;
        f32 m_targetsAngle;

        ActorRef m_playerRef;
        Vec2d m_playerSmoothedPosition;
        Vec2d m_speed;
        Vec2d m_playerOffset;

        KrillState m_state;

        Ray_AbyssLightComponent * m_abyssLightComponent;

        f32 m_targetsWantedRotationSpeed;
        f32 m_targetsCurrentRotationSpeed;
        f32 m_particleSurface;
        f32 m_protectionRadius;
        f32 m_protectionSurface;

        u32 m_nbActiveParticles;
        Vec2d m_particlesBarycenter;
        Vec2d m_lightOffset;

        bbool m_outOfSpawnerRange;
        Vec2d m_spawnerPosition;
        f32 m_timeOutOfSpawnerRange;
    };

    class Ray_FireFlyKrillAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FireFlyKrillAIComponent_Template, TemplateActorComponent,4063470096);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FireFlyKrillAIComponent);

    public:
        Ray_FireFlyKrillAIComponent_Template();
        ~Ray_FireFlyKrillAIComponent_Template();

        ITF_INLINE u32 getNbGroups() const;
        ITF_INLINE u32 getNbParticles() const;
        ITF_INLINE f32 getParticleMinSize() const;
        ITF_INLINE f32 getParticleMaxSize() const;
        ITF_INLINE f32 getKrillRadius() const;
        ITF_INLINE f32 getKrillGenerationRadius() const;
        ITF_INLINE f32 getLifeDuration() const;
        ITF_INLINE f32 getLifeDurationRandom() const;
        ITF_INLINE f32 getDeathDuration() const;
        ITF_INLINE f32 getSeparationDistance() const;
        ITF_INLINE f32 getSeparationFactor() const;
        ITF_INLINE f32 getAttractionFactor() const;
        ITF_INLINE f32 getFrictionFactor() const;
        ITF_INLINE f32 getPositionOffsetFactor() const;
        ITF_INLINE f32 getSpeed() const;
        ITF_INLINE f32 getKrillRespawnSpeedFactor() const;
        ITF_INLINE f32 getKrillBarycenterInfluence() const;
        ITF_INLINE const Path & getTextureFile() const;

    private:
        u32 m_nbGroups;
        u32 m_nbParticles;
        f32 m_particleMinSize;
        f32 m_particleMaxSize;
        f32 m_krillRadius;
        f32 m_krillGenerationRadius;
        f32 m_lifeDuration;
        f32 m_lifeDurationRandom;
        f32 m_deathDuration;
        f32 m_separationDistance;
        f32 m_separationFactor;
        f32 m_attractionFactor;
        f32 m_frictionFactor;
        f32 m_positionOffsetFactor;
        f32 m_speed;
        f32 m_krillRespawnSpeedFactor;
        f32 m_krillBarycenterInfluence;
        Path m_textureFile;
    };


    ITF_INLINE const Ray_FireFlyKrillAIComponent_Template * const Ray_FireFlyKrillAIComponent::getTemplate() const
    {
        return static_cast<const Ray_FireFlyKrillAIComponent_Template * const>( m_template );
    }



    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getKrillRadius() const
    {
        return m_krillRadius;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getKrillGenerationRadius() const
    {
        return m_krillGenerationRadius;
    }

    ITF_INLINE u32 Ray_FireFlyKrillAIComponent_Template::getNbParticles() const
    {
        return m_nbParticles;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getParticleMinSize() const
    {
        return m_particleMinSize;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getParticleMaxSize() const
    {
        return m_particleMaxSize;
    }

    ITF_INLINE u32 Ray_FireFlyKrillAIComponent_Template::getNbGroups() const
    {
        return m_nbGroups;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getLifeDuration() const
    {
        return m_lifeDuration;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getLifeDurationRandom() const
    {
        return m_lifeDurationRandom;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getDeathDuration() const
    {
        return m_deathDuration;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getSeparationDistance() const
    {
        return m_separationDistance;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getAttractionFactor() const
    {
        return m_attractionFactor;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getSeparationFactor() const
    {
        return m_separationFactor;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getFrictionFactor() const
    {
        return m_frictionFactor;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getPositionOffsetFactor() const
    {
        return m_positionOffsetFactor;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getSpeed() const
    {
        return m_speed;
    }

    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getKrillRespawnSpeedFactor() const
    {
        return m_krillRespawnSpeedFactor;
    }
        
    ITF_INLINE f32 Ray_FireFlyKrillAIComponent_Template::getKrillBarycenterInfluence() const
    {
        return m_krillBarycenterInfluence;
    }
        
    ITF_INLINE const Path & Ray_FireFlyKrillAIComponent_Template::getTextureFile() const
    {
        return m_textureFile;
    }
}

#endif //_ITF_RAY_FIREFLYKRILLAICOMPONENT_H_