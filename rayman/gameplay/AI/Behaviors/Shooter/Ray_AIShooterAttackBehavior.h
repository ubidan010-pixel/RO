#ifndef _ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_
#define _ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_TIMEDSPAWNERCOMPONENT_H_
#include "gameplay/Components/Common/TimedSpawnerComponent.h"
#endif //_ITF_TIMEDSPAWNERCOMPONENT_H_

namespace ITF
{
    class AIIdleAction;
    class Ray_AIShooterLaunchBulletAction;

    class Ray_AIShooterAttackBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterAttackBehavior, AIBehavior,2271744321)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIShooterAttackBehavior();
        virtual ~Ray_AIShooterAttackBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual void            update( f32 _delta  );
        virtual void            onActionFinished();
        virtual void            updateAnimInputs();
        virtual void            onResourceReady();
        virtual void            onEvent( Event* _event);

        void                    setVictim( ObjectRef _victim ) { m_victim = _victim; }
        ObjectRef               getVictim() const { return m_victim; }
        void                    setTimedSpawnerData( const class TimedSpawnerData* _data )  { m_timedSpawnerData = _data; }
        void                    setFixedAngle( f32 _fixedAngle )                            { m_fixedAngle.SetDegrees( _fixedAngle ); }

    private:

        ITF_INLINE const class Ray_AIShooterAttackBehavior_Template * getTemplate() const;

        AIIdleAction*                       m_idle;
        Ray_AIShooterLaunchBulletAction*    m_hit;
        AIAction *                          m_giveUp;

        ObjectRef               m_victim;

        void                    aim();
        void                    aimWithBone( Vec2d& _direction, bbool _flip );
        void                    aimWithActor( Vec2d& _direction, bbool _flip );
        void                    aimWithAnimation( Vec2d& _direction, bbool _flip );
        f32                     getAngleInActorSpace( Vec2d& _direction, bbool _flip, f32& min, f32& max, f32& actorAngle );
        void                    getAimDir( Vec2d & _res_dir );
        void                    getCanonDir( Vec2d & _res_dir );

        // 3 modes: rotate whole actor, rotate specific bone (pivot), or use a rotation animation
        u32                     m_gunPivotBoneIndex;
        f32                     m_aimCursor;
        f32                     m_aimCursorSpeed;
        Vec2d                   m_keepAimDir;
        bbool                   m_KeepAimDirActive;

        u32                     m_canonPivotBoneIndex;
        u32                     m_canonExtremityBoneIndex;
        const class TimedSpawnerData* m_timedSpawnerData;
        TimedSpawnerData        m_timedSpawnerDataFromEvent;
        TimedSpawner            m_timedSpawner;
        Angle                   m_fixedAngle;

    };

    class Ray_AIShooterAttackBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterAttackBehavior_Template,TemplateAIBehavior,2518636371);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIShooterAttackBehavior);

    public:

        Ray_AIShooterAttackBehavior_Template();
        ~Ray_AIShooterAttackBehavior_Template();

        const class AIIdleAction_Template*                      getIdle() const { return m_idle; }
        const class Ray_AIShooterLaunchBulletAction_Template*   getHit() const { return m_hit; }
        const class AIAction_Template*                          getGiveUp() const { return m_giveUp; }
        bbool                                                   getRotateUsingAnimation() const { return m_rotateUsingAnimation; }
        f32                                                     getAttackDistance() const { return m_attackDistance; }
        const Angle&                                            getMinAngle() const { return m_minAngle; }
        const Angle&                                            getMaxAngle() const { return m_maxAngle; }
        const bbool&                                            isMinMaxAngleFlip() const { return m_minMaxAngleFlip; }
        bbool                                                   getLookRight() const { return m_lookRight; }
        f32                                                     getGiveUpRange() const { return m_giveUpRange; }
        f32                                                     getAimCursorRestingPos() const { return m_aimCursorRestingPos; }
        f32                                                     getAimTimeToFade() const { return m_aimTimeToFade; }
        f32                                                     getAimOffset() const { return m_aimOffset; }
        const Angle&                                            getForcedAngle() const { return m_forcedAngle; }
        bbool                                                   getIsForcedAngle() const { return m_forceAngle; }
        bbool                                                   getDisableAttack() const { return m_DEBUG_disableAttack; }
        ITF_INLINE bbool                                        getKeepDirForBurst() const { return m_keepDirForBurst; }
        ITF_INLINE bbool                                        getAutoForceDir() const { return m_autoForceDir; }

        const StringID&                                         getGunPivotBoneName() const { return m_gunPivotBoneName; }
        const StringID&                                         getCanonPivotBoneName() const { return m_canonPivotBoneName; }
        const StringID&                                         getCanonExtremityBoneName() const { return m_canonExtremityBoneName; }

    private:

        AIIdleAction_Template*                          m_idle;
        Ray_AIShooterLaunchBulletAction_Template*       m_hit;
        AIAction_Template*                              m_giveUp;
        StringID                                        m_gunPivotBoneName;
        bbool                                           m_rotateUsingAnimation;
        f32                                             m_attackDistance;
        Angle                                           m_minAngle;
        Angle                                           m_maxAngle;
        bbool                                           m_minMaxAngleFlip;
        bbool                                           m_lookRight;
        f32                                             m_giveUpRange;
        f32                                             m_aimCursorRestingPos;
        f32                                             m_aimTimeToFade;
        f32                                             m_aimOffset;
        Angle                                           m_forcedAngle;
        bbool                                           m_forceAngle;
        bbool                                           m_autoForceDir;
        
        bbool                                           m_keepDirForBurst;
        StringID                                        m_canonPivotBoneName;
        StringID                                        m_canonExtremityBoneName;

        // HACK
        bbool                                           m_DEBUG_disableAttack;
    };

    ITF_INLINE const class Ray_AIShooterAttackBehavior_Template * Ray_AIShooterAttackBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIShooterAttackBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_