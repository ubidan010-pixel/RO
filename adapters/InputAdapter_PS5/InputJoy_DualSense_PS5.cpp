#include "precompiled_inputadapter_PS5.h"

#include "InputJoy_DualSense_PS5.h"

#include "core/math/MathTools.h"

#include "engine/AdaptersInterfaces/GFXAdapter.h"

#include <vectormath.h>

namespace ITF
{
    // Convert axis value from range [0-255] to [min-max]
    f32 axis_convert(const u8 _value, const u8 _deadzone, f32 _min, f32 _max) 
    {
        const f32 deadZone = (f32)_deadzone / 255.f;
        const f32 rscale = (_max - _min) / _max;
        f32 result = ((f32)_value * rscale / 255.f) + _min;
        return f32_Abs(result) < deadZone ? 0.0f : result;
    }

    CONTROL_DESCRIPTION g_ControlDescGamepad[] = 
    {
        {SCE_PAD_BUTTON_UP,			"DPAD_UP",			0,		m_joyButton_DPadU},
        {SCE_PAD_BUTTON_DOWN,		"DPAD_DOWN",		0,		m_joyButton_DPadD},
        {SCE_PAD_BUTTON_LEFT,		"DPAD_LEFT",		0,		m_joyButton_DPadL},
        {SCE_PAD_BUTTON_RIGHT,		"DPAD_RIGHT",		0,		m_joyButton_DPadR},

        {SCE_PAD_BUTTON_START,		"START",			0,		m_joyButton_Start},
        {SCE_PAD_BUTTON_OPTIONS,	"BACK",				0,		m_joyButton_Back},

        {SCE_PAD_BUTTON_L1,			"L_SHOULDER",		0,		m_joyButton_L1},
        {SCE_PAD_BUTTON_R1,			"R_SHOULDER",		0,		m_joyButton_R1},

        {SCE_PAD_BUTTON_L2,			"L_SHOULDER2",		0,		m_joyButton_L2},
        {SCE_PAD_BUTTON_R2,		    "R_SHOULDER2",		0,		m_joyButton_R2},

        {SCE_PAD_BUTTON_L3,			"L_THUMB",			0,		m_joyButton_ThumbLeft},
        {SCE_PAD_BUTTON_R3,		    "R_THUMB",			0,		m_joyButton_ThumbRight},

        {SCE_PAD_BUTTON_CROSS,		"DPAD_CROSS",		0,		m_joyButton_Cross},
        {SCE_PAD_BUTTON_CIRCLE,		"DPAD_CIRCLE",		0,		m_joyButton_Circle},
        {SCE_PAD_BUTTON_SQUARE,		"DPAD_SQUARE",		0,		m_joyButton_Square},
        {SCE_PAD_BUTTON_TRIANGLE,	"DPAD_TRIANGLE",	0,		m_joyButton_Triangle},
        {SCE_PAD_BUTTON_TOUCH_PAD,	"TOUCH_PAD",		0,		m_joyButton_TouchPad}
    };
    const u32 g_dwNumDescGamepad = sizeof( g_ControlDescGamepad )/sizeof(g_ControlDescGamepad[0]);

    InputJoy_DualSense::InputJoy_DualSense()
    {
        ITF_MemSet(&m_padData,				0,	sizeof(m_padData));
        ITF_MemSet(&m_latestMoveData,		0,	sizeof(m_latestMoveData));

        m_statesNo							= 0;
        m_isVibrating						= bfalse;
        m_vibrationStartTime				= 0.0;
        m_vibrationTime 				    = 0.0;
        m_leftMotorSpeed					= 0.f;
        m_rightMotorSpeed					= 0.f;

        ITF_MemSet(m_lastTouchData,			0, sizeof(m_lastTouchData));
        ITF_MemSet(m_currentTouchData,		0, sizeof(m_currentTouchData));
    }

    void InputJoy_DualSense::init(i32 _portHandle, i32 _id )
    {
        InputJoy_PS5::init(_portHandle, _id, JOY_TYPE_DUALSENSE);
    }

    f32 InputJoy_DualSense::getAxe(u32 _axe) const
    {
        ITF_ASSERT(_axe < MAX_AXES);
        switch(_axe)
        {
            case AXE_LEFT_X:
                return axis_convert(m_latestMoveData.leftStick.x, m_padInfo.stickInfo.deadZoneLeft, -1.f, 1.f);
            case AXE_LEFT_Y:
                return - axis_convert(m_latestMoveData.leftStick.y, m_padInfo.stickInfo.deadZoneLeft, -1.f, 1.f);
            case AXE_RIGHT_X:
                return axis_convert(m_latestMoveData.rightStick.x, m_padInfo.stickInfo.deadZoneRight, -1.f, 1.f);
            case AXE_RIGHT_Y:
                return - axis_convert(m_latestMoveData.rightStick.y, m_padInfo.stickInfo.deadZoneRight, -1.f, 1.f);
            case AXE_TRIGGER_LEFT:
                return axis_convert(m_latestMoveData.analogButtons.l2, m_padInfo.stickInfo.deadZoneLeft, 0.f, 1.f);
            case AXE_TRIGGER_RIGHT:
                return axis_convert(m_latestMoveData.analogButtons.r2, m_padInfo.stickInfo.deadZoneRight, 0.f, 1.f);
        }
        return 0.f;
    }

    bbool InputJoy_DualSense::isConnected() const
    {
        return m_latestMoveData.connected;
    }

    InputAdapter::PadType InputJoy_DualSense::getPadType() const
    {
        return InputAdapter::Pad_PS5;
    }

    void InputJoy_DualSense::updateInputState()
    {
        if(m_portHandle < 0)
            return;

        scePadGetControllerInformation(m_portHandle, &m_padInfo);

        m_statesNo = scePadRead(m_portHandle, m_padData, SCE_PAD_MAX_DATA_NUM);
        if(m_statesNo <= 0)
            return;

        m_latestMoveData = m_padData[m_statesNo - 1];

        if(bfalse == m_latestMoveData.connected)
            return;

        // update buttons
        InputAdapter::PressStatus prevButtons[JOY_MAX_BUT];
        ITF_Memcpy(prevButtons, m_buttons, sizeof(m_buttons));

        for(ux i = 0, n = g_dwNumDescGamepad; i < n; i++)
        {
            InputAdapter::PressStatus & buttonPressStatus = m_buttons[g_ControlDescGamepad[i].mappingValue];
            buttonPressStatus = InputAdapter::Released;
            for(i32 sample = 0, nbSample = m_statesNo; sample<nbSample; ++sample)
            {
                if (m_padData[sample].buttons & g_ControlDescGamepad[i].ID)
                {
                    buttonPressStatus = InputAdapter::Pressed;
                    break;
                }
            }
        }

        for(ux i = 0; i < JOY_MAX_BUT; i++)
        {
            if (prevButtons[i] != InputAdapter::JustPressed && prevButtons[i] != InputAdapter::Pressed && m_buttons[i] == InputAdapter::Pressed)
                m_buttons[i] = InputAdapter::JustPressed;
            else if (prevButtons[i] != InputAdapter::JustReleased && prevButtons[i] != InputAdapter::Released && m_buttons[i] == InputAdapter::Released)
                m_buttons[i] = InputAdapter::JustReleased;
        }

        // update vibrations
        if(m_isVibrating)
        {
            f64 vibratingElapsedTime = SYSTEM_ADAPTER->getTime() - m_vibrationStartTime;

            // check if need to vibrate more than 3 seconds
            if((vibratingElapsedTime >= 2.9f) && (m_vibrationTime > 3.f) )
            {
                m_vibrationTime -= vibratingElapsedTime;
                padVibration(m_leftMotorSpeed, m_rightMotorSpeed, m_vibrationTime);
            }
            else
            {
                // stop
                if(vibratingElapsedTime >= m_vibrationTime)
                {
                    padVibration(0.f, 0.f, 0.0);
                }
            }
        }

        // update touch data
        bbool lastValidState[SCE_PAD_MAX_TOUCH_NUM];
        for(u8 i = 0; i < SCE_PAD_MAX_TOUCH_NUM; ++i)
            lastValidState[i] = m_lastTouchData[i].valid;
        ITF_Memcpy(m_lastTouchData, m_currentTouchData, sizeof(m_lastTouchData));
        for(u8 i = 0; i < SCE_PAD_MAX_TOUCH_NUM; ++i)
        {
            m_currentTouchData[i].valid = (m_latestMoveData.touchData.touchNum >= (i+1));
            if(m_currentTouchData[i].valid)
            {
                // Round to not respond to micro move (min move = 1 pixel)
                m_currentTouchData[i].pos.set(  roundFloat(((f32)(m_latestMoveData.touchData.touch[i].x) / (f32)m_padInfo.touchPadInfo.resolution.x) * (f32)GFX_ADAPTER->getScreenWidth()),
                                                roundFloat(((f32)(m_latestMoveData.touchData.touch[i].y) / (f32)m_padInfo.touchPadInfo.resolution.y) * (f32)GFX_ADAPTER->getScreenHeight()) );

                if(!lastValidState[i])
                {
                    m_lastTouchData[i].pos = m_currentTouchData[i].pos;
                }
            }
        }
    }

    void InputJoy_DualSense::getLowLevelAccels(ITF_VECTOR<Vec3d>& _accels, bbool _invertX, bbool _invertY, bbool _invertZ, f32 _maxLimit) const
    {
        Vec3d vec;
        for(u32 i = 0; i < m_statesNo; ++i)
        {
            f32 accelX = (abs(m_padData[i].acceleration.x) > _maxLimit ? _maxLimit * m_padData[i].acceleration.x / abs(m_padData[i].acceleration.x) : m_padData[i].acceleration.x);
            f32 accelY = (abs(m_padData[i].acceleration.y) > _maxLimit ? _maxLimit * m_padData[i].acceleration.y / abs(m_padData[i].acceleration.y) : m_padData[i].acceleration.y);
            f32 accelZ = (abs(m_padData[i].acceleration.z) > _maxLimit ? _maxLimit * m_padData[i].acceleration.z / abs(m_padData[i].acceleration.z) : m_padData[i].acceleration.z);

            // Invert the axis if necessary and set the accels
            vec.setX( _invertX ? (-1 * accelX) : accelX);
            vec.setY( _invertY ? (-1 * accelY) : accelY);
            vec.setZ( _invertZ ? (-1 * accelZ) : accelZ);

            _accels.push_back(vec);
        }
    }

    void InputJoy_DualSense::padVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time)
    {
        ITF_ASSERT(m_portHandle >= 0);

        m_vibrationStartTime		= SYSTEM_ADAPTER->getTime();
        m_vibrationTime       		= _time;
        m_leftMotorSpeed			= _leftMotorSpeed;
        m_rightMotorSpeed			= _rightMotorSpeed;

        m_isVibrating				= (m_leftMotorSpeed > 0.f) || (m_rightMotorSpeed > 0.f);

        // large = left; small = right
        ScePadVibrationParam vibration_param;
        vibration_param.largeMotor = (u8)(m_leftMotorSpeed * 255.f);
        vibration_param.smallMotor = (u8)(m_rightMotorSpeed * 255.f);

        ITF_VERIFY_SCE_CALL(scePadSetVibration(m_portHandle, &vibration_param));
    }

    void InputJoy_DualSense::getAcceleration(Vec3d& acc) const
    {
        acc.set(m_latestMoveData.acceleration.x, m_latestMoveData.acceleration.y, m_latestMoveData.acceleration.z);
    }

    Vec3d InputJoy_DualSense::getAngles() const
    {
        sce::Vectormath::Simd::Aos::Quat orient(m_latestMoveData.orientation.x,m_latestMoveData.orientation.y,m_latestMoveData.orientation.z,m_latestMoveData.orientation.w);
        sce::Vectormath::Simd::Aos::Vector3 eulerAngles = sce::Vectormath::Simd::Aos::Quat::euler(orient, sce::Vectormath::Simd::kXYZ);
        return Vec3d(eulerAngles.getX().getAsFloat(), eulerAngles.getY().getAsFloat(), eulerAngles.getZ().getAsFloat());
    }

    void InputJoy_DualSense::resetOrientation()
    {
        scePadResetOrientation(m_portHandle);
    }
}
