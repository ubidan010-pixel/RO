#include "precompiled_inputadapter_PS5.h"

#include "InputJoy_PS5.h"

#include "core/utility.h"
#include "core/math/MathTools.h"

namespace ITF
{
    const int PAD_PORT_INVALID		= -1;

    InputJoy_PS5::InputJoy_PS5()
    : m_id ( U32_INVALID )
    {
        m_portHandle	= PAD_PORT_INVALID;

        fill(m_buttons, InputAdapter::Released);
        m_type = JOY_TYPE_DUALSENSE;
        
	}

	void InputJoy_PS5::init(i32 _portHandle, i32 _id, InputJoy_PS5::JoyType type)
	{
		m_portHandle	= _portHandle;
		m_type			= type;
        m_id            = _id;
	}

	InputAdapter::PressStatus InputJoy_PS5::getButton(u32 _button) const
	{
		ITF_ASSERT(_button < JOY_MAX_BUT);
		return m_buttons[_button];
	}

	void InputJoy_PS5::getAcceleration(Vec3d& acc) const
	{
		acc.setX(0.f);
		acc.setY(0.f);
		acc.setZ(0.f);
	}

    Vec3d InputJoy_PS5::getAngles() const
    {
        return Vec3d::Zero;
    }

}
