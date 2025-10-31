#ifndef _ITF_RAY_SHOOTERGARDIANMORAY_H_
#define _ITF_RAY_SHOOTERGARDIANMORAY_H_

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_RAY_SNAKEBODYPART_MIXED_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Mixed.h"
#endif //_ITF_RAY_SNAKEBODYPART_MIXED_H_

namespace ITF
{
    //-----------------------------------------------------------------------------------------------
    //                                      HEAD PART
    //-----------------------------------------------------------------------------------------------
    class Ray_ShooterGardianMorayBodyPart;
    class Ray_ShooterGardianMoray : public Ray_SnakeAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterGardianMoray, Ray_SnakeAIComponent,3395627550)
        DECLARE_SERIALIZE();

    public:

        Ray_ShooterGardianMoray();
        virtual ~Ray_ShooterGardianMoray();

        virtual void        Update( f32 _dt );
        virtual void        onActorLoaded( Pickable::HotReloadType /*_hotReload*/ );
        virtual void        onEvent( Event* _event );
        virtual void        onBecomeActive();

        Ray_ShooterGardianMorayBodyPart * getBodyPart( u32 _idx );

    private:

        void                updateDeathReward( f32 _dt );
        void                spawnDeathReward();

        f32         m_waterLevel;
        PolyLine*   m_waterLevelPolyLine;
        f32         m_spawnFxTimer;
        bbool       m_headInWater;
        bbool       m_tailInWater;

        SafeArray<i32> m_lastPartInWater;

        bbool       m_dead;
        bbool       m_tailVisible;

        SpawneeGenerator m_deathRewardGenerator;
        f32         m_deathRewardTimer;
        f32         m_deathNextSpawnTime;
        f32         m_deathLastSpawnDelay;

        ITF_INLINE const class Ray_ShooterGardianMoray_Template * getTemplate() const;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ShooterGardianMoray_Template : public Ray_SnakeAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterGardianMoray_Template, Ray_SnakeAIComponent_Template,1927756513);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterGardianMoray);

    public:

        Ray_ShooterGardianMoray_Template();
        virtual ~Ray_ShooterGardianMoray_Template(){}

        ITF_INLINE f32      getWaterPerturbationForce() const { return m_waterPerturbationForce; }
        ITF_INLINE f32      getWaterTestWidth()         const { return m_waterTestWidth; }
        ITF_INLINE f32      getWaterTestBubonWidth()    const { return m_waterTestBubonWidth; }
        ITF_INLINE f32      getWaterTestTailLength()    const { return m_waterTestTailLength; }
        ITF_INLINE f32      getWaterTestTailWidth()     const { return m_waterTestTailWidth; }
        ITF_INLINE f32      getWaterTestHeadLength()    const { return m_waterTestHeadLength; }
        ITF_INLINE f32      getWaterTestHeadWidth()     const { return m_waterTestHeadWidth; }
        ITF_INLINE f32      getWaterSpawnFxDelay()      const { return m_waterSpawnFxDelay; }
        ITF_INLINE const Path &     getDeathRewardSpawnPath()       const { return m_deathRewardSpawnPath; }
        ITF_INLINE u32              getDeathRewardNumber()          const { return m_deathRewardNumber; }
        ITF_INLINE f32              getDeathRewardSpawnDuration()   const { return m_deathRewardSpawnDuration; }
        ITF_INLINE const Vec2d &    getDeathRewardSpawnDist()       const { return m_deathRewardSpawnDist; }
        ITF_INLINE const Vec2d &    getDeathRewardSpawnAngle()      const { return m_deathRewardSpawnAngle; }
        
    private:
        f32     m_waterPerturbationForce;
        f32     m_waterTestWidth;
        f32     m_waterTestBubonWidth;
        f32     m_waterTestTailLength;
        f32     m_waterTestTailWidth;
        f32     m_waterTestHeadLength;
        f32     m_waterTestHeadWidth;
        f32     m_waterSpawnFxDelay;
        Path    m_deathRewardSpawnPath;
        u32     m_deathRewardNumber;
        f32     m_deathRewardSpawnDuration;
        Vec2d   m_deathRewardSpawnDist;
        Vec2d   m_deathRewardSpawnAngle;
      
    };

    ITF_INLINE const Ray_ShooterGardianMoray_Template* Ray_ShooterGardianMoray::getTemplate() const
    {
        return static_cast<const Ray_ShooterGardianMoray_Template*>(m_template);
    }

    //-----------------------------------------------------------------------------------------------
    //                                      BODY PART
    //-----------------------------------------------------------------------------------------------
    class Ray_ShooterGardianMorayBodyPart : public BodyPart
    {
        DECLARE_OBJECT_CHILD_RTTI( Ray_ShooterGardianMorayBodyPart, BodyPart ,2915376064)

    public:
        Ray_ShooterGardianMorayBodyPart();
        virtual ~Ray_ShooterGardianMorayBodyPart();

        virtual void        onInit();
        virtual void        clear();
        virtual void        update( f32 _dt );
        virtual void        onEvent( Event * _event );
        virtual bbool       receiveHit( HitStim * _hitStim, bbool _leftSide );
        virtual bbool       processHitStim( HitStim * _hitStim );
        virtual void        onTrajectoryUpdate();
        virtual bbool       removeAndKeepAliveOnDeath();
        virtual bbool       updateKeepAlive( f32 _dt );
        virtual void        positionOnTrajectory( const Vec3d& _pos, const f32 _angle );

        bbool               isBubonAlive( bbool _left ) const { return ( _left ? ( ( m_leftBubon.m_curState != Dead ) ? btrue : bfalse ) : ( ( m_rightBubon.m_curState != Dead ) ? btrue : bfalse ) ); }

        void                setHealthFactor( f32 _healthFactor )   { m_healthFactor = _healthFactor; updateMultiPlayerHealth(); m_prevHealthFactor = m_healthFactor; }

    protected:
        virtual Actor*      spawnBodyPart();
        virtual Actor*      createBodyPart();
        virtual void        onAnimFinished();

    private:
        enum BubonState
        {
            Stand = 0,
            Hit = 1,
            Death = 2,
            Dead = 3,
            ENUM_FORCE_SIZE_32( BubonState )
        };

        struct Bubon
        {
            Bubon() :   m_phantom( NULL ), 
                        m_boneIdx( U32_INVALID), 
                        m_health( -1 ), 
                        m_curState( Stand ),
                        m_wantedState( Stand ),
                        m_checkWantedStateNextFrame( bfalse ) {} 
            PhysPhantom *       m_phantom;
            PhysShapeCircle     m_shape;
            u32                 m_boneIdx;
            i32                 m_health;
            BubonState          m_curState;
            BubonState          m_wantedState;
            StringID            m_boneName;
            bbool               m_checkWantedStateNextFrame;
            StringID            m_animInput;
        };

        void                updateBubon( Bubon & _bubon );
        bbool               receiveBubonHit( Bubon & _bubon, HitStim * _hitStim );
        void                changeBubonState( Bubon & _bubon, BubonState _state );
        void                onBubonStateFinished( Bubon & _bubon );
        void                sendBubonStim( Bubon & _bubon );

        void                createPhantom( PhysPhantom * & _phatom, PhysShapeCircle & _shape );
        void                destroyPhantom( PhysPhantom * & _phatom );

        Actor *             spawnTail();
        void                swapToTail();
        void                startTailTransformation();

        void                updateMultiPlayerHealth();

        void                sendLumsReward( u32 _number, HitStim * _hitStim, Bubon * _bubon );
        
        Bubon               m_leftBubon;
        Bubon               m_rightBubon;
        SpawneeGenerator    m_rewardGenerator;
        SpawneeGenerator    m_tailGenerator;
        ActorRef            m_tailActor;
        i32                 m_defaultMaxHealthMulti;
        Ray_EventSpawnReward* m_bubonReward;
        bbool               m_destroyRewardGiven;
        f32                 m_healthFactor;                 // used to change the life and modifie the difficulty
        f32                 m_prevHealthFactor;

        f32                 m_tailInterpolationTimer;
        
        ITF_INLINE const class Ray_ShooterGardianMorayBodyPart_Template * getTemplate() const;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ShooterGardianMorayBodyPart_Template : public BodyPart_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterGardianMorayBodyPart_Template,BodyPart_Template,822361727);
        DECLARE_SERIALIZE();
        DECLARE_BODYPART_TEMPLATE(Ray_ShooterGardianMorayBodyPart);

    public:

        Ray_ShooterGardianMorayBodyPart_Template();
        virtual ~Ray_ShooterGardianMorayBodyPart_Template();

        ITF_INLINE const StringID & getLeftBubonBoneName()          const { return m_leftBubonBoneName; }
        ITF_INLINE const StringID & getRightBubonBoneName()         const { return m_rightBubonBoneName; }
        ITF_INLINE const f32        getBubonPhantomSize()           const { return m_bubonPhantomSize; }
        ITF_INLINE const Path &     getBubonRewardSpawnPath()       const { return m_bubonRewardSpawnPath; }
        ITF_INLINE const Path &     getTailPath()                   const { return m_tailPath; }
        ITF_INLINE const u32        getRewardNumber()               const { return m_rewardNumber; }
        ITF_INLINE const u32        getDestroyRewardNumber()        const { return m_destroyRewardNumber; }
        ITF_INLINE const i32        getBubonHealth()                const { return m_bubonHealth; }
        ITF_INLINE EReceivedHitType getHitType()					const { return m_hitType; }
        ITF_INLINE u32              getHitLevel()					const { return m_hitLevel; }
        ITF_INLINE Faction 			getFaction()					const { return m_faction; }
        ITF_INLINE const StringID & getTransfoTotailAnim()          const { return m_transfoTotailAnim; }
        ITF_INLINE f32              getMultiPlayerLifePointFactor() const { return m_multiPlayerLifePointFactor; }
        ITF_INLINE Ray_EventSpawnReward* getBubonReward()           const { return m_bubonReward; }
        
    private:
        
        StringID            m_leftBubonBoneName;
        StringID            m_rightBubonBoneName;
        f32                 m_bubonPhantomSize;
        Path                m_bubonRewardSpawnPath;
        Path                m_tailPath;
        u32                 m_rewardNumber;
        u32                 m_destroyRewardNumber;
        i32                 m_bubonHealth;
        EReceivedHitType	m_hitType;
        u32                 m_hitLevel;
        Faction             m_faction;
        StringID            m_transfoTotailAnim;
        f32                 m_multiPlayerLifePointFactor;
        Ray_EventSpawnReward* m_bubonReward;
    };

    ITF_INLINE const class Ray_ShooterGardianMorayBodyPart_Template * Ray_ShooterGardianMorayBodyPart::getTemplate() const
    {
        return static_cast<const class Ray_ShooterGardianMorayBodyPart_Template *>(m_template);
    }

    //-----------------------------------------------------------------------------------------------
    //                                      TAIL PART
    //-----------------------------------------------------------------------------------------------
    //class Ray_ShooterGardianMorayTail : public BodyPart
    //{
    //    DECLARE_OBJECT_CHILD_RTTI( Ray_ShooterGardianMorayTail, BodyPart ,1035446650)

    //public:
    //    Ray_ShooterGardianMorayTail();
    //    virtual ~Ray_ShooterGardianMorayTail();

    //    virtual void        initialize( Actor * _parent, Ray_SnakeAIComponent* _parentComponent );
    //    virtual void        update( f32 _dt );
    //    virtual void        onEvent( Event * _event );
    //    virtual Actor *     spawnBodyPart();
    //    virtual bbool       processHitStim( HitStim * _hitStim );

    //    ////virtual bbool       receiveHit( HitStim * _hitStim, bbool _leftSide );
    //    
    //    

    //private:   
    //    enum TailState
    //    {
    //        Closed = 0,
    //        Opened = 1,
    //        Hit = 2,
    //        ENUM_FORCE_SIZE_32( TailState )
    //    };

    //    void                changeState( TailState _state );
    //    void                onStateFinished();

    //    void                createPhantom( PhysPhantom * & _phatom, const PhysShape * _shape );
    //    void                destroyPhantom( PhysPhantom * & _phatom );
    //    void                disablePhatom(  PhysPhantom * & _phatom, bbool & _insertFlag );
    //    void                enablePhatom(  PhysPhantom * & _phatom, bbool & _insertFlag );

    //    PhysPhantom *       m_phantom;
    //    bbool               m_phantomInserted;
    //    u32                 m_phantomBoneIdx;

    //    TailState           m_curState;
    //    TailState           m_wantedState;
    //    bbool               m_checkWantedStateNextFrame;

    //    ITF_INLINE const class Ray_ShooterGardianMorayTail_Template * getTemplate() const;
    //};

    ////-------------------------------------------------------------------------------------
    //class Ray_ShooterGardianMorayTail_Template : public BodyPart_Template
    //{
    //    DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterGardianMorayTail_Template, BodyPart_Template ,2539198368);
    //    DECLARE_SERIALIZE();
    //    DECLARE_BODYPART_TEMPLATE(Ray_ShooterGardianMorayTail);

    //public:

    //    Ray_ShooterGardianMorayTail_Template();
    //    virtual ~Ray_ShooterGardianMorayTail_Template();

    //    ITF_INLINE const PhysShape*     getPhantomShape() const { return m_phantomShape; }
    //    ITF_INLINE const StringID &     getPhantomBoneName() const { return m_phantomBoneName; }

    //private:

    //    PhysShape*          m_phantomShape;
    //    StringID            m_phantomBoneName;

    //};

    //ITF_INLINE const class Ray_ShooterGardianMorayTail_Template * Ray_ShooterGardianMorayTail::getTemplate() const
    //{
    //    return static_cast<const class Ray_ShooterGardianMorayTail_Template *>(getBodyPartTemplate());
    //}

}

#endif //_ITF_RAY_SHOOTERGARDIANMORAY_H_

