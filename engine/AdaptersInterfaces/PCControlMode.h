#ifndef _ITF_PCCONTROLMODE_H_
#define _ITF_PCCONTROLMODE_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

namespace ITF
{
#if defined(ITF_WINDOWS)
    enum PCControlMode
    {
        PCControlMode_Keyboard = 0,
        PCControlMode_Controller = 1,
        PCControlMode_Hybrid = 2,
        PCControlMode_Count = 3
    };

    static const u32 PC_CONTROL_MODE_CHOICES = static_cast<u32>(PCControlMode_Count);
#endif
}

#endif //_ITF_PCCONTROLMODE_H_
