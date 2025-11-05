////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealinput.h  Engine Abstraction Layer Input Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __EALINPUT_H_INCLUDED
#define __EALINPUT_H_INCLUDED

#include "ealdef.h"
/// This module covers the input reference API.
/*! \addtogroup Input
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210).
/// See ealdef.h for more information.
#define EAL_INPUT_VERSION               212

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Handle to a gamepad
typedef void                            eal_input_gamepad;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Mouse buttons
enum eal_input_mouse_button
{
    EAL_INPUT_MOUSE_BUTTON_LEFT,        ///< Left mouse button
    EAL_INPUT_MOUSE_BUTTON_RIGHT,       ///< Right mouse button
    EAL_INPUT_MOUSE_BUTTON_MIDDLE,      ///< Middle mouse button
    EAL_INPUT_MOUSE_BUTTON_COUNT        ///< Maximum number of mouse buttons supported
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Mouse state

    \note The cursor position is expressed in pixels, and the origin (0,0) is the upper left corner
          of the application window.
*/
struct eal_input_mouse_state
{
    /// Current cursor position on the X axis, in the range [0, MaxCursorX[.
    eal_u32                             CursorX;
    /// Current cursor position on the Y axis, in the range [0, MaxCursorY[.
    eal_u32                             CursorY;
    /// Maximum position on the X axis.
    eal_u32                             MaxCursorX;
    /// Maximum position on the Y axis.
    eal_u32                             MaxCursorY;
    /// State of the mouse buttons, true if pressed, false if unpressed.
    bool                                Buttons[EAL_INPUT_MOUSE_BUTTON_COUNT];
};

