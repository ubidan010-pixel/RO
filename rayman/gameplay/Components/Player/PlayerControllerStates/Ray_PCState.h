#ifndef _ITF_RAY_PCSTATE_H_
#define _ITF_RAY_PCSTATE_H_

class Ray_State : public PlayerControllerState
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_State, PlayerControllerState,3939257140)

public:
    Ray_State()
        : m_parent(NULL)
    {}
    virtual ~Ray_State() {}

    virtual void        init( PlayerControllerComponent* _parent,
                              class AnimatedComponent* _animComponent,
                              class StickToPolylinePhysComponent* _characterPhys,
                              class FXControllerComponent* _fxController );

#ifndef ITF_FINAL
    virtual void        drawDebug();
#endif // ITF_FINAL
    bool                checkBounce();

protected:

    Ray_PlayerControllerComponent* m_parent;
};

#endif //_ITF_RAY_PCSTATE_H_

