#include "precompiled_InputAdapter_iPad.h"

#ifndef _ITF_INPUTADAPTER_IPAD_H_
#include "adapters/InputAdapter_iPad/InputAdapter_iPad.h"
#endif //_ITF_INPUTADAPTER_IPAD_H_


namespace ITF
{	
	const AABB		InputAdapter_iPad::s_joyVirtualZone(Vec2d(0.f, 0.f), Vec2d(150.f, 150.f));
	const AABB		InputAdapter_iPad::s_startVirtualZone(Vec2d(0.f, 700.f), Vec2d(100.f, 768.f));
	const AABB		InputAdapter_iPad::s_XVirtualZone(Vec2d(824, 0), Vec2d(924, 68));
	const AABB		InputAdapter_iPad::s_AVirtualZone(Vec2d(924, 0), Vec2d(1024, 68));
	
	
	InputAdapter_iPad::InputAdapter_iPad()
	{
		m_accelerationX = 0.f;
		m_accelerationY = 0.f;
		m_accelerationZ = 0.f;
		
		for(u32 i = 0; i < JOY_MAX_BUT; ++i)
		{
			m_virtualJoyStatus[ i ] = Released;
		}
		
		for(u32 i = 0; i < MAX_AXES; ++i)
		{
			m_virtualAxeStatus[ i ] = 0.f;
		}
	}

