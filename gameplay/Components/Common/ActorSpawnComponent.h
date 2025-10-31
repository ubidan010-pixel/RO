#ifndef _ITF_ACTORSPAWNCOMPONENT_H_
#define _ITF_ACTORSPAWNCOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Spawns actors once (from a LUA-defined list), then make them follow one of our bones.
    // See TimedSpawnerComponent to spawn actors at intervals.
    class ActorSpawnComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ActorSpawnComponent,ActorComponent,2920550788);

    public:
        DECLARE_SERIALIZE()

        ActorSpawnComponent();
        ~ActorSpawnComponent();

        virtual bbool           needsUpdate() const { return btrue; }
        virtual bbool           needsDraw() const { return bfalse; }
        virtual bbool           needsDraw2D() const { return bfalse; }

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onBecomeActive();
        virtual void            onBecomeInactive();
        virtual void            onResourceReady();
#ifdef ITF_SUPPORT_EDITOR
        virtual void            onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        virtual void            Update( f32 _deltaTime );
        virtual void            onEvent( Event* _event );
        
                void            destroySpawns( bbool _direct = bfalse ) { clear(_direct); pause(); }

    private:
        ITF_INLINE const class ActorSpawnComponent_Template* getTemplate() const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        struct SpawnData
        {
            DECLARE_SERIALIZE()
            SpawnData()
                : m_spawnRef(ITF_INVALID_OBJREF)
                , m_boneIndex(U32_INVALID)
                , m_boneEnd(bfalse)
            {}

            ObjectRef                   m_spawnRef;
            u32                         m_boneIndex;
            bbool                       m_boneEnd;
        };
        typedef ITF_VECTOR<SpawnData> SpawnList;


        bbool                       m_alreadySpawned;
        
        void                        clear( bbool _direct = bfalse );
        void                        spawn();
        void                        readBoneInfo( const ITF_STDSTRING& _boneName, SpawnData& _spawn );
        void                        updateAllSpawn();
        void                        updateSpawn( const StringID& _polylineRef, const StringID& _polylinePointRef,
                                                 bbool _useParentScale, bbool _useParentFlip, bbool useParentAngle, const Vec3d& _offset, SpawnData& _spawn );
        void                        updateSpawnFromBone( Actor* _actor, u32 _boneIndex, bbool _boneEnd, bbool _useParentAngle, const Vec3d& _offset );
        void                        updateSpawnFromPolyline( Actor* _actor, const StringID& _polyID, const StringID& _pointID, bbool _useParentAngle, const Vec3d& _offset );
        void                        spawnActor( const Path& _lua, bbool _useParentScale, bbool _useParentFlip, const Vec3d& _offset, SpawnData& _spawn );
        
        void                        processForceOnActor( class EventForceOnActor* _forceInfo );
        void                        sendEventToSpawns( Event* _event );

        bbool                       m_spawnDelayed;
        SpawnList                   m_spawnList;
        class AnimLightComponent*   m_anim;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class ActorSpawnComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ActorSpawnComponent_Template, TemplateActorComponent,3516031925);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ActorSpawnComponent);
    public:

        ///////////////////////////////////////////////////////////////////////////////////////////
        struct SpawnData
        {
            DECLARE_SERIALIZE()
            SpawnData()
                : m_useParentScale(btrue)
                , m_useParentFlip(btrue)
                , m_useParentAngle(btrue)
                , m_offset(Vec3d::Zero)
            {}

            Path                        m_spawnActorLua;
            ITF_STDSTRING               m_boneName;
            StringID                    m_polylineRef;
            StringID                    m_polylinePointRef;
            bbool                       m_useParentScale;
            bbool                       m_useParentFlip;
            bbool                       m_useParentAngle;
            Vec3d                       m_offset;
        };
        typedef ITF_VECTOR<SpawnData> SpawnList;

        ActorSpawnComponent_Template() : m_spawnDelayed(bfalse) {}
        ~ActorSpawnComponent_Template() {}

        ITF_INLINE bbool getSpawnDelayed() const { return m_spawnDelayed; }
        const SpawnList &getSpawnList() const {return m_spawnList;}

    private:
        bbool m_spawnDelayed;
        SpawnList m_spawnList;
    };

    ITF_INLINE const class ActorSpawnComponent_Template* ActorSpawnComponent::getTemplate() const
    { return static_cast<const ActorSpawnComponent_Template*>(m_template); }
}

#endif // _ITF_ACTORSPAWNCOMPONENT_H_
