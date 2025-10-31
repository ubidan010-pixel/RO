#ifndef _ITF_RAY_REDWIZARDAICOMPONENT_H_
#define _ITF_RAY_REDWIZARDAICOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

namespace ITF
{
    class Ray_AIGroundFollowBehavior;

    class Ray_RedWizardAIComponent : public Ray_GroundAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RedWizardAIComponent, Ray_GroundAIComponent,329209979)

    public:
        DECLARE_SERIALIZE();

        Ray_RedWizardAIComponent();
        virtual ~Ray_RedWizardAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );
        virtual void        onBehaviorFinished();

    protected:
        virtual void        updateBehaviors( f32 _dt );
        virtual void        updateRoam( f32 _dt );

    private:
        ITF_INLINE const class Ray_RedWizardAIComponent_Template * getTemplate() const;

        void                startFollow( ObjectRef _target );
        void                onFinishedFollow();

        Ray_AIGroundFollowBehavior*     m_followBehavior;
        SafeArray<AIBehavior*>          m_eventTriggeredBehaviors;
    };

    //-------------------------------------------------------------------------------------
    class Ray_RedWizardAIComponent_Template : public Ray_GroundAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RedWizardAIComponent_Template,Ray_GroundAIComponent_Template,147929559);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_RedWizardAIComponent);

    public:

        Ray_RedWizardAIComponent_Template();
        ~Ray_RedWizardAIComponent_Template();

        ITF_INLINE ITF::f32 getFollowDetectDistance() const { return m_followDetectDistance; }
        ITF_INLINE const class Ray_AIGroundFollowBehavior_Template * getFollowBehavior() const { return m_followBehavior; }
        ITF_INLINE const SafeArray<class TemplateAIBehavior*> & getEventTriggeredBehaviors() const { return m_eventTriggeredBehaviors; }

    private:

        class Ray_AIGroundFollowBehavior_Template *     m_followBehavior;
        SafeArray<class TemplateAIBehavior*>          m_eventTriggeredBehaviors;
        f32                 m_followDetectDistance;
    };
    ITF_INLINE const class Ray_RedWizardAIComponent_Template * Ray_RedWizardAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_RedWizardAIComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_REDWIZARDAICOMPONENT_H_