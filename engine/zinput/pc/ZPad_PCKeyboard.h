#ifndef _ITF_ZPAD_PCKEYBOARD_H_
#define _ITF_ZPAD_PCKEYBOARD_H_

#if defined(ITF_WINDOWS)

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif //_ITF_ZPAD_BASE_H_

namespace ITF
{
    class ZPad_PCKeyboard : public ZPad_Base
    {
    public:
        explicit ZPad_PCKeyboard(u32 id) : ZPad_Base(id) {}
        ~ZPad_PCKeyboard() override = default;

        EInputSourceType GetInputSourceType() const override { return InputSource_Keyboard; }

        u32 GetFirstActiveControl() const override;

    protected:
        PhysicalButtonMapping GetPhysicalButtonMapping() const override;
        void UpdateDeviceInfo(SDeviceInfo& deviceInfo) override;
        bbool IsSourceAllowed() const override;
    };
} // namespace ITF

#endif // defined(ITF_WINDOWS)

#endif //_ITF_ZPAD_PCKEYBOARD_H_
