#ifndef _ITF_RAY_BOSSPLANTAICOMPONENT_H_
#define _ITF_RAY_BOSSPLANTAICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    class Ray_BossPlantNodeComponent;
    class Ray_StimComponent;

    class Ray_BossPlantAIComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossPlantAIComponent, AIComponent,203747753)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BossPlantAIComponent();
        virtual ~Ray_BossPlantAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onCheckpointLoaded();
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );

    private:

        ITF_INLINE const class Ray_BossPlantAIComponent_Template * getTemplate() const;

        void            clear();

        Ray_StimComponent*  m_stimComponent;

        ActorRef        m_initialNode;
        ActorRef        m_currentNode;
        ActorRef        m_nextNode;
        u32             m_cycleIndex;
        bbool           m_justStarted;
        i32             m_currentNetworkIndex;     // the boss can be linked to several networks, and when hit, it will select the next one

        f32             m_playRateMultiplier;

        void            processActivateEvent( Ray_EventSetInitialNode* _nodeEvent );
        void            setInitialNode( Ray_BossPlantNodeComponent* _node, bbool _doItNow );
        void            setCurrentNode( Ray_BossPlantNodeComponent* _node );
        void            startNode( Ray_BossPlantNodeComponent* _node );
        Ray_BossPlantNodeComponent* getNode( ActorRef _nodeActor );
        Ray_BossPlantNodeComponent* getNode( const ObjectPath& _objectPath );
#ifndef ITF_FINAL
        void            debugNodes();
#endif // ITF_FINAL


        void            shakeCamera( AnimGameplayEvent* _animEvent );
        void            cheatCycleNodes( Ray_BossPlantNodeComponent* _current,  Ray_BossPlantNodeComponent* _next );
    };

    //-------------------------------------------------------------------------------------
    class Ray_BossPlantAIComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossPlantAIComponent_Template,AIComponent_Template,1969325865);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossPlantAIComponent);

    public:

        Ray_BossPlantAIComponent_Template();
        ~Ray_BossPlantAIComponent_Template() {}

        ITF_INLINE const Vec2d & getTargetEvaluationOffset() const { return m_targetEvaluationOffset; }
        ITF_INLINE f32 getPlayRateMultiplierMinDistance() const { return m_playRateMultiplierMinDistance; }
        ITF_INLINE f32 getPlayRateMultiplierMaxDistance() const { return m_playRateMultiplierMaxDistance; }
        ITF_INLINE f32 getPlayRateMultiplierMinValue() const { return m_playRateMultiplierMinValue; }
        ITF_INLINE f32 getPlayRateMultiplierMaxValue() const { return m_playRateMultiplierMaxValue; }
        ITF_INLINE f32 getPlayRateMultiplierAcceleration() const { return m_playRateMultiplierAcceleration; }
        ITF_INLINE f32 getPlayRateMultiplierDeceleration() const { return m_playRateMultiplierDeceleration; }

    private:

        Vec2d           m_targetEvaluationOffset;   
        f32             m_playRateMultiplierMinDistance;
        f32             m_playRateMultiplierMaxDistance;
        f32             m_playRateMultiplierMinValue;
        f32             m_playRateMultiplierMaxValue;
        f32             m_playRateMultiplierAcceleration;
        f32             m_playRateMultiplierDeceleration;
    };

    ITF_INLINE const Ray_BossPlantAIComponent_Template * Ray_BossPlantAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BossPlantAIComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_BOSSPLANTAICOMPONENT_H_