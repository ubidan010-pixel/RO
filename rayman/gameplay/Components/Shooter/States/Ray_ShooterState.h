#ifndef _ITF_RAY_SHOOTERSTATE_H_
#define _ITF_RAY_SHOOTERSTATE_H_

class Ray_ShooterStateImplement : public PlayerStateImplement
{
	DECLARE_OBJECT_CHILD_RTTI( Ray_ShooterStateImplement, PlayerStateImplement ,3251453359)
public:

	virtual void onExit()				{ Super::onExit(); getParent()->resetCapacityFlag(); } 

	ITF_INLINE Ray_ShooterControllerComponent *	getParent()	const	{ ITF_ASSERT( m_owner != NULL ); return ( m_owner ? static_cast<Ray_ShooterControllerComponent * >( m_owner->getParent() ) : NULL ); }
};





#endif //_ITF_RAY_SHOOTERSTATE_H_