	bbool InputAdapter_iPad::isKeyPressed(i32 _key) const
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
		return bfalse;
	}

	bbool InputAdapter_iPad::isMousePressed(  MouseButton _but) const
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
		return bfalse;
	}

	void InputAdapter_iPad::getMousePos( i32& _x, i32& _y) const
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
	}

	u32 InputAdapter_iPad::getGamePadCount()
	{
		return 1;
	}

	void InputAdapter_iPad::getGamePadPos(u32 _environment ,u32 _pad, float *_pos, u32 _numAxes) const
	{
        if ((m_environmentInput & _environment) != 0) //This environment is enable ,fill the buffer
        {
            for (u32 i = 0 ; i < _numAxes ; i++)
                _pos[i] = m_virtualAxeStatus[i];
        }
        else
        {
              for (u32 i = 0 ; i < _numAxes ; i++)//environment not enable,skip it
                _pos[i] = 0;
        }
	}

	void InputAdapter_iPad::getGamePadButtons(u32 _environment ,u32 _pad, PressStatus* _buttons, u32 _numButtons) const
	{
		if(_pad == 0 &&  (m_environmentInput & _environment) != 0)
		{
			for (u32 i = 0; i < _numButtons; i++)
			{
				_buttons[i] =   m_virtualJoyStatus[ i ];
			}
		}
		else
		{
			for (u32 i = 0; i < _numButtons; i++)
			{
				_buttons[i] =   Released;
			}			
		}

	}

	void InputAdapter_iPad::padVibration(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
	}

	void InputAdapter_iPad::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
	}

	void InputAdapter_iPad::onKey(i32 _key, InputAdapter::PressStatus _status )
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
	}

	void InputAdapter_iPad::onMouseWheel(i32 _wheelValue)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
	}

	void InputAdapter_iPad::onMousePos(i32 _x, i32 _y)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
	}
	
	void InputAdapter_iPad::onTouch(NSSet* touches, UIView* view)
	{
		m_currentTouches.clear();
		
		for(u32 index = 0; index < [touches count]; ++index)
		{
			UITouch *touch = [[touches allObjects] objectAtIndex:index];
			if([touch phase] != UITouchPhaseCancelled)
			{
				Touch			newTouch;
				CGPoint			location = [touch locationInView: view];
				CGPoint			previousLocation = [touch previousLocationInView: view];
				
				newTouch.x = location.y;
				newTouch.y = location.x;
				
				newTouch.px = previousLocation.y;
				newTouch.py = previousLocation.x;
				
				switch([touch phase])
				{
					case UITouchPhaseBegan:
						newTouch.state = Touch::eJustPressed;
						break;
					case UITouchPhaseMoved:
						newTouch.state = Touch::eMoved;
						break;
					case UITouchPhaseStationary:
						newTouch.state = Touch::ePressed;
						break;
					case UITouchPhaseEnded:
						newTouch.state = Touch::eReleased;
					case UITouchPhaseCancelled:
						break;
				}
				
				m_currentTouches.push_back(newTouch);
			}
		}
	}
	
	void InputAdapter_iPad::onAccelerometer(f32 x, f32 y, f32 z)
	{
		m_accelerationX = x;
		m_accelerationY = y;
		m_accelerationZ = z;
	}
	
	void InputAdapter_iPad::update()
	{
		m_touches.clear();
		foreach(touch, m_currentTouches)
		{
			m_touches.push_back(*touch);
		}
		
		foreach(touch, m_currentTouches)
		{
			if(touch->state == Touch::eReleased)
			{
				m_currentTouches.erase(touch);
			}
			else if(touch->state == Touch::eJustPressed || touch->state == Touch::eMoved)
			{
				touch->state = Touch::ePressed;
			}
		}
		
		for(u32 i = 0; i < JOY_MAX_BUT; ++i)
		{
			m_virtualJoyStatus[ i ] = Released;
		}
		
		for(u32 i = 0; i < MAX_AXES; ++i)
		{
			m_virtualAxeStatus[ i ] = 0.f;
		}
		
		[m_mainWindow updateVirtualJoyStickPosition:s_joyVirtualZone.getCenter().m_y :s_joyVirtualZone.getCenter().m_x];
		foreach(touch, m_touches)
		{
			CheckTouchZone(touch->x, touch->y, touch->state);
			CheckAxeZone(touch->x, touch->y);
		}
	}
	
	ITF::Vector<Touch>*	InputAdapter_iPad::getTouchList()
	{
		return &m_touches;
	}
	
	void InputAdapter_iPad::CheckTouchZone(i32 x, i32 y, Touch::State state)
	{
		u32 button = GetVirtualButton(x, y);
		
		if(button < JOY_MAX_BUT)
		{
			switch(state)
			{
				case Touch::eJustPressed:
					m_virtualJoyStatus[ button ] = JustPressed;
					break;
					
				case Touch::ePressed:
				case Touch::eMoved:
					m_virtualJoyStatus[ button ] = Pressed;
					break;
					
				case Touch::eReleased:
					m_virtualJoyStatus[ button ] = JustReleased;
					break;
				default:
					break;
			}
		}
	}
		
	u32 InputAdapter_iPad::GetVirtualButton(i32 x, i32 y)
	{
		Vec2d pos(x, y);
		if(s_AVirtualZone.contains(pos))
		{
			return m_joyButton_A;
		}
		else if(s_startVirtualZone.contains(pos))
		{
			return m_joyButton_Start;
		}
		else if(s_XVirtualZone.contains(pos))
		{
			return m_joyButton_X;
		}
		
		return JOY_MAX_BUT;
	}
	
	void InputAdapter_iPad::CheckAxeZone(i32 x, i32 y)
	{
		Vec2d pos(x, y);
		if(s_joyVirtualZone.contains(pos))
		{
			Vec2d center = s_joyVirtualZone.getCenter();
			m_virtualAxeStatus[ 0 ] = (pos.m_x - center.m_x) / (s_joyVirtualZone.getWidth() * .2f);
			if(m_virtualAxeStatus[ 0 ] > 1.f)
			{
				m_virtualAxeStatus[ 0 ] = 1.f;
			}
			else if(m_virtualAxeStatus[ 0 ] < -1.f)
			{
				m_virtualAxeStatus[ 0 ] = -1.f;
			}
			m_virtualAxeStatus[ 1 ] = (pos.m_y - center.m_y) / (s_joyVirtualZone.getHeight() * .2f);
			if(m_virtualAxeStatus[ 1 ] > 1.f)
			{
				m_virtualAxeStatus[ 1 ] = 1.f;
			}
			else if(m_virtualAxeStatus[ 1 ] < -1.f)
			{
				m_virtualAxeStatus[ 1 ] = -1.f;
			}
			
			[m_mainWindow updateVirtualJoyStickPosition:y :x];
		}
	}
	
	void InputAdapter_iPad::setVirtualButtons(EAGLView* view)
	{
		CGRect buttonZone;
		Vec2d origin = s_startVirtualZone.getMin();
		buttonZone.origin = CGPointMake(origin.m_y, origin.m_x);
		buttonZone.size = CGSizeMake(s_startVirtualZone.getWidth(), s_startVirtualZone.getHeight());
		[view addVirtualButton:buttonZone :"start.png"];
		
		origin = s_XVirtualZone.getMin();
		buttonZone.origin = CGPointMake(origin.m_y, origin.m_x);
		buttonZone.size = CGSizeMake(s_XVirtualZone.getWidth(), s_XVirtualZone.getHeight());
		[view addVirtualButton:buttonZone :"hit.png"];
		
		origin = s_AVirtualZone.getMin();
		buttonZone.origin = CGPointMake(origin.m_y, origin.m_x);
		buttonZone.size = CGSizeMake(s_AVirtualZone.getWidth(), s_AVirtualZone.getHeight());
		[view addVirtualButton:buttonZone :"jump.png"];
		
		origin = s_joyVirtualZone.getMin();
		buttonZone.origin = CGPointMake(origin.m_y, origin.m_x);
		buttonZone.size = CGSizeMake(s_joyVirtualZone.getWidth(), s_joyVirtualZone.getHeight());
		[view setVirtualJoyStick:buttonZone :"joystick.tif" :"joystickcursor.tif"];
		
		m_mainWindow = view;
	}
} // namespace ITF

