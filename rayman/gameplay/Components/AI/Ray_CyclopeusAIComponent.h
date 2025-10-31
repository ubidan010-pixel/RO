#ifndef _ITF_RAY_CYCLOPEUSAICOMPONENT_H_
#define _ITF_RAY_CYCLOPEUSAICOMPONENT_H_

#ifndef _ITF_RAY_SIMPLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SimpleAIComponent.h"
#endif //_ITF_RAY_SIMPLEAICOMPONENT_H_

namespace ITF
{

    class AISimplePlayAnimBehavior;

    class Ray_CyclopeusAIComponent : public Ray_SimpleAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CyclopeusAIComponent,Ray_SimpleAIComponent,1994508162)
        DECLARE_SERIALIZE();

    public:

        Ray_CyclopeusAIComponent();
        ~Ray_CyclopeusAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onBehaviorFinished();
        virtual void        onReceiveHitBehaviorFinished();

    protected:

        virtual void        onHurtBehaviorFinished();
        virtual void        processHit( HitStim* _hit );


    private:
                void        UpdatePupil( f32 _dt);

    protected:

        ITF_INLINE const class Ray_CyclopeusAIComponent_Template * getTemplate() const;

    private:

        AISimplePlayAnimBehavior*   m_hurtBehavior;

        u32             m_hitCount;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

    class AISimplePlayAnimBehavior_Template;

    class Ray_CyclopeusAIComponent_Template : public Ray_SimpleAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CyclopeusAIComponent_Template,Ray_SimpleAIComponent_Template,3214622220);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CyclopeusAIComponent);

    public:

        Ray_CyclopeusAIComponent_Template();
        ~Ray_CyclopeusAIComponent_Template();

        ITF_INLINE const AISimplePlayAnimBehavior_Template* getHurtBehavior() const        { return m_hurtBehavior; }
        ITF_INLINE u32 getHitMax() const                                                   { return m_hitMax; }

    private:
        
        AISimplePlayAnimBehavior_Template*   m_hurtBehavior;

        u32 m_hitMax;

    };

    ITF_INLINE const class Ray_CyclopeusAIComponent_Template * Ray_CyclopeusAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_CyclopeusAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_CYCLOPEUSAICOMPONENT_H_

