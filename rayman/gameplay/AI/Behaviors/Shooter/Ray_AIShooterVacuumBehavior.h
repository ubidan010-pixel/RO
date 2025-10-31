#ifndef _ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_
#define _ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_SHOOTERVACUUMSKILL_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterVacuumSkill.h"
#endif //_ITF_RAY_SHOOTERVACUUMSKILL_H_

namespace ITF
{
    class Ray_AIShooterVacuumBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterVacuumBehavior, AIBehavior ,1724246635)
    public:

        Ray_AIShooterVacuumBehavior();
        virtual ~Ray_AIShooterVacuumBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        virtual void            onDeactivate();
        virtual void            update( f32 _dt );
        virtual void            onResourceReady();
        
        virtual void            onActionFinished();
		virtual void            onEvent( Event * _event);
        virtual void            updateAnimInputs();
        
#ifdef ITF_SUPPORT_EDITOR
        virtual void            drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

        void                    processEvtChangeState( Ray_EventShooterVacuumBhvChangeState * _evt );

	protected:
        
        enum VACUUM_BHV_STATES
        {
            WAIT = 0,
            VACUUM = 1,
            SPIT = 2,
            VACUUM_BHV_STATES_COUNT,
            ENUM_FORCE_SIZE_32(VACUUM_BHV_STATES)
        };

        void                    startVacuum();
        void                    stopVacuum();           // stop and finish it directly 
        void                    stopVacuumRequest();    // stop ans continue last aspirate actor transition
        void                    startWaitAction();
        void                    startVacuumAction();
        void                    startSpitAction();
        void                    spit();
        
        void                    processChangeStateRequest();

        VacuumSkill             m_vacuumSkill;
        bbool                   m_startVacuumRequested;
        bbool                   m_resourceReady;
        f32                     m_startVacuumTimer;
        bbool                   m_stopVacuumRequested;

        u32                     m_externStateRequested;
        bbool                   m_canReceiveHit;

        AIAction *	            m_waitAction;
        AIAction *	            m_vacuumAction;
        AIAction *	            m_spitAction;

        u32                     m_spitBoneIndex;
        bbool                   m_spitProcessed;
        f32                     m_absorbedAnimCursor;
        bbool                   m_stoppingVacuum;
        bbool                   m_stoppingVacuumReset;
        
        class Ray_GeyserPlatformAIComponent * m_geyserPlatformAIComponent;
        
    private:
        ITF_INLINE const class Ray_AIShooterVacuumBehavior_Template * getTemplate() const;

    };

    class Ray_AIShooterVacuumBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterVacuumBehavior_Template,TemplateAIBehavior,3152160776);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIShooterVacuumBehavior);

    public:

        Ray_AIShooterVacuumBehavior_Template();
        ~Ray_AIShooterVacuumBehavior_Template();

        ITF_INLINE const VacuumSkill_Template &         getVacuumSkill()            const { return m_vacuumSkill; }
        ITF_INLINE const AIAction_Template *	        getWaitActionTemplate()		const { return m_waitAction; }
        ITF_INLINE const AIAction_Template *	        getVacuumActionTemplate()	const { return m_vacuumAction; }
        ITF_INLINE const AIAction_Template *	        getSpitActionTemplate()		const { return m_spitAction; }
        ITF_INLINE const StringID &	                    getSpitMarker()		        const { return m_spitMarker; }
        ITF_INLINE const StringID &	                    getSpitBoneName()		    const { return m_spitBoneName; }
        ITF_INLINE const Vec2d &	                    getSpitForce()		        const { return m_spitForce; }
        ITF_INLINE const StringID &	                    getStartVacuumMarker()		const { return m_startVacuumMarker; }

    private:

        VacuumSkill_Template    m_vacuumSkill;
        AIAction_Template *	    m_waitAction;
        AIAction_Template *	    m_vacuumAction;
        AIAction_Template *	    m_spitAction;
        StringID                m_spitMarker;
        StringID                m_spitBoneName;
        Vec2d                   m_spitForce;
        StringID                m_startVacuumMarker;

    };

    ITF_INLINE const class Ray_AIShooterVacuumBehavior_Template * Ray_AIShooterVacuumBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIShooterVacuumBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_