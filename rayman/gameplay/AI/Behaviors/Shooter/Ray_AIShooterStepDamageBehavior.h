#ifndef _ITF_RAY_AISHOOTERSTEPDAMAGEBEHAVIOR_H_
#define _ITF_RAY_AISHOOTERSTEPDAMAGEBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class StepDamage
    {
        DECLARE_SERIALIZE();

        StepDamage() : m_startHealth( 0 ) {}
        i32         m_startHealth;
        StringID    m_hitAnim;
        StringID    m_defaultAnim;
    };

    class Ray_AIShooterStepDamageBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterStepDamageBehavior, AIBehavior ,706240516)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIShooterStepDamageBehavior();
        virtual ~Ray_AIShooterStepDamageBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        //virtual void            onDeactivate();
        virtual void            update( f32 _delta  );
        //virtual void            onActionFinished();
		virtual void            onEvent( Event * _event);

	protected:

        void                    playAnimation( const StringID & _anim );
        bbool                   isAnimFinished() const;

        const StepDamage *      m_curStepDamage;
        StringID                m_curAnim;


    private:
        ITF_INLINE const class Ray_AIShooterStepDamageBehavior_Template * getTemplate() const;

    };

    class Ray_AIShooterStepDamageBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterStepDamageBehavior_Template,TemplateAIBehavior,2847399885);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIShooterStepDamageBehavior);

    public:

        Ray_AIShooterStepDamageBehavior_Template();
        ~Ray_AIShooterStepDamageBehavior_Template();
        
        typedef ITF_VECTOR <StepDamage>  StepDamageArray;
		const StepDamage *	    getStepDamageFromHealth( i32 _health )		const;

    private:
		StepDamageArray     m_stepDamageList;

    };

    ITF_INLINE const class Ray_AIShooterStepDamageBehavior_Template * Ray_AIShooterStepDamageBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIShooterStepDamageBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AISHOOTERSTEPDAMAGEBEHAVIOR_H_