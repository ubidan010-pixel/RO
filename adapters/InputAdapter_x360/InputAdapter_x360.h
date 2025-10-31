#ifndef _ITF_INPUTADAPTER_X360_H_
#define _ITF_INPUTADAPTER_X360_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#include <XInput2.h>

namespace ITF
{
    struct XINPUT_DEVICE_NODE
    {
        DWORD dwVidPid;
        XINPUT_DEVICE_NODE* pNext;
    };
    typedef struct 
    {
        XINPUTID        ID;
        WCHAR*          szName;
        DWORD           dwDataType;
        DWORD           mappingValue;
    } CONTROL_DESCRIPTION;



    typedef struct
    {
        bbool            bLAlt;
        bbool            bRAlt;
        bbool            bLCtrl;
        bbool            bRCtrl;
        bbool            bLShift;
        bbool            bRShift;
    } MODIFIER;

    // Keep track of connected game pads.
    const enum EINPUTDEVICESTATE
    {
        EINPUTDEVICE_NOTCONNECTED = 0,      // Not gamepad connected
        EINPUTDEVICE_CONNECTEDINCOMPATIBLE, // A gamepad is connected doens't meet req's
        EINPUTDEVICE_CONNECTEDCOMPATIBLE,   // Gamepad is connected and verified compatible
    };

    ///-------------------------------------------------------------------------------------
    struct XINPUTCompare
    {
        ITF_INLINE bool operator()( const XINPUTID & lhs, const XINPUTID & rhs ) const
        { 
            return ( memcmp( &lhs, &rhs, sizeof(XINPUTID) ) < 0); 
        } 
    };

    typedef std::map<XINPUTID,XINPUTID,XINPUTCompare>              mapKeyTranslator;
    typedef std::map<XINPUTID, mapKeyTranslator,XINPUTCompare>     mapDeviceMapping;

    class InputMappingTranslator
    {
    public:
        void add(XINPUTID _device,XINPUTID _key,XINPUTID _value) {m_deviceMapping[_device].insert(std::pair<XINPUTID,XINPUTID>(_key,_value));}
        XINPUTID getTranslatedID(XINPUTID _device,XINPUTID _key) const;
    private:
        mapDeviceMapping        m_deviceMapping;
    };
    ///--------------------------------------------------------------------------------------


    class Joy
    {
    public:
 
        static const u32 MAX_AXES   =   6;

        Joy();
        ~Joy();

        ITF_INLINE i32                      getId       (   )           {   return m_id;    }
        ITF_INLINE void                     setId       (  i32 _id )    {   m_id = _id; }

        void                                padVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time);
        void                                UpdateInputState(const CONTROL_DESCRIPTION controlDesc[],const InputMappingTranslator& _mappingTranslator);
        float                               getAxe      (u32 _axe)  const;
        InputAdapter::PressStatus           getButton   (u32 _button)   const;

        EINPUTDEVICESTATE                   getDeviceState()  const                     {return m_eInputDeviceState;}
        void                                setDeviceState(EINPUTDEVICESTATE _state)    {m_eInputDeviceState = _state;}

        void                                mapButtons(const CONTROL_DESCRIPTION& controlDesc,bbool boolValue,f32 floatValue,XINPUT2_POINT* pointValue = NULL);
        void                                setDeviceID(XINPUTID& _deviceID) {m_DeviceID = _deviceID;}

    protected:

        BYTE                                m_bControlDataBlock[128];

        i32                                 m_id;
        float                               m_axes[MAX_AXES];
        InputAdapter::PressStatus           m_buttons[JOY_MAX_BUT];
        EINPUTDEVICESTATE                   m_eInputDeviceState;
        XINPUTID                            m_DeviceID;

        typedef struct	RumbleState_
        {			
            f64 	duringTime; 
            f64 	startTime; 
            f64		lastEndTime;
            bbool	motorOn;
        } RumbleState;

        RumbleState                         m_rumbleState;
    };

  
    class InputX360
    {
        
    public:
        static const int MAX_PAD = 4;


        InputX360();
        ~InputX360();

        
        bool                    mb_FilterOutXinputDevices;
        XINPUT_DEVICE_NODE*     mp_XInputDeviceList;
        void                    EnumJoysticks();
        HRESULT                 InitDirectInput     ();


        GUID                    m_Instance;

        u32                     m_numberpad;
        Joy                     m_pad[MAX_PAD];

    private:
    };

    class InputAdapter_x360 : public InputAdapter
    {
    private:
        static const u32        maxPad    =    4;
        u32                     getConnectedPadIndex (u32 _padIndex) const;
    public:
                                InputAdapter_x360 (    );
                                ~InputAdapter_x360(    );

        HRESULT                 getDXPadButton(u32 _pad, PressStatus* _buttons, u32 _numButtons);
        void                    getGamePadPos       (  u32 _environment, u32 _pad, float *_pos, u32 _numAxes ) const;
        void                    getGamePadButtons   (  u32 _environment , u32 _pad, PressStatus* _buttons, u32 _numButtons ) const;
        u32                     getGamePadCount     (    );

        void                    onKeyCB( u32 nChar, bbool bKeyDown);

        void                    updateKeyboard();

        void                    updateAllInputState();

        void                    updateMouse();

        void		            startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed);
        void                    stopRumble(u32 _numPad);
        
        InputX360               m_InputX360;

        bool                    m_Keys[256];        // array of key state
        bool                    m_LastKeys[256];    // array of last key state
        bool                    m_MouseButtons[5];  // array of mouse states

        XINPUT_KEYBOARDSTATE    m_keyboardState;
        XINPUT_KEYBOARDSTATE    m_keyboardStatePrevious;
        
        MODIFIER                m_keyboardModifierStatePrevious;

        InputMappingTranslator  m_mappingTranslator;
    };

} // namespace ITF

#endif //_ITF_INPUTADAPTER_X360_H_