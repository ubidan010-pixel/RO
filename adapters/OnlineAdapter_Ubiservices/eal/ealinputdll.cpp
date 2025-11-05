////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealinputdll.cpp  DLL wrapper implementation for the EAL (input).
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See file ealdef.h for details on how to use the various EAL APIs.
//
// ONLY INCLUDE THIS FILE IN YOUR PROJECT IF YOU ARE USING DLLs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "precompiled_OnlineAdapter_Ubiservices.h"
#include "ealinput.h"

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL Side

static eal_input_dll_interface        gEalInputDllInterface = {};

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalInputDllInitInterface(const eal_input_dll_interface& Interface)
{
    gEalInputDllInterface = Interface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool EalInputLibInit(eal_u32 Version)
{
    return (*gEalInputDllInterface.pEalInputLibInit)(Version);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalInputLibShutdown()
{
    (*gEalInputDllInterface.pEalInputLibShutdown)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EalInputGetMouseState(eal_input_mouse_state& MouseState)
{
    return (*gEalInputDllInterface.pEalInputGetMouseState)(MouseState);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u8* EalInputGetKeyboardState(eal_s32& NumKeys)
{
    return (*gEalInputDllInterface.pEalInputGetKeyboardState)(NumKeys);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_input_key_mod EalInputGetKeyboardModState()
{
    return (*gEalInputDllInterface.pEalInputGetKeyboardModState)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalInputGetNumGamepads()
{
    return (*gEalInputDllInterface.pEalInputGetNumGamepads)();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_input_gamepad* EalInputGetGamepad(eal_u32 Index)
{
    return (*gEalInputDllInterface.pEalInputGetGamepad)(Index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalInputGamepadGetNumAxis(eal_input_gamepad* pGamepad)
{
    return (*gEalInputDllInterface.pEalInputGamepadGetNumAxis)(pGamepad);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

eal_u32 EalInputGamepadGetNumButtons(eal_input_gamepad* pGamepad)
{
    return (*gEalInputDllInterface.pEalInputGamepadGetNumButtons)(pGamepad);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
eal_s16 EalInputGamepadGetAxis(eal_input_gamepad* pGamepad, eal_s32 axis)
{
    return (*gEalInputDllInterface.pEalInputGamepadGetAxis)(pGamepad, axis);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
eal_u8 EalInputGamepadGetButton(eal_input_gamepad* pGamepad, eal_s32 button)
{
    return (*gEalInputDllInterface.pEalInputGamepadGetButton)(pGamepad, button);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#else

////////////////////////////////////////////////////////////////////////////////////////////////////
// Engine Side

void EalInputDllPopulateInterface(eal_input_dll_interface& Interface)
{
    Interface.pEalInputLibInit                  = EalInputLibInit;
    Interface.pEalInputLibShutdown              = EalInputLibShutdown;
    Interface.pEalInputGetMouseState            = EalInputGetMouseState;
    Interface.pEalInputGetKeyboardState         = EalInputGetKeyboardState;
    Interface.pEalInputGetKeyboardModState      = EalInputGetKeyboardModState;
    Interface.pEalInputGetNumGamepads           = EalInputGetNumGamepads;
    Interface.pEalInputGetGamepad               = EalInputGetGamepad;
    Interface.pEalInputGamepadGetNumAxis        = EalInputGamepadGetNumAxis;
    Interface.pEalInputGamepadGetNumButtons     = EalInputGamepadGetNumButtons;
    Interface.pEalInputGamepadGetAxis           = EalInputGamepadGetAxis;
    Interface.pEalInputGamepadGetButton         = EalInputGamepadGetButton;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

