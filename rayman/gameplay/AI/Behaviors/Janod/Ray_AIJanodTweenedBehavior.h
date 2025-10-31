#ifndef _ITF_RAY_AIJANODTWEENEDBEHAVIOR_H_
#define _ITF_RAY_AIJANODTWEENEDBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Fruit/Ray_AIFruitRoamingBehavior.h"
#endif //_ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIBOUNCEUTILITY_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIBounceUtility.h"
#endif

#ifndef __RAY_AIJANODROAMINGBASEBEHAVIOR_H__
#include "rayman/gameplay/ai/Behaviors/Janod/Ray_AIJanodRoamingBaseBehavior.h"
#endif //__RAY_AIJANODROAMINGBASEBEHAVIOR_H__

namespace ITF
{
    class StickToPolylinePhysComponent;
    class AIPlayAnimAction;

    class Ray_AIJanodTweenedBehavior : public Ray_AIJanodRoamingBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIJanodTweenedBehavior, Ray_AIJanodRoamingBaseBehavior,3808521499);
        //DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIJanodTweenedBehavior();
        virtual ~Ray_AIJanodTweenedBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActionFinished();

    private:
        ITF_INLINE const class Ray_AIJanodTweenedBehavior_Template * getTemplate() const;

        enum State_t 
        {
            State_NA=0,
            State_Tweened,
            State_Other,
            State_ReactingToTinyCrush,
            State_Bristling, 
            State_Count
        };

        void onEvent(Event * _event);
        void onActivate();
        void onDeactivate();
        bbool checkIfSwitchToBristle();
        void setState(State_t _state, const void *_params=NULL);
        void resetState();
        void updateTweened();        
        void enableTweening(bbool _enabled);

        State_t m_state;
        f32 m_timeInCurrentState;
        bbool m_signaledTinyCrush;
        StickToPolylinePhysComponent *m_physComponent;
    };

    class Ray_AIJanodTweenedBehavior_Template : public Ray_AIJanodRoamingBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIJanodTweenedBehavior_Template,Ray_AIJanodRoamingBaseBehavior_Template,64776580);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIJanodTweenedBehavior);

    public:

        Ray_AIJanodTweenedBehavior_Template();
        ~Ray_AIJanodTweenedBehavior_Template() {}

        f32                 getStimFeedback() const { return m_stimFeedback; }
        f32                 getStimFeedbackDist() const { return m_stimFeedbackDist; }


    private:

        f32 m_stimFeedback, m_stimFeedbackDist;
    };

    ITF_INLINE const class Ray_AIJanodTweenedBehavior_Template * Ray_AIJanodTweenedBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIJanodTweenedBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIJANODTWEENEDBEHAVIOR_H_

