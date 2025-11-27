#include "precompiled_engine.h"

#include "engine/zinput/PadX360/ZPadX360_common.h"

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

namespace ITF
{
#if defined(ITF_WINDOWS) || defined(ITF_X360)
    ZPad_Base::PhysicalButtonMapping ZPad_x360::GetPhysicalButtonMapping() const
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