/// Keyboard keys
enum eal_input_key
{
    EAL_INPUT_KEY_RESERVED              = 0x00,
    EAL_INPUT_KEY_ERRORROLLOVER         = 0x01,
    EAL_INPUT_KEY_POSTFAIL              = 0x02,
    EAL_INPUT_KEY_ERRORUNDEFINED        = 0x03,
    EAL_INPUT_KEY_A                     = 0x04,
    EAL_INPUT_KEY_B                     = 0x05,
    EAL_INPUT_KEY_C                     = 0x06,
    EAL_INPUT_KEY_D                     = 0x07,
    EAL_INPUT_KEY_E                     = 0x08,
    EAL_INPUT_KEY_F                     = 0x09,
    EAL_INPUT_KEY_G                     = 0x0a,
    EAL_INPUT_KEY_H                     = 0x0b,
    EAL_INPUT_KEY_I                     = 0x0c,
    EAL_INPUT_KEY_J                     = 0x0d,
    EAL_INPUT_KEY_K                     = 0x0e,
    EAL_INPUT_KEY_L                     = 0x0f,
    EAL_INPUT_KEY_M                     = 0x10,
    EAL_INPUT_KEY_N                     = 0x11,
    EAL_INPUT_KEY_O                     = 0x12,
    EAL_INPUT_KEY_P                     = 0x13,
    EAL_INPUT_KEY_Q                     = 0x14,
    EAL_INPUT_KEY_R                     = 0x15,
    EAL_INPUT_KEY_S                     = 0x16,
    EAL_INPUT_KEY_T                     = 0x17,
    EAL_INPUT_KEY_U                     = 0x18,
    EAL_INPUT_KEY_V                     = 0x19,
    EAL_INPUT_KEY_W                     = 0x1a,
    EAL_INPUT_KEY_X                     = 0x1b,
    EAL_INPUT_KEY_Y                     = 0x1c,
    EAL_INPUT_KEY_Z                     = 0x1d,
    EAL_INPUT_KEY_1                     = 0x1e,
    EAL_INPUT_KEY_2                     = 0x1f,
    EAL_INPUT_KEY_3                     = 0x20,
    EAL_INPUT_KEY_4                     = 0x21,
    EAL_INPUT_KEY_5                     = 0x22,
    EAL_INPUT_KEY_6                     = 0x23,
    EAL_INPUT_KEY_7                     = 0x24,
    EAL_INPUT_KEY_8                     = 0x25,
    EAL_INPUT_KEY_9                     = 0x26,
    EAL_INPUT_KEY_0                     = 0x27,
    EAL_INPUT_KEY_RETURN                = 0x28,
    EAL_INPUT_KEY_ESCAPE                = 0x29,
    EAL_INPUT_KEY_BACK                  = 0x2a,
    EAL_INPUT_KEY_TAB                   = 0x2b,
    EAL_INPUT_KEY_SPACE                 = 0x2c,
    EAL_INPUT_KEY_MINUS                 = 0x2d, // '-' and '_'
    EAL_INPUT_KEY_EQUALS                = 0x2e, // '=' and '+'
    EAL_INPUT_KEY_LBRACKET              = 0x2f, // '{' and '['
    EAL_INPUT_KEY_RBRACKET              = 0x30, // '}' and ']'
    EAL_INPUT_KEY_BACKSLASH             = 0x31, // '\' and '|'
    EAL_INPUT_KEY_SHARP                 = 0x32, // '#' and '~' (non-us) OR '[' / '{' for Japan
    EAL_INPUT_KEY_SEMICOLON             = 0x33, // ':' and ';'
    EAL_INPUT_KEY_APOSTROPHE            = 0x34, // ''' and  '"'
    EAL_INPUT_KEY_GRAVE                 = 0x35, // '^' and '~'
    EAL_INPUT_KEY_COMMA                 = 0x36, // ',' and '<'
    EAL_INPUT_KEY_PERIOD                = 0x37, // '.' and '>'
    EAL_INPUT_KEY_SLASH                 = 0x38, // '/' and '?'
    EAL_INPUT_KEY_CAPITAL               = 0x39, // Caps Lock
    EAL_INPUT_KEY_F1                    = 0x3a,
    EAL_INPUT_KEY_F2                    = 0x3b,
    EAL_INPUT_KEY_F3                    = 0x3c,
    EAL_INPUT_KEY_F4                    = 0x3d,
    EAL_INPUT_KEY_F5                    = 0x3e,
    EAL_INPUT_KEY_F6                    = 0x3f,
    EAL_INPUT_KEY_F7                    = 0x40,
    EAL_INPUT_KEY_F8                    = 0x41,
    EAL_INPUT_KEY_F9                    = 0x42,
    EAL_INPUT_KEY_F10                   = 0x43,
    EAL_INPUT_KEY_F11                   = 0x44,
    EAL_INPUT_KEY_F12                   = 0x45,
    EAL_INPUT_KEY_SYSRQ                 = 0x46,
    EAL_INPUT_KEY_SCROLL                = 0x47,
    EAL_INPUT_KEY_PAUSE                 = 0x48,
    EAL_INPUT_KEY_INSERT                = 0x49,
    EAL_INPUT_KEY_HOME                  = 0x4a,
    EAL_INPUT_KEY_PRIOR                 = 0x4b,
    EAL_INPUT_KEY_DELETE                = 0x4c,
    EAL_INPUT_KEY_END                   = 0x4d,
    EAL_INPUT_KEY_NEXT                  = 0x4e,
    EAL_INPUT_KEY_RIGHT                 = 0x4f,
    EAL_INPUT_KEY_LEFT                  = 0x50,
    EAL_INPUT_KEY_DOWN                  = 0x51,
    EAL_INPUT_KEY_UP                    = 0x52,
    EAL_INPUT_KEY_NUMLOCK               = 0x53,
    EAL_INPUT_KEY_DIVIDE                = 0x54,
    EAL_INPUT_KEY_MULTIPLY              = 0x55,
    EAL_INPUT_KEY_SUBTRACT              = 0x56,
    EAL_INPUT_KEY_ADD                   = 0x57,
    EAL_INPUT_KEY_NUMPADENTER           = 0x58,
    EAL_INPUT_KEY_NUMPAD1               = 0x59,
    EAL_INPUT_KEY_NUMPAD2               = 0x5A,
    EAL_INPUT_KEY_NUMPAD3               = 0x5B,
    EAL_INPUT_KEY_NUMPAD4               = 0x5C,
    EAL_INPUT_KEY_NUMPAD5               = 0x5D,
    EAL_INPUT_KEY_NUMPAD6               = 0x5E,
    EAL_INPUT_KEY_NUMPAD7               = 0x5F,
    EAL_INPUT_KEY_NUMPAD8               = 0x60,
    EAL_INPUT_KEY_NUMPAD9               = 0x61,
    EAL_INPUT_KEY_NUMPAD0               = 0x62,
    EAL_INPUT_KEY_NUMPADDELETE          = 0x63, // Delete and '.'
    EAL_INPUT_KEY_SLASHNONUS            = 0x64, // '\' and '|'
    EAL_INPUT_KEY_APPLICATION           = 0x65,
    EAL_INPUT_KEY_POWER                 = 0x66,
    EAL_INPUT_KEY_NUMPADEQUALS          = 0x67,
    EAL_INPUT_KEY_F13                   = 0x68,
    EAL_INPUT_KEY_F14                   = 0x69,
    EAL_INPUT_KEY_F15                   = 0x6A,
    EAL_INPUT_KEY_F16                   = 0x6B,
    EAL_INPUT_KEY_F17                   = 0x6C,
    EAL_INPUT_KEY_F18                   = 0x6D,
    EAL_INPUT_KEY_F19                   = 0x6E,
    EAL_INPUT_KEY_F20                   = 0x6F,
    EAL_INPUT_KEY_F21                   = 0x70,
    EAL_INPUT_KEY_F22                   = 0x71,
    EAL_INPUT_KEY_F23                   = 0x72,
    EAL_INPUT_KEY_F24                   = 0x73,
    EAL_INPUT_KEY_EXECUTE               = 0x74,
    EAL_INPUT_KEY_HELP                  = 0x75,
    EAL_INPUT_KEY_MENU                  = 0x76,
    EAL_INPUT_KEY_SELECT                = 0x77,
    EAL_INPUT_KEY_STOP                  = 0x78,
    EAL_INPUT_KEY_AGAIN                 = 0x79,
    EAL_INPUT_KEY_UNDO                  = 0x7A,
    EAL_INPUT_KEY_CUT                   = 0x7B,
    EAL_INPUT_KEY_COPY                  = 0x7C,
    EAL_INPUT_KEY_PASTE                 = 0x7D,
    EAL_INPUT_KEY_FIND                  = 0x7E,
    EAL_INPUT_KEY_MUTE                  = 0x7F,
    EAL_INPUT_KEY_VOLUMEUP              = 0x80,
    EAL_INPUT_KEY_VOLUMEDOWN            = 0x81,
    EAL_INPUT_KEY_LOCKCAPSLOC           = 0x82,
    EAL_INPUT_KEY_LOCKNUMLOCK           = 0x83,
    EAL_INPUT_KEY_LOCKSCROLLLOCK        = 0x84,
    EAL_INPUT_KEY_NUMPADCOMMA           = 0x85,
    EAL_INPUT_KEY_NUMPADEQUALSIGN       = 0x86,
    EAL_INPUT_KEY_INTERNATIONAL1        = 0x87, // Backslash/underline/ro/broken bar for Japan
    EAL_INPUT_KEY_INTERNATIONAL2        = 0x88, // Katakana/hiragana
    EAL_INPUT_KEY_INTERNATIONAL3        = 0x89, // Yen/vertical bar
    EAL_INPUT_KEY_INTERNATIONAL4        = 0x8A, // Henkan
    EAL_INPUT_KEY_INTERNATIONAL5        = 0x8B, // Muhenkan
    EAL_INPUT_KEY_INTERNATIONAL6        = 0x8C,
    EAL_INPUT_KEY_INTERNATIONAL7        = 0x8D,
    EAL_INPUT_KEY_INTERNATIONAL8        = 0x8E,
    EAL_INPUT_KEY_INTERNATIONAL9        = 0x8F,
    EAL_INPUT_KEY_LANG1                 = 0x90,
    EAL_INPUT_KEY_LANG2                 = 0x91,
    EAL_INPUT_KEY_LANG3                 = 0x92,
    EAL_INPUT_KEY_LANG4                 = 0x93,
    EAL_INPUT_KEY_LANG5                 = 0x94, // Hankaku/zenkaku/kanji for Japan (some model)
    EAL_INPUT_KEY_LANG6                 = 0x95,
    EAL_INPUT_KEY_LANG7                 = 0x96,
    EAL_INPUT_KEY_LANG8                 = 0x97,
    EAL_INPUT_KEY_LANG9                 = 0x98,
    EAL_INPUT_KEY_ALTERNATEERASE        = 0x99,
    EAL_INPUT_KEY_SYSREQ                = 0x9A,
    EAL_INPUT_KEY_CANCEL                = 0x9B,
    EAL_INPUT_KEY_CLEAR                 = 0x9C,
    EAL_INPUT_KEY_PRIOR2                = 0x9D,
    EAL_INPUT_KEY_RETURN2               = 0x9E,
    EAL_INPUT_KEY_SEPARATOR             = 0x9F,
    EAL_INPUT_KEY_OUT                   = 0xA0,
    EAL_INPUT_KEY_OPER                  = 0xA1,
    EAL_INPUT_KEY_CLEARAGAIN            = 0xA2,
    EAL_INPUT_KEY_CRSEL                 = 0xA3,
    EAL_INPUT_KEY_EXSEL                 = 0xA4,
    EAL_INPUT_KEY_RESERVED0             = 0xA5,
    EAL_INPUT_KEY_RESERVED1             = 0xA6,
    EAL_INPUT_KEY_RESERVED2             = 0xA7,
    EAL_INPUT_KEY_RESERVED3             = 0xA8,
    EAL_INPUT_KEY_RESERVED4             = 0xA9,
    EAL_INPUT_KEY_RESERVED5             = 0xAA,
    EAL_INPUT_KEY_RESERVED6             = 0xAB,
    EAL_INPUT_KEY_RESERVED7             = 0xAC,
    EAL_INPUT_KEY_RESERVED8             = 0xAD,
    EAL_INPUT_KEY_RESERVED9             = 0xAE,
    EAL_INPUT_KEY_RESERVED10            = 0xAF,
    EAL_INPUT_KEY_KEYPAD00              = 0xB0,
    EAL_INPUT_KEY_KEYPAD000             = 0xB1,
    EAL_INPUT_KEY_THOUSANDSSEPARATOR    = 0xB2,
    EAL_INPUT_KEY_DECIMALSEPARATOR      = 0xB3,
    EAL_INPUT_KEY_CURRENCYUNIT          = 0xB4,
    EAL_INPUT_KEY_CURRENCYSUBUNIT       = 0xB5,
    EAL_INPUT_KEY_KEYPADLPARENTHESIS    = 0xB6,
    EAL_INPUT_KEY_KEYPADRPARENTHESIS    = 0xB7,
    EAL_INPUT_KEY_KEYPADLBRACKET        = 0xB8,
    EAL_INPUT_KEY_KEYPADRBRACKET        = 0xB9,
    EAL_INPUT_KEY_KEYPADTAB             = 0xBA,
    EAL_INPUT_KEY_KEYPADBACKSPACE       = 0xBB,
    EAL_INPUT_KEY_KEYPADA               = 0xBC,
    EAL_INPUT_KEY_KEYPADB               = 0xBD,
    EAL_INPUT_KEY_KEYPADC               = 0xBE,
    EAL_INPUT_KEY_KEYPADD               = 0xBF,
    EAL_INPUT_KEY_KEYPADE               = 0xC0,
    EAL_INPUT_KEY_KEYPADF               = 0xC1,
    EAL_INPUT_KEY_KEYPADXOR             = 0xC2,
    EAL_INPUT_KEY_KEYPADACCENTGRAVE     = 0xC3,
    EAL_INPUT_KEY_KEYPADPERCENT         = 0xC4,
    EAL_INPUT_KEY_KEYPADGREATERTHAN     = 0xC5,
    EAL_INPUT_KEY_KEYPADLOWERTHAN       = 0xC6,
    EAL_INPUT_KEY_KEYPADAND             = 0xC7,
    EAL_INPUT_KEY_KEYPADCONDITIONNALAND = 0xC8,
    EAL_INPUT_KEY_KEYPADOR              = 0xC9,
    EAL_INPUT_KEY_KEYPADCONDITIONNALOR  = 0xCA,
    EAL_INPUT_KEY_KEYPADCOLON           = 0xCB,
    EAL_INPUT_KEY_KEYPADSHARP           = 0xCC,
    EAL_INPUT_KEY_KEYPADSPACE           = 0xCD,
    EAL_INPUT_KEY_KEYPADAT              = 0xCE,
    EAL_INPUT_KEY_KEYPADEXCLAMATION     = 0xCF,
    EAL_INPUT_KEY_KEYPADMEMORYSTORE     = 0xD0,
    EAL_INPUT_KEY_KEYPADMEMORYRECALL    = 0xD1,
    EAL_INPUT_KEY_KEYPADMEMORTCLEAR     = 0xD2,
    EAL_INPUT_KEY_KEYPADMEMORYADD       = 0xD3,
    EAL_INPUT_KEY_KEYPADMEMORYSUBTRACT  = 0xD4,
    EAL_INPUT_KEY_KEYPADMEMORYMULTIPLY  = 0xD5,
    EAL_INPUT_KEY_KEYPADMEMORYDIVIDE    = 0xD6,
    EAL_INPUT_KEY_KEYPADPLUSMINUS       = 0xD7,
    EAL_INPUT_KEY_KEYPADCLEAR           = 0xD8,
    EAL_INPUT_KEY_KEYPADCLEARENTRY      = 0xD9,
    EAL_INPUT_KEY_KEYPADBINARY          = 0xDA,
    EAL_INPUT_KEY_KEYPADOCTAL           = 0xDB,
    EAL_INPUT_KEY_KEYPADDECIMAL         = 0xDC,
    EAL_INPUT_KEY_KEYPADHEXADECIMAL     = 0xDD,
    EAL_INPUT_KEY_RESERVED11            = 0xDE,
    EAL_INPUT_KEY_RESERVED12            = 0xDF,
    EAL_INPUT_KEY_LCONTROL              = 0xE0,
    EAL_INPUT_KEY_LSHIFT                = 0xE1,
    EAL_INPUT_KEY_LMENU                 = 0xE2, // Left alt
    EAL_INPUT_KEY_LWIN                  = 0xE3, // Windows key
    EAL_INPUT_KEY_RCONTROL              = 0xE4,
    EAL_INPUT_KEY_RSHIFT                = 0xE5,
    EAL_INPUT_KEY_RMENU                 = 0xE6,
    EAL_INPUT_KEY_RWIN                  = 0xE7, // Windows key
    EAL_INPUT_KEY_RESERVED13            = 0xE8,
    EAL_INPUT_KEY_RESERVED14            = 0xE9,
    EAL_INPUT_KEY_RESERVED15            = 0xEA,
    EAL_INPUT_KEY_RESERVED16            = 0xEA,
    EAL_INPUT_KEY_RESERVED17            = 0xEB,
    EAL_INPUT_KEY_RESERVED18            = 0xEC,
    EAL_INPUT_KEY_RESERVED19            = 0xED,
    EAL_INPUT_KEY_RESERVED20            = 0xEE,
    EAL_INPUT_KEY_RESERVED21            = 0xEF,
    EAL_INPUT_KEY_RESERVED22            = 0xF0,
    EAL_INPUT_KEY_RESERVED23            = 0xF1,
    EAL_INPUT_KEY_RESERVED24            = 0xF2,
    EAL_INPUT_KEY_RESERVED25            = 0xF3,
    EAL_INPUT_KEY_RESERVED26            = 0xF4,
    EAL_INPUT_KEY_RESERVED27            = 0xF5,
    EAL_INPUT_KEY_RESERVED28            = 0xF6,
    EAL_INPUT_KEY_RESERVED29            = 0xF7,
    EAL_INPUT_KEY_RESERVED30            = 0xF8,
    EAL_INPUT_KEY_RESERVED31            = 0xF9,
    EAL_INPUT_KEY_RESERVED32            = 0xFA,
    EAL_INPUT_KEY_RESERVED33            = 0xFB,
    EAL_INPUT_KEY_RESERVED34            = 0xFC,
    EAL_INPUT_KEY_RESERVED35            = 0xFD,
    EAL_INPUT_KEY_RESERVED36            = 0xFE,
    EAL_INPUT_KEY_RESERVED37            = 0xFF
};

