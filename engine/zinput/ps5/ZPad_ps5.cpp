#include "precompiled_engine.h"

#include "engine/zinput/ps5/ZPad_ps5.h"

#include "engine/AdaptersInterfaces/InputAdapter.h"
#include "adapters/InputAdapter_PS5/InputAdapter_PS5.h"

namespace ITF
{
    ZPad_Base::PhysicalButtonMapping ZPad_ps5::GetPhysicalButtonMapping() const
    {
        PhysicalButtonMapping mapping;
        mapping.faceButtonSouth = m_joyButton_Cross;
        mapping.faceButtonEast  = m_joyButton_Circle;
        mapping.faceButtonWest  = m_joyButton_Square;
        mapping.faceButtonNorth = m_joyButton_Triangle;
        mapping.shoulderLeft    = m_joyButton_L1;
        mapping.shoulderRight   = m_joyButton_R1;
        mapping.selectButton    = m_joyButton_TouchPad;
        mapping.startButton     = m_joyButton_Start;
        return mapping;
    }

    void ZPad_ps5::InitPlatformSpecificControls()
    {
        m_accelerometerControlIndex = AddPlatformControl(SInputInfo::INPUTTYPE_ACCELEROMETER, "ACCELEROMETER");
    }

    void ZPad_ps5::UpdatePlatformSpecificControls(SDeviceInfo& deviceInfo, const InputAdapter::PressStatus* /*buttons*/, const f32* /*axes*/)
    {
        if (m_accelerometerControlIndex == U32_INVALID)
            return;

        Vec3d acc;
        InputAdapter_PS5* ps5InputAdapter = static_cast<InputAdapter_PS5 *>(InputAdapter::getptr());
        ps5InputAdapter->getPadAcc(m_id, acc);

        SInputInfo& info = deviceInfo.m_inputInfo[m_accelerometerControlIndex];
        if (m_accelerometerControlIndex < m_deviceInfo.m_inputInfo.size())
        {
            info.m_accelerometerInfo.m_prevAcc = m_deviceInfo.m_inputInfo[m_accelerometerControlIndex].m_accelerometerInfo.m_acc;
        }

        info.m_accelerometerInfo.m_acc.x = acc.m_x;
        info.m_accelerometerInfo.m_acc.y = acc.m_y;
        info.m_accelerometerInfo.m_acc.z = acc.m_z;
    }
} //namespace ITF
