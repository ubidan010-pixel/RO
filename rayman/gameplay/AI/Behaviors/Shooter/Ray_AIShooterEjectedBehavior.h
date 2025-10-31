#ifndef _ITF_RAY_AISHOOTEREJECTEDBEHAVIOR_H_
#define _ITF_RAY_AISHOOTEREJECTEDBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AIShooterEjectedBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterEjectedBehavior, AIBehavior ,2652583255)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIShooterEjectedBehavior();
        virtual ~Ray_AIShooterEjectedBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        //virtual void            onDeactivate();
        virtual void            update( f32 _delta  );
        virtual void            onActionFinished();
		virtual void            onEvent( Event * _event);

	protected:

        void        onHitSuccessful( EventHitSuccessful * _hitSuccessfulEvt );
        void        ejectActionFinished( bbool _fromAction );


		Vec2d		m_curEjectDir;
		Vec2d		m_curEjectForce;

        u32         m_hitCount;

		AIAction *	m_ejectAction;
        class Ray_AIShooterProjectileAction * m_ejectProjectileAction;



    private:
        ITF_INLINE const class Ray_AIShooterEjectedBehavior_Template * getTemplate() const;

    };

    class Ray_AIShooterEjectedBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterEjectedBehavior_Template,TemplateAIBehavior,1940359400);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIShooterEjectedBehavior);

    public:

        Ray_AIShooterEjectedBehavior_Template();
        ~Ray_AIShooterEjectedBehavior_Template();

		ITF_INLINE const AIAction_Template *	getEjectActionTemplate()		const { return m_ejectAction; }
        ITF_INLINE const u32	                getHitNumber()		            const { return m_hitNumber; }
		ITF_INLINE const bbool					needDestroyOnEjectActionsEnd()	const { return m_destroyOnEjectActionsEnd; }
        ITF_INLINE const bbool					needKillOnEnd()	                const { return m_killOnEnd; }
        ITF_INLINE const StringID &             getHitNumberNextBhvName()	    const { return m_hitNumberNextBhvName; }
        ITF_INLINE const StringID &             getEjectActionNextBhvName()	    const { return m_ejectActionNextBhvName; }
        //ITF_INLINE const Event *                getOnEjectActionEndEvent()	    const { return m_onEjectActionEndEvent; }


    private:
		AIAction_Template *	m_ejectAction;
		bbool				m_destroyOnEjectActionsEnd;
        bbool               m_killOnEnd;
        u32                 m_hitNumber;
        StringID            m_hitNumberNextBhvName;
        StringID            m_ejectActionNextBhvName;
        //Event*              m_onEjectActionEndEvent;

    };

    ITF_INLINE const class Ray_AIShooterEjectedBehavior_Template * Ray_AIShooterEjectedBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIShooterEjectedBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AISHOOTEREJECTEDBEHAVIOR_H_