/// Key modifiers
typedef eal_u32                         eal_input_key_mod;

enum
{
    EAL_INPUT_KEYMOD_NONE               = 0x00000000,
    EAL_INPUT_KEYMOD_LSHIFT             = 0x00000001,
    EAL_INPUT_KEYMOD_RSHIFT             = 0x00000002,
    EAL_INPUT_KEYMOD_LCTRL              = 0x00000004,
    EAL_INPUT_KEYMOD_RCTRL              = 0x00000008,
    EAL_INPUT_KEYMOD_LALT               = 0x00000010,
    EAL_INPUT_KEYMOD_RALT               = 0x00000020,
    EAL_INPUT_KEYMOD_NUM                = 0x00000040,
    EAL_INPUT_KEYMOD_CAPS               = 0x00000080,
    EAL_INPUT_KEYMOD_CTRL               = (EAL_INPUT_KEYMOD_LCTRL | EAL_INPUT_KEYMOD_RCTRL),
    EAL_INPUT_KEYMOD_SHIFT              = (EAL_INPUT_KEYMOD_LSHIFT | EAL_INPUT_KEYMOD_RSHIFT),
    EAL_INPUT_KEYMOD_ALT                = (EAL_INPUT_KEYMOD_LALT | EAL_INPUT_KEYMOD_RALT)
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Initialize the library.
    \param Version      Must be equal to current version = \ref EAL_INPUT_VERSION
    \return true if initialization succeeded

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to EalInputLibInit(), so
      EalInputLibShutdown() is called the exact same number of times.
    - Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - The library will be initialized and all API calls will be allowed.
    - EalInputLibShutdown() must be called even if false is returned.

