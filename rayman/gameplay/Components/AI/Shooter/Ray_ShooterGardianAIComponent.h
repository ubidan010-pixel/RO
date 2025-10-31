#ifndef _ITF_RAY_SHOOTERGARDIANAICOMPONENT_H_
#define _ITF_RAY_SHOOTERGARDIANAICOMPONENT_H_

#ifndef _ITF_RAY_SIMPLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SimpleAIComponent.h"
#endif //_ITF_RAY_SIMPLEAICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class Ray_ShooterGardianAIComponent : public Ray_SimpleAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterGardianAIComponent, Ray_SimpleAIComponent ,66886853)

    public:
        DECLARE_SERIALIZE()

        Ray_ShooterGardianAIComponent();
        virtual ~Ray_ShooterGardianAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _deltaTime );
        virtual void        receiveHit( HitStim* _hit, bbool _damageDependsOnLevel = btrue, u32 _forcedDamage = 0 );

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        virtual void        onEvent( class Event * _event);
        virtual void        onBehaviorFinished();
        virtual void        updateAnimInput();

    protected:
        virtual void        processHit( HitStim* _hit );
        virtual void        startDeath();

        void        updateMultiPlayerHealth();

        f32         m_hitAddAnimCursor;
        f32         m_lastHitTimer;
        bbool       m_isInvulnerable;

        i32         m_defaultMaxHealthMulti;
        SpawneeGenerator m_deathRewardGenerator;
        f32         m_deathRewardTimer;
        f32         m_deathNextSpawnTime;
        f32         m_deathLastSpawnDelay;

        class Ray_AIShooterVacuumBehavior * m_vacuumBhv;


    private:
        ITF_INLINE const class Ray_ShooterGardianAIComponent_Template * getTemplate() const;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ShooterGardianAIComponent_Template : public Ray_SimpleAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterGardianAIComponent_Template,Ray_SimpleAIComponent_Template,2815141000);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterGardianAIComponent);

    public:

        Ray_ShooterGardianAIComponent_Template();
        virtual ~Ray_ShooterGardianAIComponent_Template();

        ITF_INLINE u32              getMinHitStunLevel()            const { return m_minHitStunLevel; }
        ITF_INLINE f32              getMultiPlayerLifePointFactor() const { return m_multiPlayerLifePointFactor; }
        ITF_INLINE const Path &     getDeathRewardSpawnPath()       const { return m_deathRewardSpawnPath; }
        ITF_INLINE u32              getDeathRewardNumber()          const { return m_deathRewardNumber; }
        ITF_INLINE f32              getDeathRewardSpawnDuration()   const { return m_deathRewardSpawnDuration; }
        ITF_INLINE const Vec2d &    getDeathRewardSpawnDist()       const { return m_deathRewardSpawnDist; }
        ITF_INLINE const Vec2d &    getDeathRewardSpawnAngle()      const { return m_deathRewardSpawnAngle; }
    
    private:

        u32     m_minHitStunLevel;
        f32     m_multiPlayerLifePointFactor;
        Path    m_deathRewardSpawnPath;
        u32     m_deathRewardNumber;
        f32     m_deathRewardSpawnDuration;
        Vec2d   m_deathRewardSpawnDist;
        Vec2d   m_deathRewardSpawnAngle;
    };

    ITF_INLINE const class Ray_ShooterGardianAIComponent_Template * Ray_ShooterGardianAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_ShooterGardianAIComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_SHOOTERGARDIANAICOMPONENT_H_

