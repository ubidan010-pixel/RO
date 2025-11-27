#include "precompiled_engine.h"

#include "engine/zinput/Nintendo/ZPad_Nintendo.h"
#include "engine/AdaptersInterfaces/InputAdapter.h"
#include "adapters/InputAdapter_Nintendo/InputPad_Nintendo.h"

namespace ITF
{

    ZPad_Base::PhysicalButtonMapping ZPad_Nintendo::GetPhysicalButtonMapping() const
    {
        PhysicalButtonMapping mapping;
        mapping.faceButtonSouth = m_joyButton_B;
        mapping.faceButtonEast  = m_joyButton_A;
        mapping.faceButtonWest  = m_joyButton_Y;
        mapping.faceButtonNorth = m_joyButton_X;
        mapping.shoulderLeft    = m_joyButton_LB;
        mapping.shoulderRight   = m_joyButton_RB;
        mapping.selectButton    = m_joyButton_Minus;
        mapping.startButton     = m_joyButton_Plus;
        return mapping;
    }

} //namespace ITF