    \sa EalInputLibShutdown
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
EAL_DLL_ENTRY bool EalInputLibInit(eal_u32 Version = EAL_INPUT_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Shut down the library after usage.
    \PRECONDITIONS
    - Must be called once for every call to EalInputLibInit().

    \POSTCONDITIONS
    - You are not allowed to call any other library member after calling EalInputLibShutdown().

    \sa EalInputLibInit
*/
EAL_DLL_ENTRY void EalInputLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get the mouse button states and cursor position.
    \param MouseState State of the mouse
*/
EAL_DLL_ENTRY void EalInputGetMouseState(eal_input_mouse_state& MouseState);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get a snapshot of the current state of the keyboard.
    \param [out] NumKeys    Number of items stored in the returned buffer.
    \return An array of key states containing NumKeys.

    \POSTCONDITIONS
    - The returned buffer is handled by the implementation and should never be freed by the user.
*/
EAL_DLL_ENTRY eal_u8* EalInputGetKeyboardState(eal_s32& NumKeys);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get the state of modifier keys.
    \return A mask of the modifier keys currently active, see \ref eal_input_key_mod.
*/
EAL_DLL_ENTRY eal_input_key_mod EalInputGetKeyboardModState();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get the number of gamepads available.
    \return The number of gamepads currently available.
*/
EAL_DLL_ENTRY eal_u32 EalInputGetNumGamepads();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get the gamepad at a given index.
    \param Index    Index of the gamepad
    \return the gamepad for this index

    \PRECONDITIONS
    - Index is a value between 0 and the result of EalInputGetNumGamepads().
*/
EAL_DLL_ENTRY eal_input_gamepad* EalInputGetGamepad(eal_u32 Index);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get the number of axis available for a given gamepad.
    \param pGamepad     Gamepad for which you wish to retrieve the number of axis.
    \return The number of axis available for the gamepad.

    \PRECONDITIONS
    - pGamepad is a valid gamepad returned by EalInputGetGamepad().
*/
EAL_DLL_ENTRY eal_u32 EalInputGamepadGetNumAxis(eal_input_gamepad* pGamepad);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get the number of buttons available for a given gamepad.
    \param pGamepad     Gamepad for which you wish to retrieve the number of buttons.
    \return The number of buttons for the specified gamepad.

    \PRECONDITIONS
    - pGamepad is a valid gamepad returned by EalInputGetGamepad().
*/
EAL_DLL_ENTRY eal_u32 EalInputGamepadGetNumButtons(eal_input_gamepad* pGamepad);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get the value of an axis for the specified gamepad.
    \param pGamepad     Gamepad for which you wish to retrieve an axis value.
    \param axis         Index of the axis you want to obtain.
    \return the requested axis value in the range [-32768, 32767]

    \PRECONDITIONS
    - pGamepad is a valid gamepad returned by EalInputGetGamepad().
    - Axis is a value between 0 and the result of EalInputGamepadGetNumAxis().
*/
EAL_DLL_ENTRY eal_s16 EalInputGamepadGetAxis(eal_input_gamepad* pGamepad, eal_s32 axis);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Get the value of a button for the specified gamepad.
    \param  pGamepad    Gamepad for which you wish to retrieve an axis value.
    \param  button      Index of the button.
    \return The state of the requested button, 1 if button is down, 0 otherwise.

    \PRECONDITIONS
    - pGamepad is a valid gamepad returned by EalInputGetGamepad().
*/
EAL_DLL_ENTRY eal_u8 EalInputGamepadGetButton(eal_input_gamepad* pGamepad, eal_s32 button);


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL input functions.
struct eal_input_dll_interface
{
    bool                (*pEalInputLibInit)             (eal_u32 Version);
    void                (*pEalInputLibShutdown)         ();
    void                (*pEalInputGetMouseState)       (eal_input_mouse_state& MouseState);
    eal_u8*             (*pEalInputGetKeyboardState)    (eal_s32& NumKeys);
    eal_input_key_mod   (*pEalInputGetKeyboardModState) ();
    eal_u32             (*pEalInputGetNumGamepads)      ();
    eal_input_gamepad*  (*pEalInputGetGamepad)          (eal_u32 Index);
    eal_u32             (*pEalInputGamepadGetNumAxis)   (eal_input_gamepad* pGamepad);
    eal_u32             (*pEalInputGamepadGetNumButtons)(eal_input_gamepad* pGamepad);
    eal_s16             (*pEalInputGamepadGetAxis)      (eal_input_gamepad* pGamepad, eal_s32 axis);
    eal_u8              (*pEalInputGamepadGetButton)    (eal_input_gamepad* pGamepad, eal_s32 btn);
};

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initialization code and must be done before any
    call to any EAL function.
    \param Interface    Structure containing valid pointers to the engine EAL functions.
    \sa EalFileDllPopulateInterface
*/
void EalInputDllInitInterface(const eal_input_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Only defined on the engine side. It is used to fill up the Interface structure.

    This function is provided by the ealdll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so, it will dynamically connect it's internal Eal
    function calls to the one in your engine.
    \param Interface    A structure that will be filled up with pointers to local EAL functions.
    \sa EalFileDllInitInterface
*/
void EalInputDllPopulateInterface(eal_input_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */

#endif // #ifdef __EALINPUT_H_INCLUDED
