#ifndef _ITF_RAY_AISHOOTERPROJECTILEACTION_H_
#define _ITF_RAY_AISHOOTERPROJECTILEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_RAY_BASICBULLET_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_BasicBullet.h"
#endif //_ITF_RAY_BASICBULLET_H_

namespace ITF
{
    class Ray_AIShooterProjectileAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterProjectileAction, AIAction,2250286364);

    public:

        Ray_AIShooterProjectileAction();
        virtual ~Ray_AIShooterProjectileAction();

		virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
		virtual void            onDeactivate();
		
        virtual void            update( f32 _dt );
		virtual void            onEvent( Event * _event );

        ITF_INLINE bbool        hasExitScreen() const { return m_bulletAbility.hasExitScreen(); }

	protected:
		Ray_BasicBullet 		m_bulletAbility;
        Vec2d                   m_prevPositionOffset;

    private:

        ITF_INLINE const class Ray_AIShooterProjectileAction_Template* getTemplate() const;
    };

    class Ray_AIShooterProjectileAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterProjectileAction_Template,AIAction_Template,2097305397);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIShooterProjectileAction);

    public:

        Ray_AIShooterProjectileAction_Template();
        ~Ray_AIShooterProjectileAction_Template() {}

		ITF_INLINE const Ray_BasicBullet_Template *	    getBasicBulletTemplate()	const	{ return &m_basicBullet; }
        ITF_INLINE const Vec2d &	                    getPosOffset()	            const	{ return m_posOffset; }
        
	private :

		Ray_BasicBullet_Template    m_basicBullet;
        Vec2d                       m_posOffset;

    };

    ITF_INLINE const class Ray_AIShooterProjectileAction_Template* Ray_AIShooterProjectileAction::getTemplate() const { return static_cast<const Ray_AIShooterProjectileAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AISHOOTERPROJECTILEACTION_H_

