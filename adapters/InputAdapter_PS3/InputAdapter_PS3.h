#ifndef _ITF_INPUTADAPTER_PS3_H_
#define _ITF_INPUTADAPTER_PS3_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#include <cell/pad.h>
#include <cell/keyboard.h>

#define CELL_UTIL_PAD_MAX	7

namespace ITF
{
    typedef struct 
    {
        u32             ID;
        char*           szName;
        u32             dwDataType;
        u32             mappingValue;
    } CONTROL_DESCRIPTION;

    class Vec2d;

    class Joy
    {
    public:

        static const u32 MAX_AXES   =   6;

        Joy();
        ~Joy();

        ITF_INLINE i32                      getId       (   )           {   return m_id;    }
        ITF_INLINE void                     setId       (  i32 _id )    {   m_id = _id; }

        float                               getAxe      (u32 _axe)  const;
        InputAdapter::PressStatus           getButton   (u32 _button)   const;
        void                                padVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time);

        void                                UpdateInputState(const CONTROL_DESCRIPTION controlDesc[]);
        void                                UpdateNoInputState(const CONTROL_DESCRIPTION controlDesc[]); // call it when pad is disconnected or intercepted

        void                                mapButtons(const CONTROL_DESCRIPTION& controlDesc,bbool boolValue,f32 floatValue,const Vec2d* _pointValue);

        protected:

        BYTE                                m_bControlDataBlock[128];

        i32                                 m_id;
        float                               m_axes[MAX_AXES];
        InputAdapter::PressStatus           m_buttons[JOY_MAX_BUT];
    private:

        struct	RumbleState
        {			
            f64 	duringTime; 
            f64 	startTime; 
            f64		lastEndTime;
            bbool	motorOn;
        };

        RumbleState                         m_rumbleState;
    };

#ifndef ITF_FINAL
    #define ITF_ALLOW_KEYBOARD
    #define ITF_ALLOW_MOUSE
#endif

    class InputAdapter_PS3 : public InputAdapter
    {
    private:
        static const u32        maxPad    =    7;
        u32                     getConnectedPadIndex (u32 _padIndex) const;
    public:
                                InputAdapter_PS3 (    );
                                ~InputAdapter_PS3(    );

        void                    getGamePadPos       (  u32 _environment, u32 _pad, float *_pos, u32 _numAxes ) const;
        void                    getGamePadButtons   (  u32 _environment , u32 _pad, PressStatus* _buttons, u32 _numButtons ) const;
        u32                     getGamePadCount     (    );

        void                    updateAllInputState();
        void		            startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed);
        void                    stopRumble(u32 _numPad);
        bbool                   isConnected(u8 pad) const;

    private:

        CellPadInfo2            sPadInfo2;
        Joy                     m_Joy[CELL_UTIL_PAD_MAX];


        void                    updateKeyboard();
        void                    updateMouse();
        

        enum
        {
            MaxMouseButtons     = 5
        };
        bool                    m_MouseButtons[MaxMouseButtons];  // array of mouse states

        void KeyboardDisconnected();
        void KeyboardConnected(u32 _port);
        void CleanKeyboardStates();
        enum { InvalidKeyboardPort = u32(-1) };

        void ProcessKeyEvents();
        void ProcessMetaKeyEvents();
        template <u32 Mask, u32 ItfKey>
        void ProcessMetaKeyEvent();

        enum
        {
            MaxKeyboardKeyPressed = CELL_KB_MAX_KEYCODES,
            MaxKeyboardKeyCode = 256,
        };

        i32 ConvertKeyCode(u32 _cellKbKeyCode); // platform specific to itf specific key code conversion
        void initConvertKeyTable();
        i32 m_cellToItfConvertTable[MaxKeyboardKeyCode];

        typedef Vector<u32> VecKeyPressed;
        VecKeyPressed m_lastKeyPressed;
        VecKeyPressed m_keyPressed;
        u32 m_lastMetaKeyFlags;
        u32 m_metaKeyFlags;
        u32 m_currentKeyBoardPort;
    };

} // namespace ITF

#endif //_ITF_INPUTADAPTER_PS3_H_