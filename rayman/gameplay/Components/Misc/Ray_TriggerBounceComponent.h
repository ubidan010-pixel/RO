#ifndef _ITF_RAY_TRIGGERBOUNCECOMPONENT_H_
#define _ITF_RAY_TRIGGERBOUNCECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_TriggerBounceComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_TriggerBounceComponent, ActorComponent,4162442951)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_TriggerBounceComponent();
        virtual ~Ray_TriggerBounceComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onBecomeActive();

        virtual void                onBecomeInactive();

        virtual void                onDepthChanged( f32 _oldZ, f32 _newZ );
        virtual void                onEvent(Event * _event);
        virtual void                Update( f32 _deltaTime );
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE bbool            isBouncing() const { return m_bouncePlayed; }

    private:
        ITF_INLINE const class Ray_TriggerBounceComponent_Template * getTemplate() const;

        void                        setDisabled( bbool _val);
        void                        registerPhantom();
        void                        unregisterPhantom();

        void                        detectBounce();
        void                        processNewDetectedActor( ActorRef _ref );
        void                        sendBounce( Actor* _actor, const Vec2d& _dir, bbool _fromHit );
        void                        updateMemorized( f32 _dt );
        bbool                       isMemorized( ActorRef _ref ) const;
        bbool                       isAttached( ActorRef _ref ) const { return m_attachedActors.find(_ref) != U32_INVALID; }
        Vec2d                       getBounceDirection( Actor* _actor ) const;
        void                        processFruitAttachQuery( Ray_EventFruitAttachQuery* _attachQuery );
        void                        processFruitDetach( Ray_EventFruitDetach* _detachEvent );
        void                        processReceivePunch( PunchStim* _punch );
        Pickable*                   findBounceToObj( Actor* _receiver ) const;

        bbool                       sendBounceRegular( Actor* _receiver, const Vec2d& _dir );
        bbool                       sendBounceToObj( Actor* _receiver, Pickable* _obj );
        void registerObjects();
        void unregisterObjects();

        struct TriggeredMem
        {
            ActorRef        m_ref;
            f32             m_timer;
            bbool           m_wasHit;
        };

        class PhysPhantom*              m_phantom;
        FixedArray <TriggeredMem,10>    m_memorized;
        FixedArray <TriggeredMem,3>     m_punchHurtMemorized;
        class ShapeDetectorComponent*   m_shapeDetector;
        AnimLightComponent*             m_animComponent;
        LinkComponent*                  m_linkComponent;
        SafeArray <ActorRef>            m_attachedActors;
        SafeArray <ActorRef>            m_actorsInside;
        ObjectRef                       m_receivedHitAttacker;
        Vec2d                           m_receivedHitDir;
        bbool                           m_bouncePlayed;
        bbool                           m_disabled;
        bbool                           m_registered;

    };

    //---------------------------------------------------------------------------------------------------

    class Ray_TriggerBounceComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_TriggerBounceComponent_Template,TemplateActorComponent,557222769);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_TriggerBounceComponent);

    public:

        Ray_TriggerBounceComponent_Template();
        ~Ray_TriggerBounceComponent_Template();

        const PhysShape*                getPhantomShape() const { return m_phantomShape; }
        BounceType                      getBounceType() const { return m_bounceType; }
        u32                             getHurtLevel() const { return m_hurtLevel; }
        Faction                         getFaction() const { return m_faction; }
        f32                             getRetriggerDelay() const { return m_retriggerDelay; }
        const StringID&                 getIdleAnim() const { return m_idleAnim; }
        const StringID&                 getBounceAnim() const { return m_bounceAnim; }
        const Vec2d&                    getStimDirection() const { return m_stimDirection; }
        bbool                           getUseDirAsWorldAngle() const { return m_useDirAsWorldAngle; }
        bbool                           getIsFruitStick() const { return m_fruitSticks; }
        bbool                           getSendBounceOnHit() const { return m_sendBounceOnHit; }
        bbool                           getIsRadial() const { return m_isRadial; }
        bbool                           getIsRadialConstrained() const { return m_isRadialConstrained; }
        bbool                           getIsSendMoreThanOnce() const { return m_sendMoreThanOnce; }
        bbool                           getUseBounceToPickable() const { return m_useBounceToPickable; }
        bbool                           getUseAngleToDiscardBounceToPickable() const { return m_useAngleToDistardBounceToPickable; }
        f32                             getBounceMultiplier() const { return m_bounceMultiplier; }

        f32                             getBounceToActorHeight() const { return m_bounceToActorheight; }
        f32                             getBounceToActorHeight2() const { return m_bounceToActorheight2; }
        f32                             getBounceToActorSpeed() const { return m_bounceToActorSpeed; }
        const Vec2d&                    getBounceToActorTargetOffset() const { return m_bounceToActorTargetOffset; }
        const Angle&                    getDetectRange() const { return m_detectRange; }
        ITF_INLINE bbool getBlocksHits() const { return m_blocksHits; }
        ITF_INLINE u32 getIgnoreHitLevel() const { return m_ignoreHitLevel; }
		ITF_INLINE bbool getDisableAtBounce() const { return m_disableAtBounce; }
        ITF_INLINE bbool getRegisterToAIManager() const { return m_registerToAIManager; }
        ITF_INLINE bbool getPlayBounceAnimOnHit() const { return m_playBounceAnimOnHit; }

    private:

        class PhysShape*                m_phantomShape;
        BounceType                      m_bounceType;
        u32                             m_hurtLevel;
        Faction                         m_faction;
        f32                             m_retriggerDelay;
        f32                             m_bounceToActorheight;               // first point of bezier curve
        f32                             m_bounceToActorheight2;              // second point of bezier curve
        f32                             m_bounceToActorSpeed;                // time to reach the end
        f32                             m_bounceMultiplier;
        StringID                        m_idleAnim;
        StringID                        m_bounceAnim;
        Vec2d                           m_stimDirection;
        Vec2d                           m_bounceToActorTargetOffset;         // offset from the target point
        Angle                           m_detectRange;
        bbool                           m_useDirAsWorldAngle;
        bbool                           m_fruitSticks;
        bbool                           m_sendBounceOnHit;
        bbool                           m_isRadial;
        bbool                           m_isRadialConstrained;
        bbool                           m_sendMoreThanOnce;
        bbool                           m_useBounceToPickable;
        bbool                           m_useAngleToDistardBounceToPickable;
        bbool                           m_blocksHits;
        u32                             m_ignoreHitLevel;
		bbool                           m_disableAtBounce;
		bbool                           m_registerToAIManager;
        bbool                           m_playBounceAnimOnHit;
    };
    ITF_INLINE const class Ray_TriggerBounceComponent_Template * Ray_TriggerBounceComponent::getTemplate() const
    {
        return static_cast<const class Ray_TriggerBounceComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_TRIGGERBOUNCECOMPONENT_H__