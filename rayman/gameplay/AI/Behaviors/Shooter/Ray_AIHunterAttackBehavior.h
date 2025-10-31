#ifndef _ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_
#define _ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIIdleAction;
    class Ray_AIHunterLaunchBulletAction;

    class Ray_AIHunterAttackBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIHunterAttackBehavior, AIBehavior,3090799886)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIHunterAttackBehavior();
        virtual ~Ray_AIHunterAttackBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual void            update( f32 _delta  );
        virtual void            onEvent( Event* _event );

        virtual void            onActionFinished();

        virtual void            updateAnimInputs();

        void                    setVictim( ObjectRef _victim ) { m_victim = _victim; }
        ObjectRef               getVictim() const { return m_victim; }
        void                    setData(u32 _numBulletsPerCycle,u32 _numCycles);

#ifdef ITF_SUPPORT_EDITOR
        void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_AIHunterAttackBehavior_Template * getTemplate() const;

        AIAction*                           m_idle;
        AIAction*                           m_aim;
        Ray_AIHunterLaunchBulletAction*     m_hit;
        AIAction *                          m_giveUp;

        bbool                   m_linkedToActivator;
        ObjectRef               m_victim;

        void                    aim();
        void                    aimWithAnimation( const Vec2d& _direction, bbool _flip );
        f32                     getAngleInActorSpace( const Vec2d& _direction, bbool _flip, f32& min, f32& max, f32& actorAngle );
        void updateGiveUp( f32 _delta );
        void updateIdle(f32 _delta);

        Actor * getVictim();
        void updateAim(f32 _delta);

        bbool updateVictim();
        void updateHit( f32 _delta );
        
        void startGiveUp();
        void startIdle();
        void startHit();
        void startAim();
        void onFinishedHit();
        void onFinishedGiveUp();
        void onFinishedIdle();
        void onFinishedAim();

        u32 getCurrentLivingMissiles() const ;

        // 3 modes: rotate whole actor, rotate specific bone (pivot), or use a rotation animation
        f32                     m_aimCursor;
        f32                     m_aimCursorSpeed;

        u32                     m_numBulletsPerCycle;
        u32                     m_currentBullet;
        f32                     m_bulletTimer;
        f32                     m_cycleTimer;
        u32                     m_numTotalCycles;
        u32                     m_currentCycle;

        bbool                   m_enabled;
        ActorRefList            m_bulletsLaunched;

    };

    class Ray_AIHunterAttackBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIHunterAttackBehavior_Template,TemplateAIBehavior,117685005);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIHunterAttackBehavior);

    public:

        Ray_AIHunterAttackBehavior_Template();
        ~Ray_AIHunterAttackBehavior_Template();

        ITF_INLINE const class AIAction_Template*                          getAim() const { return m_aim; }
        ITF_INLINE const class AIAction_Template*                          getIdle() const { return m_idle; }
        ITF_INLINE const class Ray_AIHunterLaunchBulletAction_Template*    getHit() const { return m_hit; }
        ITF_INLINE const class AIAction_Template*                          getGiveUp() const { return m_giveUp; }
        ITF_INLINE f32                                                     getAttackDistance() const { return m_attackDistance; }
        ITF_INLINE const Angle&                                            getMinAngle() const { return m_minAngle; }
        ITF_INLINE const Angle&                                            getMaxAngle() const { return m_maxAngle; }
        ITF_INLINE bbool                                                   getLookRight() const { return m_lookRight; }
        ITF_INLINE f32                                                     getGiveUpRange() const { return m_giveUpRange; }
        ITF_INLINE f32                                                     getAimCursorRestingPos() const { return m_aimCursorRestingPos; }
        ITF_INLINE f32                                                     getAimTimeToFade() const { return m_aimTimeToFade; }
        ITF_INLINE f32                                                     getAimOffset() const { return m_aimOffset; }
        ITF_INLINE const Angle&                                            getForcedAngle() const { return m_forcedAngle; }
        ITF_INLINE bbool                                                   getIsForcedAngle() const { return m_forceAngle; }
        ITF_INLINE u32                                                     getNumBulletsPerCycle() const { return m_numBulletsPerCycle; }
        ITF_INLINE f32                                                     getTimeBetweenBullets() const { return m_timeBetweenBullets; }
        ITF_INLINE f32                                                     getTimeBetweenCycles() const { return m_timeBetweenCycles; }
        ITF_INLINE bbool                                                   getCanFlip() const { return m_canFlip; }
        ITF_INLINE bbool                                                   getDisableAttack() const { return m_DEBUG_disableAttack; }
        ITF_INLINE f32                                                     getFindEnemyRadius() const {return m_findEnemyRadius;}
        ITF_INLINE const AABB&                                             getDetectionRange() const {return m_detectionRange;}
        ITF_INLINE bbool                                                   getUseRadius() const {return m_useRadius;}
        ITF_INLINE Angle getMinArmAngle() const { return m_minArmAngle; }
        ITF_INLINE Angle getMaxArmAngle() const { return m_maxArmAngle; }
        ITF_INLINE u32 getMaxLivingMissiles() const { return m_maxLivingMissiles; }

    private:

        AIAction_Template*                              m_idle;
        AIAction_Template*                              m_aim;
        Ray_AIHunterLaunchBulletAction_Template*        m_hit;
        AIAction_Template*                              m_giveUp;
        f32                                             m_attackDistance;
        Angle                                           m_minAngle;
        Angle                                           m_maxAngle;
        bbool                                           m_lookRight;
        f32                                             m_giveUpRange;
        f32                                             m_aimCursorRestingPos;
        f32                                             m_aimTimeToFade;
        f32                                             m_aimOffset;
        Angle                                           m_forcedAngle;
        bbool                                           m_forceAngle;
        u32                                             m_numBulletsPerCycle;
        f32                                             m_timeBetweenBullets;
        f32                                             m_timeBetweenCycles;
        bbool                                           m_canFlip;
        f32                                             m_findEnemyRadius;
        AABB                                            m_detectionRange;
        bbool                                           m_useRadius;
        Angle                                             m_minArmAngle;
        Angle                                             m_maxArmAngle;
        // HACK
        bbool                                           m_DEBUG_disableAttack;
        u32                                             m_maxLivingMissiles;
    };

    ITF_INLINE const class Ray_AIHunterAttackBehavior_Template * Ray_AIHunterAttackBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIHunterAttackBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_