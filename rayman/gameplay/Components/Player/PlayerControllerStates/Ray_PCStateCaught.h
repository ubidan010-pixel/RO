#ifndef _ITF_RAY_PCSTATECAUGHT_H_
#define _ITF_RAY_PCSTATECAUGHT_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateCaught : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateCaught,Ray_State,1130543178)

public:

    StateCaught() {}
    virtual ~StateCaught() {};

    virtual bbool   checkStateChange( f32 _dt );
    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(CAUGHT,1993717254);
    }

private:

};

#endif //_ITF_RAY_PCSTATECAUGHT_H_


