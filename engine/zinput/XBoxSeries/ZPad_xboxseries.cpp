#include "precompiled_engine.h"

#include "engine/zinput/XBoxSeries/ZPad_XBoxSeries.h"
#include "engine/AdaptersInterfaces/InputAdapter.h"

namespace ITF
{
#if defined(ITF_XBOX_SERIES)
    ZPad_Base::PhysicalButtonMapping ZPad_XBoxSeries::GetPhysicalButtonMapping() const
    {
        PhysicalButtonMapping mapping;
        mapping.faceButtonSouth = m_joyButton_A;
        mapping.faceButtonEast  = m_joyButton_B;
        mapping.faceButtonWest  = m_joyButton_X;
        mapping.faceButtonNorth = m_joyButton_Y;
        mapping.shoulderLeft    = m_joyButton_LB;
        mapping.shoulderRight   = m_joyButton_RB;
        mapping.selectButton    = m_joyButton_Back;
        mapping.startButton     = m_joyButton_Start;
        return mapping;
    }

#endif
} //namespace ITF
