#pragma once

#include "engine/zinput/ZPad_Base.h"

namespace ITF
{
#if defined(ITF_XBOX_SERIES)
    class ZPad_XBoxSeries : public ZPad_Base
    {
    public:

        explicit ZPad_XBoxSeries( u32 id ) : ZPad_Base(id) {}
        virtual ~ZPad_XBoxSeries() = default;

    protected:
        virtual PhysicalButtonMapping GetPhysicalButtonMapping() const override;
    };
#endif // defined(ITF_XBOX_SERIES)
}//namespace ITF


