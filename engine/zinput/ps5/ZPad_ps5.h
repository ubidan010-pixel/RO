#ifndef ITF_INPUTDEVICE_PS5_H_
#define ITF_INPUTDEVICE_PS5_H_

#pragma once

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif //_ITF_ZPAD_BASE_H_

namespace ITF
{
#ifdef ITF_PS5
    class ZPad_ps5 : public ZPad_Base
    {
    public:
        explicit ZPad_ps5( u32 id )
            : ZPad_Base(id)
        {}
        virtual ~ZPad_ps5() = default;

    protected:
        virtual PhysicalButtonMapping GetPhysicalButtonMapping() const override;
        virtual void InitPlatformSpecificControls() override;
        virtual void UpdatePlatformSpecificControls(SDeviceInfo& deviceInfo, const InputAdapter::PressStatus* buttons, const f32* axes) override;

    private:
        u32 m_accelerometerControlIndex = U32_INVALID;
    };
#endif // ITF_PS5
}//namespace ITF

#endif // ITF_INPUTDEVICE_PS5_H_

