#ifndef _ITF_RAY_PCSTATEONPEDESTAL_H_
#define _ITF_RAY_PCSTATEONPEDESTAL_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateOnPedestal : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateOnPedestal,Ray_State,3984243067)

public:

    StateOnPedestal() : m_prevPos(Vec2d::Zero), m_allowSupport(bfalse), m_justEntered(bfalse) {}
    virtual ~StateOnPedestal() {};

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(IDLE,1633200266);
    }

private:

    Vec2d               m_prevPos;
    bbool               m_allowSupport;
    bbool               m_justEntered;
};

#endif //_ITF_RAY_PCSTATEONPEDESTAL_H_


