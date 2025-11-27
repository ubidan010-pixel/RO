#ifndef _ITF_INPUTDEVICE_X360_H_
#define _ITF_INPUTDEVICE_X360_H_

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif //_ITF_ZPAD_BASE_H_

namespace ITF
{
#if defined(ITF_WINDOWS) || defined(ITF_X360)
    class ZPad_x360 : public ZPad_Base
    {
    public:

        ZPad_x360( u32 id ) : ZPad_Base(id) {} 
        virtual ~ZPad_x360() {}

    protected:
        virtual PhysicalButtonMapping GetPhysicalButtonMapping() const override;
    };
#endif // defined(ITF_WINDOWS) || defined(ITF_X360)
}//namespace ITF

#endif // _ITF_INPUTDEVICE_WIN32_H_


