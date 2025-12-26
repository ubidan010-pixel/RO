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
        static constexpr u32 MAX_KEY_MAPPINGS = 32;

        explicit ZPad_PCKeyboard(u32 id);
        ~ZPad_PCKeyboard() override = default;

        EInputSourceType GetInputSourceType() const override { return InputSource_Keyboard; }

        u32 GetFirstActiveControl() const override;
        void SetRemap(u32 logicalControl, u32 physicalControl) override;
        void ResetRemapping() override;
        u32 GetRemap(u32 logicalControl) const override;
        i32 GetFirstPressedRawKey() const;
        void SetKeyMapping(u32 logicalControl, i32 keyCode);
        i32 GetKeyMapping(u32 logicalControl) const;
        void ResetKeyMappings();
        bbool HasCustomKeyMappings() const;
    protected:
        PhysicalButtonMapping GetPhysicalButtonMapping() const override;
        void UpdateDeviceInfo(SDeviceInfo& deviceInfo) override;
        void UpdateDeviceInfoRaw(SDeviceInfo& deviceInfo) override;
        bbool IsSourceAllowed() const override;

    private:

        i32 m_keyMappings[MAX_KEY_MAPPINGS];
        static i32 GetDefaultKeyForButton(u32 buttonIndex);
    };
} // namespace ITF

#endif // defined(ITF_WINDOWS)

#endif //_ITF_ZPAD_PCKEYBOARD_H_
