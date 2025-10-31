#ifndef _ITF_INPUTADAPTER_IPAD_H_
#define _ITF_INPUTADAPTER_IPAD_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_SYSTEMADAPTER_IPAD_H_
#include "adapters/SystemAdapter_iPad/SystemAdapter_iPad.h"
#endif //_ITF_SYSTEMADAPTER_IPAD_H_

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#define MAX_AXES 6

namespace ITF
{
	class Touch
	{
		public:
			typedef enum
			{
				eJustPressed,	//just after the finger is put on screen
				ePressed,		//after the finger is put on the screen the OS does not gives us any message. After one frame JustPressed state will become Pressed
				eMoved,			//when a finger is moved
                eReleased	,	//when a finger is rised from the screen. After one frame this state will be removed
                ENUM_FORCE_SIZE_32(0)
			}State;
		
		public:
			i32		x;
			i32		y;
			i32		px;		//previous x, used for drag
			i32		py;		//previous y, used for drag
			State	state;
	};
	
	//It is not a good way to use listeners for iPad because the events are asynchronous and some of input events might be lost
	//The best way is to check input states 
	//I'll use double buffering to syncronize input states 
    class InputAdapter_iPad : public InputAdapter
    {
		public:

		
		public:
			InputAdapter_iPad();

			bbool				isKeyPressed(i32 _key) const;
			bbool				isMousePressed(  MouseButton _but) const;
			void				getMousePos( i32& _x, i32& _y) const;
			u32					getGamePadCount();
			void				getGamePadPos(u32 _environment ,u32 _pad, float *_pos, u32 _numAxes) const;
			void				getGamePadButtons(u32 _environment ,u32 _pad, PressStatus* _buttons, u32 _numButtons) const;
			void				padVibration(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed);
			
			void				onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status);
			void				onKey(i32 _key, InputAdapter::PressStatus _status );
			void				onMouseWheel(i32 _wheelValue);
			void				onMousePos(i32 _x, i32 _y);
			void				onTouch(NSSet* touches, UIView* view);
			void				onAccelerometer(f32 x, f32 y, f32 z);
		
			ITF::Vector<Touch>*	getTouchList();
		
			void				update();
			void				setVirtualButtons(EAGLView* view);
		
		private:
			static const AABB		s_joyVirtualZone;
			static const AABB		s_startVirtualZone;
			static const AABB		s_XVirtualZone;
			static const AABB		s_AVirtualZone;
			
			void					CheckTouchZone(i32 x, i32 y, Touch::State state);
			void					CheckAxeZone(i32 x, i32 y);
			u32						GetVirtualButton(i32 x, i32 y);
		
			PressStatus				m_virtualJoyStatus[JOY_MAX_BUT];
			float					m_virtualAxeStatus[MAX_AXES];
		
			ITF::Vector<Touch>		m_touches;
			ITF::Vector<Touch>		m_currentTouches;
			//Accelerometer values
			f32						m_accelerationX;
			f32						m_accelerationY;
			f32						m_accelerationZ;
		
			EAGLView*				m_mainWindow;
    };
} // namespace ITF

#endif //_ITF_INPUTADAPTER_IPAD_H_