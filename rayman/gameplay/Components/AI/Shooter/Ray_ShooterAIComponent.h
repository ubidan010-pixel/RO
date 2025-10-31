#ifndef _ITF_RAY_SHOOTERAICOMPONENT_H_
#define _ITF_RAY_SHOOTERAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AIDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIDeathBehavior.h"
#endif //_ITF_RAY_AIDEATHBEHAVIOR_H_

namespace ITF
{
    class Ray_ShooterAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterAIComponent, Ray_AIComponent,3358339901)

    public:
        DECLARE_SERIALIZE()

        Ray_ShooterAIComponent();
        virtual ~Ray_ShooterAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onCheckpointLoaded();
        virtual void        Update( f32 _deltaTime );
        virtual void        onBecomeInactive();
        virtual void        onBecomeActive();
        virtual void        onDepthChanged( f32 _oldZ, f32 _newZ );

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        virtual void        onEvent( class Event * _event);
        virtual void        onBehaviorFinished();
        virtual void        updateAnimInput();

    protected:
        void                processCrushed( EventCrushed* crushed );
        void                processInteractionQuery( EventInteractionQuery* interactionQuery );
        void                applyCamRelativeInitialPos();
        void                disableComponents();

    private:
        ITF_INLINE const class Ray_ShooterAIComponent_Template * getTemplate() const;
        void                                updateReceiveHit( f32 _deltaTime );
        void                                receivePunch( class PunchStim* _punch );

        AIBehavior *                        m_idleBehavior;
        Ray_AIReceiveHitBehavior *          m_receiveHitBehavior;
        AIBehavior  *                       m_deathBehavior;
        
        bbool                               m_swallowed;
        bbool                               m_underVacuumInfluence;
        bbool                               m_animInputUnderVacuumInfluence;
        Vec2d                               m_prevPosition;
        ActorRef                            m_lastHitSender;
        f32                                 m_lastHitSenderCoolDown;
        Color                               m_curFogColor;
        f32                                 m_curFogDepth;

        
        class TweenComponent *              m_tweenComponent;
        class Ray_StimComponent *           m_stimComponent;
        class Ray_ShooterActorParameterComponent * m_shooterAPComponent;

        bbool                               m_cameraRelativeTested;
        bbool                               m_isCameraRelative;
        bbool                               m_hasEnteredScreen;
        bbool                               m_useOptim;

        // saved params
        Vec3d                               m_beforeCamRelativeInitialPos;

        // constants
        static const f32                    k_SameHitSenderCoolDownDuration;

    };

    //-------------------------------------------------------------------------------------
    class Ray_ShooterAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterAIComponent_Template,Ray_AIComponent_Template,4046682542);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterAIComponent);

    public:

        Ray_ShooterAIComponent_Template();
        virtual ~Ray_ShooterAIComponent_Template();

        ITF_INLINE const class TemplateAIBehavior *                 getIdleBehavior() const { return m_idleBehavior; }
        ITF_INLINE const class Ray_AIReceiveHitBehavior_Template*   getReceiveHitBehavior() const { return m_receiveHitBehavior; }
        ITF_INLINE const class TemplateAIBehavior*                  getDeathBehavior() const { return m_deathBehavior; }

        ITF_INLINE bbool                                            isCameraRelative() const    { return m_cameraRelative; }
        ITF_INLINE bbool                                            isTriggerable() const       { return m_triggerable; }
        ITF_INLINE bbool                                            useDynamicFlip() const              { return m_dynamicFlip; }
        ITF_INLINE bbool                                            useDynamicFlipStartLeft() const     { return m_dynamicFlipStartLeft; }
        ITF_INLINE bbool                                            useFixedAngle() const       { return m_useFixedAngle; }
        ITF_INLINE const Angle &                                    getFixedAngle() const       { return m_fixedAngle; }
        ITF_INLINE const Angle &                                    getTakeMoveOrientationOffset() const    { return m_takeMoveOrientationOffset; }
        ITF_INLINE bbool                                            getTakeMoveOrientation() const          { return m_takeMoveOrientation; }
        ITF_INLINE bbool                                            useDynamicFog() const               { return m_useDynamicFog; }
        ITF_INLINE const Vec3d &                                    getDynamicFogDefaultColor() const   { return m_dynamicFogDefaultColor; }
        ITF_INLINE f32                                              getDynamicFogMaxDepth() const       { return m_dynamicFogMaxDepth; }
        ITF_INLINE bbool                                            getCanBeCrushed() const { return m_canBeCrushed; }
        ITF_INLINE bbool                                            getDisableOnBecomeInactive() const { return m_disableOnBecomeInactive; }
        ITF_INLINE f32                                              getVisualScaleMultiplier() const   { return m_visualScaleMultiplier; }
       

    private:
        class TemplateAIBehavior *                m_idleBehavior;
        class Ray_AIReceiveHitBehavior_Template*  m_receiveHitBehavior;
        class TemplateAIBehavior*                 m_deathBehavior;

        bbool   m_cameraRelative;
        bbool   m_triggerable;
        bbool   m_dynamicFlip;
        bbool   m_dynamicFlipStartLeft;
        bbool   m_useFixedAngle;
        Angle   m_fixedAngle;
        bbool   m_takeMoveOrientation;
        Angle   m_takeMoveOrientationOffset;
        bbool   m_useDynamicFog;
        Vec3d   m_dynamicFogDefaultColor;
        f32     m_dynamicFogMaxDepth;
        bbool   m_canBeCrushed;
        bbool   m_disableOnBecomeInactive;
        f32     m_visualScaleMultiplier;
    };
    ITF_INLINE const class Ray_ShooterAIComponent_Template * Ray_ShooterAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_ShooterAIComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_SHOOTERAICOMPONENT_H_

