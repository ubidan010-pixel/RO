#ifndef _ITF_RAY_AIFISHBEHAVIOR_H_
#define _ITF_RAY_AIFISHBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

namespace ITF
{
    class Ray_AIFishBehavior : public Ray_AIGroundBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFishBehavior, Ray_AIGroundBaseBehavior,591237529);

    public:

        Ray_AIFishBehavior();
        virtual ~Ray_AIFishBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActionFinished();
        virtual void                        onEvent(Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class Ray_AIFishBehavior_Template * getTemplate() const;

    private:

        void startStruggle();
        void startRelease();
        void startSpikeHit();

        void updateStruggle(f32 _dt);
        void updateRelease(f32 _dt);

        void onStruggleFinished();
        void onReleaseFinished();
        void onSpikeHitFinished();
    
        bbool checkContacts();

        AIAction *  m_struggle;
        AIAction * m_release;
        class Ray_AIReceiveCameraEjectHitAction * m_spikeHit;

        class Ray_SubAnchorComponent * m_subAnchorComponent;
        class PhantomComponent * m_phantomComponent;
        class TweenComponent * m_tweenComponent;

        Vec3d m_releasePos;
        Vec2d m_releaseDir;

        f32 m_releaseSpeed;

        Vec2d m_prevPos;
        bbool m_active;

        f32 m_timer;

        u32 m_dangerLevel;
        Vec2d m_hitDir;
        Vec2d m_hitPos;

    };

    class Ray_AIFishBehavior_Template : public Ray_AIGroundBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFishBehavior_Template,Ray_AIGroundBaseBehavior_Template,2455270684);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIFishBehavior);

    public:

        Ray_AIFishBehavior_Template();
        ~Ray_AIFishBehavior_Template();

        ITF_INLINE const AIAction_Template * getStruggle() const { return m_struggle; }
        ITF_INLINE const AIAction_Template * getRelease() const { return m_release; }
        ITF_INLINE const class Ray_AIReceiveCameraEjectHitAction_Template * getSpikeHit() const { return m_spikeHit; }

        ITF_INLINE f32 getReleaseSpeed() const { return m_releaseSpeed; }
        ITF_INLINE ITF::bbool getRotateOnRelease() const { return m_rotateOnRelease; }
        ITF_INLINE ITF::Angle getRotationSpeed() const { return m_rotationSpeed; }
        ITF_INLINE f32 getStruggleOffsetFrequency() const { return m_struggleOffsetFrequency; }
        ITF_INLINE f32 getStruggleOffsetAmplitude() const { return m_struggleOffsetAmplitude; }
        ITF_INLINE f32 getReleaseAccel() const { return m_releaseAccel; }
    private:

        AIAction_Template * m_struggle;
        AIAction_Template * m_release;
        class Ray_AIReceiveCameraEjectHitAction_Template * m_spikeHit;

        f32 m_releaseSpeed;
        f32 m_releaseAccel;
        bbool m_rotateOnRelease;
        Angle m_rotationSpeed;
        f32 m_struggleOffsetFrequency;
        f32 m_struggleOffsetAmplitude;
    };
    ITF_INLINE const Ray_AIFishBehavior_Template * Ray_AIFishBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIFishBehavior_Template *>(m_template);
    }

}

#endif //_ITF_Ray_AIFishBehavior_H_