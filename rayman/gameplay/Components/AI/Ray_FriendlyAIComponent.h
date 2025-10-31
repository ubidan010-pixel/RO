#ifndef _ITF_RAY_FRIENDLYAICOMPONENT_H_
#define _ITF_RAY_FRIENDLYAICOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

namespace ITF
{
    class Ray_AIFriendly_GoToTargetBehavior;
    class Ray_AIFriendly_GoToTargetBehavior_Template;
    class Ray_AIGroundFollowBehavior;
    class Ray_AIGroundFollowBehavior_Template;
    class Ray_AIRedWizardDanceBehavior;
    class Ray_AIRedWizardDanceBehavior_Template;

    class Ray_FriendlyAIComponent : public Ray_GroundAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FriendlyAIComponent, Ray_GroundAIComponent,3278201406)

    public:
        DECLARE_SERIALIZE();

        Ray_FriendlyAIComponent();
        virtual ~Ray_FriendlyAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event );
        virtual void        onBehaviorFinished();

    protected:
        virtual void        updateRoam( f32 _dt );

    private:
        ITF_INLINE const class Ray_FriendlyAIComponent_Template * getTemplate() const;

        AIBehavior*                         m_waitBehavior;
        AIBehavior*                         m_triggerBehavior;
        AIBehavior*                         m_exitBehavior;
        AIBehavior*                         m_pedestalBehavior;
        AIBehavior*                         m_jumpBehavior;
        Ray_AIFriendly_GoToTargetBehavior*  m_gotoBehavior;
        //AIBehavior*                         m_gotoBehavior;
        Ray_AIGroundFollowBehavior*         m_followBehavior;
        Ray_AIRedWizardDanceBehavior*       m_danceBehavior;

        void                receiveOrder( const Ray_EventAIOrder* _order );
        void                receiveDance( class Ray_EventDance* _eventDance );
        void                gotoOrder( Ray_EventAIOrder* _order );
        void                startOrder( Ray_EventAIOrder* _order );
        void                onFinishedOrder();

        void                startGoto( Event* _event );
        void                onFinishedGoto();

        void                startFollow( ObjectRef _target );
        void                onFinishedFollow();

        bbool               m_hasNewOrder;
        bbool               m_hasNewGoto;
        bbool               m_hasCurrentOrder;
        bbool               m_hasCurrentGoto;
        bbool               m_currentOrderReached;
        bbool               m_startDance;
        Ray_EventAIOrder    m_currentOrder;
        Ray_EventAIOrder    m_gotoOrder;
        AIBehavior*         m_currentOrderBehavior;



        void                startWait( Event* _event );
        void                onFinishedWait();

        void                startTrigger( Event* _event );
        void                onFinishedTrigger();

        void                startExit( Event* _event );
        void                onFinishedExit();

        void                startPedestal( Event* _event );
        void                onFinishedPedestal();

        void                startJump( Event* _event );
        void                onFinishedJump();
    };

    //-------------------------------------------------------------------------------------
    class Ray_FriendlyAIComponent_Template : public Ray_GroundAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_FriendlyAIComponent_Template,Ray_GroundAIComponent_Template,610976079);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_FriendlyAIComponent);

    public:

        Ray_FriendlyAIComponent_Template();
        ~Ray_FriendlyAIComponent_Template();

        ITF_INLINE const TemplateAIBehavior* getWaitBehavior() const { return m_waitBehavior; }
        ITF_INLINE const TemplateAIBehavior* getTriggerBehavior() const { return m_triggerBehavior; }
        ITF_INLINE const TemplateAIBehavior* getExitBehavior() const { return m_exitBehavior; }
        ITF_INLINE const TemplateAIBehavior* getPedestalBehavior() const { return m_pedestalBehavior; }
        ITF_INLINE const TemplateAIBehavior* getJumpBehavior() const { return m_jumpBehavior; }
        ITF_INLINE const Ray_AIFriendly_GoToTargetBehavior_Template* getGotoBehavior() const { return m_gotoBehavior; }
        //ITF_INLINE const TemplateAIBehavior* getGotoBehavior() const { return m_gotoBehavior; }
        ITF_INLINE const Ray_AIGroundFollowBehavior_Template* getFollowBehavior() const { return m_followBehavior; }
        ITF_INLINE const Ray_AIRedWizardDanceBehavior_Template* getDanceBehavior() const { return m_danceBehavior; }
        ITF_INLINE f32 getFollowDetectDistance() const { return m_followDetectDistance; }

    private:

        TemplateAIBehavior*                         m_waitBehavior;
        TemplateAIBehavior*                         m_triggerBehavior;
        TemplateAIBehavior*                         m_exitBehavior;
        TemplateAIBehavior*                         m_pedestalBehavior;
        TemplateAIBehavior*                         m_jumpBehavior;
        Ray_AIFriendly_GoToTargetBehavior_Template* m_gotoBehavior;
        //TemplateAIBehavior*                         m_gotoBehavior;
        Ray_AIGroundFollowBehavior_Template*        m_followBehavior;
        Ray_AIRedWizardDanceBehavior_Template*      m_danceBehavior;
        f32                 m_followDetectDistance;
    };

    ITF_INLINE const class Ray_FriendlyAIComponent_Template * Ray_FriendlyAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_FriendlyAIComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_FRIENDLYAICOMPONENT_H_