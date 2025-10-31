#ifndef _ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_
#define _ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_

#ifndef _ITF_RAY_AIRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIReceiveHitAction.h"
#endif //_ITF_RAY_AIRECEIVEHITACTION_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

namespace ITF
{
    class Ray_AIReceiveCameraEjectHitAction : public Ray_AIReceiveHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIReceiveCameraEjectHitAction, Ray_AIReceiveHitAction,726520);

    public:

        Ray_AIReceiveCameraEjectHitAction();
        virtual ~Ray_AIReceiveCameraEjectHitAction();

        virtual void                        onActivate();
        virtual void                        update( f32 _dt );

    private:

        ITF_INLINE const class Ray_AIReceiveCameraEjectHitAction_Template* getTemplate() const;

        void                                setEjectOutHit();
        void                                updateEjectOutHit( f32 _dt );

        Vec3d                               m_ejectSpeed;
        f32                                 m_ejectTimer;
    };

    class Ray_AIReceiveCameraEjectHitAction_Template : public Ray_AIReceiveHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIReceiveCameraEjectHitAction_Template,Ray_AIReceiveHitAction_Template,383277539);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIReceiveCameraEjectHitAction);

    public:

        Ray_AIReceiveCameraEjectHitAction_Template();
        ~Ray_AIReceiveCameraEjectHitAction_Template() {}

        ITF_INLINE const Vec3d&                        getMinStartSpeed() const { return m_minStartSpeed; }
        ITF_INLINE const Vec3d&                        getMaxStartSpeed() const { return m_maxStartSpeed; }
        ITF_INLINE f32                                 getZMinSpeed() const { return m_zMinSpeed; }
        ITF_INLINE f32                                 getZAcceleration() const { return m_zAcceleration; }
        ITF_INLINE f32                                 getGravityMultiplier() const { return m_gravityMultiplier; }
        ITF_INLINE f32                                 getEjectDuration() const { return m_ejectDuration; }
        ITF_INLINE i32                                 getZForced() const { return m_zForced; }
        ITF_INLINE f32                                 getRotationSpeed() const { return m_rotationSpeed; }
        ITF_INLINE const Vec2d &                       getFixedEjectDir() const { return m_fixedEjectDir; }
        ITF_INLINE f32                                 getFadeDuration() const      { return m_fadeDuration; }
        ITF_INLINE f32                                 getDelayBeforeFade() const   { return m_delayBeforeFade; }

    private:

        Vec3d                               m_minStartSpeed;
        Vec3d                               m_maxStartSpeed;
        f32                                 m_zMinSpeed;
        f32                                 m_zAcceleration;
        f32                                 m_gravityMultiplier;
        f32                                 m_ejectDuration;
        i32                                 m_zForced;
        f32                                 m_rotationSpeed;
        Vec2d                               m_fixedEjectDir;
        f32                                 m_fadeDuration;
        f32                                 m_delayBeforeFade;
    };

    ITF_INLINE const class Ray_AIReceiveCameraEjectHitAction_Template* Ray_AIReceiveCameraEjectHitAction::getTemplate() const { return static_cast<const Ray_AIReceiveCameraEjectHitAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_