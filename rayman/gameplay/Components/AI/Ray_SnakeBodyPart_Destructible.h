#ifndef _ITF_RAY_SNAKEBODYPART_DESTRUCTIBLE_H_
#define _ITF_RAY_SNAKEBODYPART_DESTRUCTIBLE_H_

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_RAY_SNAKEBODYPART_MIXED_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Mixed.h"
#endif //_ITF_RAY_SNAKEBODYPART_MIXED_H_

namespace ITF
{
    class DestructibleBodyPart : public BodyPart
    {
        DECLARE_OBJECT_CHILD_RTTI(DestructibleBodyPart, BodyPart,81228598)

    public:

        DestructibleBodyPart();
        virtual ~DestructibleBodyPart();

        virtual void        onInit();
        virtual bbool       updateKeepAlive( f32 _dt );
        virtual bbool       removeAndKeepAliveOnDeath();        // called on destruction call to keep dead part updated
        virtual bbool       receiveHit( HitStim * _hitStim, bbool _leftSide );

        ITF_INLINE const Ray_EventSpawnReward * getReward() const           { return m_reward; }
        ITF_INLINE const class DestructibleBodyPart_Template* getTemplate() const;

    protected:

        virtual Actor*     spawnBodyPart();

    private:

        bbool       m_ejectInitialized;
        Vec3d       m_ejectSpeed;
        f32         m_ejectTimer;
        Ray_EventSpawnReward*   m_reward;
    };

    //-----------------------------------------------------------------
    class DestructibleBodyPart_Template : public BodyPart_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(DestructibleBodyPart_Template, BodyPart_Template,1660554312);
        DECLARE_BODYPART_TEMPLATE(DestructibleBodyPart);
        DECLARE_SERIALIZE();

    public:

        DestructibleBodyPart_Template();
        virtual ~DestructibleBodyPart_Template();

        ITF_INLINE bbool                               ejectOnDeath() const { return m_ejectOnDeath; }

        ITF_INLINE const Vec3d&                        getEjectMinStartSpeed()      const { return m_ejectMinStartSpeed; }
        ITF_INLINE const Vec3d&                        getEjectMaxStartSpeed()      const { return m_ejectMaxStartSpeed; }
        ITF_INLINE f32                                 getEjectZMinSpeed()          const { return m_ejectzMinSpeed; }
        ITF_INLINE f32                                 getEjectZAcceleration()      const { return m_ejectzAcceleration; }
        ITF_INLINE f32                                 getEjectGravityMultiplier()  const { return m_ejectGravityMultiplier; }
        ITF_INLINE f32                                 getEjectDuration()           const { return m_ejectDuration; }
        ITF_INLINE i32                                 getEjectZForced()            const { return m_ejectzForced; }
        ITF_INLINE f32                                 getEjectRotationSpeed()      const { return m_ejectRotationSpeed; }
        ITF_INLINE const Vec2d &                       getEjectFixedEjectDir()      const { return m_ejectFixedEjectDir; }
        ITF_INLINE f32                                 getEjectFadeDuration()       const { return m_ejectFadeDuration; }
        ITF_INLINE f32                                 getEjectDelayBeforeFade()    const { return m_ejectDelayBeforeFade; }
        ITF_INLINE const Ray_EventSpawnReward*         getReward()                  const { return m_reward; } 

        

    private:      
        bbool                   m_ejectOnDeath;

        Vec3d                   m_ejectMinStartSpeed;
        Vec3d                   m_ejectMaxStartSpeed;
        f32                     m_ejectzMinSpeed;
        f32                     m_ejectzAcceleration;
        f32                     m_ejectGravityMultiplier;
        f32                     m_ejectDuration;
        i32                     m_ejectzForced;
        f32                     m_ejectRotationSpeed;
        Vec2d                   m_ejectFixedEjectDir;
        f32                     m_ejectFadeDuration;
        f32                     m_ejectDelayBeforeFade;
        Ray_EventSpawnReward*   m_reward;
    };

    ITF_INLINE const DestructibleBodyPart_Template* DestructibleBodyPart::getTemplate() const { return static_cast<const DestructibleBodyPart_Template*>(m_template); }
};

#endif //_ITF_RAY_SNAKEBODYPART_DESTRUCTIBLE_H_
