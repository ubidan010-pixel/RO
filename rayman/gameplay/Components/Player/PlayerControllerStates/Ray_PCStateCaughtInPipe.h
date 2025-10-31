#ifndef _ITF_RAY_PCSTATECAUGHTINPIPE_H_
#define _ITF_RAY_PCSTATECAUGHTINPIPE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_


    class StateCaughtInPipe : public Ray_State
    {
        DECLARE_OBJECT_CHILD_RTTI(StateCaughtInPipe, Ray_State,931785324)

    public:

        StateCaughtInPipe();
        virtual ~StateCaughtInPipe();

        virtual StringID getAnimAction() const
        {
            return ITF_GET_STRINGID_CRC(CAUGHT_IN_PIPE,3846259820);
        }

        virtual void    onEnter();
        virtual void    onExit();
        virtual bbool   checkStateChange( f32 _dt );
        virtual void    onEvent( Event* _event );

    private:

        bbool   m_released;
    };


#endif //_ITF_RAY_PCSTATECAUGHTINPIPE_H_