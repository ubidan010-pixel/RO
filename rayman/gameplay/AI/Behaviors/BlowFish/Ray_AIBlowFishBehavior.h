#ifndef _ITF_RAY_AIBLOWFISHBEHAVIOR_H_
#define _ITF_RAY_AIBLOWFISHBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

namespace ITF
{
    class Ray_AIBlowFishBehavior : public Ray_AIGroundBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBlowFishBehavior, Ray_AIGroundBaseBehavior,710365602);

    public:

        Ray_AIBlowFishBehavior();
        virtual ~Ray_AIBlowFishBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActionFinished();
        virtual void                        onEvent(Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR
        virtual void                        updateAnimInputs();

    protected:
        ITF_INLINE const class Ray_AIBlowFishBehavior_Template * getTemplate() const;

    private:



        void startIdle();
        void startDetect();
        void startInflate();
        void startInflatedIdle();
        void startDeflate();

        void updateIdle();
        void updateDetect();
        void updateInflate(f32 _dt);

        void updateStim(f32 _dt);

        void updateInflatedIdle(f32 _dt);
        void updateDeflate(f32 _dt);


        void onIdleFinished();
        void onDetectFinished();
        void onInflateFinished();
        void onInflatedIdleFinished();
        void onDeflateFinished();

        void updateScale();
        void updatePosition(f32 _dt);
        bbool isPlayerDetected(bbool _closeDetect = bfalse) const ;




        AIAction *  m_idle;
        AIAction *  m_detect;
        AIAction *  m_inflatedIdle;
        AIAction * m_inflate;
        AIAction * m_deflate;

        class PhantomComponent * m_phantomComponent;
        bbool m_active;
        f32 m_timer;
        bbool m_previouslyDetected;
        f32 m_scaleCursor;
        f32 m_phantomInitialScale;
        Vec2d m_currentSpeed;
        class TweenComponent * m_tweenComponent;

        ITF_MAP<ObjectRef,f32> m_memorizedHits;

    };

    class Ray_AIBlowFishBehavior_Template : public Ray_AIGroundBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBlowFishBehavior_Template,Ray_AIGroundBaseBehavior_Template,3094051622);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIBlowFishBehavior);

    public:

        Ray_AIBlowFishBehavior_Template();
        ~Ray_AIBlowFishBehavior_Template();

        ITF_INLINE const AIAction_Template * getIdle() const { return m_idle; }
        ITF_INLINE const AIAction_Template * getDetect() const { return m_detect; }
        ITF_INLINE const AIAction_Template * getInflatedIdle() const { return m_inflatedIdle; }
        ITF_INLINE const AIAction_Template * getInflate() const { return m_inflate; }
        ITF_INLINE const AIAction_Template * getDeflate() const { return m_deflate; }
		ITF_INLINE f32 getDetectionRange() const { return m_detectionRange; }
		ITF_INLINE f32 getDetectionCloseRange() const { return m_detectionCloseRange; }
        ITF_INLINE f32 getInflateDuration() const { return m_inflateDuration; }
        ITF_INLINE f32 getInflatedScaleMultiplier() const { return m_inflatedScaleMultiplier; }
        ITF_INLINE f32 getDeflateDuration() const { return m_deflateDuration; }
        ITF_INLINE f32 getMinIdleDuration() const { return m_minIdleDuration; }
        ITF_INLINE f32 getMinInflatedDuration() const { return m_minInflatedDuration; }
        ITF_INLINE f32 getRepulsionForce() const { return m_repulsionForce; }
        ITF_INLINE f32 getAttractionForce() const { return m_attractionForce; }
        ITF_INLINE f32 getFriction() const { return m_friction; }
        ITF_INLINE f32 getMemorizedHitTime() const { return m_memorizedHitTime; }
    private:

        AIAction_Template * m_idle;
        AIAction_Template * m_detect;
        AIAction_Template * m_inflatedIdle;
        AIAction_Template * m_inflate;
        AIAction_Template * m_deflate;
        f32 m_detectionRange;
		f32 m_detectionCloseRange;
        f32 m_inflateDuration;
        f32 m_inflatedScaleMultiplier;
        f32 m_deflateDuration;
        f32 m_minIdleDuration;
        f32 m_minInflatedDuration;
        f32 m_repulsionForce;
        f32 m_attractionForce;
        f32 m_friction;
        f32 m_memorizedHitTime;
    };
    ITF_INLINE const Ray_AIBlowFishBehavior_Template * Ray_AIBlowFishBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIBlowFishBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIBLOWFISHBEHAVIOR_H_