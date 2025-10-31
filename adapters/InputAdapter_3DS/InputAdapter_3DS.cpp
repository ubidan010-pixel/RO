#include "precompiled_inputadapter_3DS.h"

#ifndef _ITF_INPUTADAPTER_3DS_H_
#include "adapters/InputAdapter_3DS/InputAdapter_3DS.h"
#endif //_ITF_INPUTADAPTER_3DS_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#include <nn/os.h>
#include <nn/hid.h>

using namespace nn::hid;
static bool s_isAPIInitialized=false;

namespace ITF
{
    /// Constructor.
    InputAdapter_3DS::InputAdapter_3DS()
    :m_TpStatus(InputAdapter::Released)
    {
        //::ITF::InputAdapter();
		//m_accReader.SetSensitivity(0, nn::hid::MAX_OF_ACCELEROMETER_SENSITIVITY );
        InputAdapter_3DS::mayInitAPI();
    }

    /// Destructor.
    InputAdapter_3DS::~InputAdapter_3DS()
    {    
    }

    void InputAdapter_3DS::mayInitAPI()
    {
        if (!s_isAPIInitialized)
        {
            nn::Result initRes = nn::hid::CTR::Initialize();
            ITF_ASSERT(initRes.IsSuccess());           
            s_isAPIInitialized = true;
        }
    }

    void InputAdapter_3DS::getGamePadPos( u32 _environment,u32 _pad, float *_pos, u32 _numAxes ) const
    {
        if ((m_environmentInput & _environment) != 0 && _pad == 0) //This environment is enable ,fill the buffer
        {
            for (u32 i = 0 ; i < _numAxes ; i++)
            {
                _pos[i] = m_virtualPadAxes[i];
            }
        }
        else
        {
            for (u32 i = 0 ; i < _numAxes ; i++)
                _pos[i] = 0.0f;
        }        
    }

    u32 InputAdapter_3DS::getConnectedPadIndex (u32 _padIndex) const
    {
        return 0;
    }

    void InputAdapter_3DS::getGamePadButtons( u32 _environment ,u32 _pad, PressStatus* _buttons, u32 _numButtons ) const
    {
        if (_pad == 0)
        {
            for (u32 i = 0; i < _numButtons; i++)
            {
                _buttons[i] =   m_virtualPadStatus[ i ];
            }
        }
        else
        {
            for (u32 i = 0; i < _numButtons; i++)
                _buttons[i] = InputAdapter::Released;
        }
    }

    u32 InputAdapter_3DS::getGamePadCount()
    {
        return 1;
    }


    void InputAdapter_3DS::updateMouse()
    {
    }

