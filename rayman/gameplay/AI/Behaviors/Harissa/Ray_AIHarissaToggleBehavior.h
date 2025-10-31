#ifndef _RAY_AI_HARISSATOGGLEBEHAVIOR_H_
#define _RAY_AI_HARISSATOGGLEBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    //forward
    class Ray_AIHarissaToggleBehavior_Template;
    
    /**********************************************************************/
    //  Ray_AIHarissaToggleBehavior :
    //
    //      Make harissa tube toggle between its two actions with sticked or punch events
    /**********************************************************************/

    class Ray_AIHarissaToggleBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIHarissaToggleBehavior, AIBehavior,2950040180);
        DECLARE_VALIDATE_BEHAVIOR();

    public:
        Ray_AIHarissaToggleBehavior();
        virtual ~Ray_AIHarissaToggleBehavior();

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onActivate();
        virtual void onDeactivate();
        virtual void onEvent(Event * _event);
        virtual void update(f32 _dt);
        virtual void onActorBecomeInactive();

        ITF_INLINE const Ray_AIHarissaToggleBehavior_Template * getTemplate() const;

    private:
        void startOffAction();
        void startOnAction();
        void startHitAction();
        void updateOffAction();
        void updateOnAction(const f32 _dt);

        void startFlameFX();
        void stopFlameFX();

    private:
        AIAction * m_offAction;
        AIAction * m_onAction;
        AIAction * m_hitAction;

        u32 m_flameHandle;
        u32 m_stickedActorsCount;
        f32 m_forcedTime;

        enum OnActionStep
        {
            OnActionStep_UNDEFINED = -1,
            OnActionStep_IN = 0,
            OnActionStep_LOOP,
            OnActionStep_OUT,
        };
        OnActionStep m_onActionStep;
    };




    /**********************************************************************/
    //  template class
    /**********************************************************************/

    class Ray_AIHarissaToggleBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIHarissaToggleBehavior_Template,TemplateAIBehavior,101396024);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIHarissaToggleBehavior);

    public:
        Ray_AIHarissaToggleBehavior_Template();
        ~Ray_AIHarissaToggleBehavior_Template();

        ITF_INLINE const AIAction_Template * getOffAction() const;
        ITF_INLINE const AIAction_Template * getOnAction() const;
        ITF_INLINE const AIAction_Template * getHitAction() const;
        ITF_INLINE const StringID & getFlameFXName() const;
        ITF_INLINE const StringID & getSquashedInAnim() const;
        ITF_INLINE const StringID & getSquashedLoopAnim() const;
        ITF_INLINE const StringID & getSquashedOutAnim() const;
        ITF_INLINE const StringID & getPolylineName() const;
        ITF_INLINE f32 getWeightThreshold() const;
        ITF_INLINE f32 getFlamesDurationOnCrushAttack() const;
		ITF_INLINE f32 getHitActionDuration() const;

    private:
        AIAction_Template * m_offAction;
        AIAction_Template * m_onAction;
        AIAction_Template * m_hitAction;
        StringID m_flameFXName;
        StringID m_squashedInAnim;
        StringID m_squashedLoopAnim;
        StringID m_squashedOutAnim;
        StringID m_polylineName;
        f32 m_weightThreshold;
        f32 m_flamesDurationOnCrushAttack;
		f32 m_hitActionDuration;
    };




    /**********************************************************************/
    //  inlined functions definition
    /**********************************************************************/

    ITF_INLINE const Ray_AIHarissaToggleBehavior_Template * Ray_AIHarissaToggleBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIHarissaToggleBehavior_Template *>( m_template );
    }

    ITF_INLINE const AIAction_Template * Ray_AIHarissaToggleBehavior_Template::getOffAction() const
    {
        return m_offAction;
    }

    ITF_INLINE const AIAction_Template * Ray_AIHarissaToggleBehavior_Template::getOnAction() const
    {
        return m_onAction;
    }

    ITF_INLINE const AIAction_Template * Ray_AIHarissaToggleBehavior_Template::getHitAction() const
    {
        return m_hitAction;
    }

    ITF_INLINE const StringID & Ray_AIHarissaToggleBehavior_Template::getFlameFXName() const
    {
        return m_flameFXName;
    }

    ITF_INLINE const StringID & Ray_AIHarissaToggleBehavior_Template::getSquashedInAnim() const
    {
        return m_squashedInAnim;
    }

    ITF_INLINE const StringID & Ray_AIHarissaToggleBehavior_Template::getSquashedLoopAnim() const
    {
        return m_squashedLoopAnim;
    }

    ITF_INLINE const StringID & Ray_AIHarissaToggleBehavior_Template::getSquashedOutAnim() const
    {
        return m_squashedOutAnim;
    }

    ITF_INLINE const StringID & Ray_AIHarissaToggleBehavior_Template::getPolylineName() const
    {
        return m_polylineName;
    }

    ITF_INLINE f32 Ray_AIHarissaToggleBehavior_Template::getWeightThreshold() const
    {
        return m_weightThreshold;
    }

    ITF_INLINE f32 Ray_AIHarissaToggleBehavior_Template::getFlamesDurationOnCrushAttack() const
    {
        return m_flamesDurationOnCrushAttack;
    }

    ITF_INLINE f32 Ray_AIHarissaToggleBehavior_Template::getHitActionDuration() const
    {
        return m_hitActionDuration;
	}
}

#endif //_RAY_AI_HARISSATOGGLEBEHAVIOR_H_
