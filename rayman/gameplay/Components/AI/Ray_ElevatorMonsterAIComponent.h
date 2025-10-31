#ifndef _ITF_RAY_ELEVATORMONSTERAICOMPONENT_H_
#define _ITF_RAY_ELEVATORMONSTERAICOMPONENT_H_

namespace ITF
{
    class LinkComponent;

    class Ray_ElevatorMonsterAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElevatorMonsterAIComponent, ActorComponent,3963479976)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ElevatorMonsterAIComponent();
        virtual ~Ray_ElevatorMonsterAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );

    private:
        ITF_INLINE const class Ray_ElevatorMonsterAIComponent_Template* getTemplate() const;

        LinkComponent*      m_linkComponent;
        ActorRef            m_elevator;
        bbool               m_launchAttack;
        f32                 m_countDownAttack;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************
	
    class Ray_ElevatorMonsterAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElevatorMonsterAIComponent_Template, TemplateActorComponent,1241043754);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ElevatorMonsterAIComponent);

    public:

        Ray_ElevatorMonsterAIComponent_Template();
        virtual ~Ray_ElevatorMonsterAIComponent_Template();

        ITF_INLINE f32 getRatioSpeed() const { return m_ratioSpeed; }
        ITF_INLINE f32 getDistanceForHit() const { return m_distanceForHit; }
        ITF_INLINE f32 getCountDownAttack() const { return m_countDownAttack; }



    private:

        f32 m_ratioSpeed;
        f32 m_distanceForHit;
        f32 m_countDownAttack;
    };

    //*****************************************************************************

    ITF_INLINE const Ray_ElevatorMonsterAIComponent_Template* Ray_ElevatorMonsterAIComponent::getTemplate() const
    {
        return static_cast<const Ray_ElevatorMonsterAIComponent_Template*>(m_template);
    }
	
}; // end namespace

#endif //_ITF_RAY_ELEVATORMONSTERAICOMPONENT_H_