    void InputAdapter_3DS::updateAllInputState()
    {
        /// read Pad.
        nn::hid::PadReader m_PadReader;
        nn::hid::PadStatus padStatus;
        m_PadReader.ReadLatest(&padStatus);

////////////////////////////////TEST Gyroscope/////////////////////////////////
		//const nn::hid::Direction dir(nn::math::MTX33::Identity());
		//dir = GetGyroscopeDirection();
		//m_virtualPadAxes[0] = GetGyroscopeDirection().z.x;
		//m_virtualPadAxes[1] = GetGyroscopeDirection().z.y;
		//m_virtualPadAxes[0] = GetGyroscopeAngle().x;
		//m_virtualPadAxes[1] = GetGyroscopeAngle().y;

//////////////////////////////////////////////////////////////////////////

////////////////////////////////TEST Accelerometer/////////////////////////////////

		//m_virtualPadAxes[0] = -GetAccelerometerPosX();
		//m_virtualPadAxes[1] = GetAccelerometerPosZ();

//////////////////////////////////////////////////////////////////////////
		/// get stick info.

        m_virtualPadAxes[0] = m_PadReader.NormalizeStick(padStatus.stick.x);
        m_virtualPadAxes[1] = m_PadReader.NormalizeStick(padStatus.stick.y);
		m_virtualPadAxes[2] = (padStatus.hold & nn::hid::BUTTON_L ) ? InputAdapter::Pressed :  InputAdapter::Released;
		m_virtualPadAxes[5] = (padStatus.hold & nn::hid::BUTTON_R ) ? InputAdapter::Pressed :  InputAdapter::Released;

        /// Buttons.
        InputAdapter::PressStatus temp_buttons[JOY_MAX_BUT];
        ITF_Memcpy(temp_buttons, m_virtualPadStatus, sizeof(temp_buttons));

        // get button info and map button.
        m_virtualPadStatus[0] =  (padStatus.hold & nn::hid::BUTTON_B ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[1] =  (padStatus.hold & nn::hid::BUTTON_A ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[2] =  (padStatus.hold & nn::hid::BUTTON_Y ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[3] =  (padStatus.hold & nn::hid::BUTTON_X ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[4] =  (padStatus.hold & nn::hid::BUTTON_L ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[5] =  (padStatus.hold & nn::hid::BUTTON_R ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[6] =  (padStatus.hold & nn::hid::PSEUDO_BUTTON_SELECT ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[7] =  (padStatus.hold & nn::hid::BUTTON_START ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[8] =  (padStatus.hold & nn::hid::BUTTON_LEFT ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[9] =  (padStatus.hold & nn::hid::BUTTON_RIGHT ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[10] =  (padStatus.hold & nn::hid::BUTTON_UP ) ? InputAdapter::Pressed :  InputAdapter::Released;
        m_virtualPadStatus[11] =  (padStatus.hold & nn::hid::BUTTON_DOWN ) ? InputAdapter::Pressed :  InputAdapter::Released;

        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            if (temp_buttons[i] != InputAdapter::JustPressed && temp_buttons[i] != InputAdapter::Pressed && m_virtualPadStatus[i] == InputAdapter::Pressed)
                m_virtualPadStatus[i] = InputAdapter::JustPressed;
            else if (temp_buttons[i] != InputAdapter::JustReleased && temp_buttons[i] != InputAdapter::Released && m_virtualPadStatus[i] == InputAdapter::Released)
                m_virtualPadStatus[i] = InputAdapter::JustReleased;
        }

        if ( GetLatestTouchPanelStatus().touch )
        {
            if ( m_TpStatus != InputAdapter::JustPressed && m_TpStatus != InputAdapter::Pressed )
            {
                m_TpStatus = InputAdapter::JustPressed;
            }
            else
            {
                m_TpStatus = InputAdapter::Pressed;
            }
        }
        else if ( m_TpStatus == InputAdapter::JustPressed || m_TpStatus == InputAdapter::Pressed )
        {
            m_TpStatus = InputAdapter::JustReleased;
        }
        else
        {
            m_TpStatus = InputAdapter::Released;
        }
    }

    //----------------------------------------------------------------------------//
	nn::hid::TouchPanelStatus InputAdapter_3DS::GetLatestTouchPanelStatus(void)
	{
		// Get the latest data from the touch panel
		nn::hid::TouchPanelStatus tpStatus;
		m_TouchPanelReader.ReadLatest(&tpStatus);

		return tpStatus;
	}
    //----------------------------------------------------------------------------//
	nn::hid::AccelerationFloat InputAdapter_3DS::GetLatestAccelerometerStatus(void)
	{
		// Get the latest data from the touch panel
		nn::hid::AccelerometerStatus accStatus;
		m_accReader.ReadLatest(&accStatus);
		nn::hid::AccelerationFloat acceleration;
		m_accReader.ConvertToAcceleration(&acceleration, 1, &accStatus);

		return acceleration;
	}

    //----------------------------------------------------------------------------//
	nn::hid::GyroscopeStatus InputAdapter_3DS::GetLatestGyroscopeStatus(void)
	{
		// Get the latest data from the touch panel
		nn::hid::GyroscopeStatus gyroStatus;
		m_GyroReader.ReadLatest(&gyroStatus);
		return gyroStatus;
	}

	/*---------------------------------------------------------------------------*
	  End of file
	 *---------------------------------------------------------------------------*/


} // namespace ITF
