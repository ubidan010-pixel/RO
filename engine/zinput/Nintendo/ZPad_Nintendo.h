#pragma once

#include "engine/zinput/ZPad_Base.h"

#ifdef ITF_NINTENDO

namespace ITF
{
    class ZPad_Nintendo : public ZPad_Base
    {
    public:
        ZPad_Nintendo(u32 id)
            : ZPad_Base(id)
        {}

        virtual ~ZPad_Nintendo() override = default;

    protected:
        virtual PhysicalButtonMapping GetPhysicalButtonMapping() const override;
    };
}//namespace ITF

#endif // ITF_NINTENDO
