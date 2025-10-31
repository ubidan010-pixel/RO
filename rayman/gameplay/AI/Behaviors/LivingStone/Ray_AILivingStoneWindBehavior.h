#ifndef _ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_
#define _ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

namespace ITF
{
    class WindStim;

    class Ray_AILivingStoneWindBehavior : public Ray_AIGroundBaseMovementBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILivingStoneWindBehavior, Ray_AIGroundBaseMovementBehavior,2183015968)

    public:

        Ray_AILivingStoneWindBehavior();
        virtual ~Ray_AILivingStoneWindBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onActionFinished();
                void                        setData(WindStim*_stim);
        virtual void                        onEvent(Event * _event);
    private:
        Vec2d m_windDir;
        ActorRef m_source;
        bbool m_justEntered;
        f32     m_hysteresis;
        f32 m_timer;
        bbool m_continue;
    };

    class Ray_AILivingStoneWindBehavior_Template : public Ray_AIGroundBaseMovementBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILivingStoneWindBehavior_Template,Ray_AIGroundBaseMovementBehavior_Template,3646410645);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AILivingStoneWindBehavior);

    public:

        Ray_AILivingStoneWindBehavior_Template() {}
        ~Ray_AILivingStoneWindBehavior_Template(){}

    private:

    };

}

#endif //_ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_