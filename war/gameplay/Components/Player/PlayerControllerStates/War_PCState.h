#ifndef _ITF_WAR_PCSTATE_H_
#define _ITF_WAR_PCSTATE_H_

class War_State : public PlayerControllerState
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(War_State, PlayerControllerState,236768048)

public:
    virtual ~War_State() {}

    virtual void        init( PlayerControllerComponent* _parent,
                              class AnimatedComponent* _animComponent,
                              class StickToPolylinePhysComponent* _characterPhys,
                              class FXControllerComponent* _fxController );

    virtual void        drawDebug();
    bool                checkBounce();

protected:

    War_PlayerControllerComponent* m_parent;

};

#endif //_ITF_WAR_PCSTATE_H_

