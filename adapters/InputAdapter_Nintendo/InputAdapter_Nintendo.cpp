#include "precompiled_inputadapter_Nintendo.h"

#include "core/Macros.h"
#include "core/math/Mathf32.h"

#include "adapters/InputAdapter_Nintendo/InputAdapter_Nintendo.h"

#ifdef ITF_SUPPORT_SIMPLE_PAD
#include "adapters/InputAdapter_Nintendo/InputDebugPad_Nintendo.h"
#endif

#include "adapters/InputAdapter_Nintendo/InputNpad_Nintendo.h"

#include "adapters/SystemAdapter_Nintendo/SystemAdapter_Nintendo.h"

#include "core/math/MathTools.h"

#include <nn/hid.h>
#include <nn/hid/hid_Npad.h>
#include "core/AssertManager.h"

#include <nn/hid/hid_ControllerSupport.h>
#include <nn/hid/hid_NpadJoy.h>

#ifdef INPUT_USE_VKEYBOARD
#include "adapters/InputAdapter_Nintendo/Task_VKeyboard_Nintendo.h"
#endif

#include "adapters/SystemAdapter_Nintendo/SystemAdapter_Nintendo.h"

namespace ITF
{

    InputAdapter_Nintendo::InputAdapter_Nintendo()
        : InputAdapter(),
          m_pads(),
          m_init(false),
          m_gyroResetAnglesHandheld(0.0f, 0.0f, 0.0f),
          m_gyroResetAnglesJoyLeft(0.0f, 0.0f, 0.0f),
          m_gyroResetAnglesJoyRight(0.0f, 0.0f, 0.0f),
          m_showControllerAppletRequested(false),
          m_enableControllerApplet(false),
          m_activePad(nullptr)
    {
        m_pads.reserve(MAX_PAD_COUNT);
    }

    InputAdapter_Nintendo::~InputAdapter_Nintendo()
    {
        for (InputPad_Nintendo*& padPtr : m_pads)
        {
            SF_DEL(padPtr);
        }

        m_pads.clear();
    }

    void InputAdapter_Nintendo::initialize()
    {
        if (m_init)
            return;

        nn::hid::InitializeNpad();

        nn::hid::SetSupportedNpadStyleSet(nn::hid::NpadStyleHandheld::Mask | nn::hid::NpadStyleFullKey::Mask | nn::hid::NpadStyleJoyLeft::Mask |
                                          nn::hid::NpadStyleJoyRight::Mask | nn::hid::NpadStyleJoyDual::Mask);

        nn::hid::NpadIdType supportedNapIds[MAX_PAD_COUNT + 1]{};

        for (u32 joyconInd = 0; joyconInd < MAX_PAD_COUNT; joyconInd++)
        {
            supportedNapIds[joyconInd] = static_cast<nn::hid::NpadIdType>(nn::hid::NpadId::No1 + joyconInd);
        }

        supportedNapIds[MAX_PAD_COUNT] = nn::hid::NpadId::Handheld;

        nn::hid::SetSupportedNpadIdType(supportedNapIds, ITF_ARRAY_SIZE(supportedNapIds));

        // The handheld is only enabled when both the left and right JOY-CON controllers are attached
        nn::hid::SetNpadHandheldActivationMode(nn::hid::NpadHandheldActivationMode_Dual);

        // JOY-CONs are used horizontally
        nn::hid::SetNpadJoyHoldType(nn::hid::NpadJoyHoldType_Horizontal);

        #ifdef ITF_SUPPORT_SIMPLE_PAD
            nn::hid::InitializeDebugPad();
            if (InputDebugPad_Nx::IsAvailable())
            {
                m_pads.push_back(newAlloc(mId_System, InputDebugPad_Nx(m_pads.size())));
            }
        #endif

        m_pads.push_back(new (MemoryId::mId_System) InputNpad_Nintendo(m_pads.size(), nn::hid::NpadId::Handheld));
        for (u32 joyconInd = 0; joyconInd < MAX_PAD_COUNT; joyconInd++)
        {
            m_pads.push_back(new (MemoryId::mId_System) InputNpad_Nintendo(joyconInd, nn::hid::NpadId::No1 + joyconInd));
        }

        m_init = true;
    }

u32 InputAdapter_Nintendo::getGamePadCount()
{
    u32 count = 0;
    for (InputPad_Nintendo * pad : m_pads)
    {
        if (pad != nullptr && pad->isConnected())
            count++;
    }
    return ITF::Min<u32>(count, MAX_PAD_COUNT);
}

void InputAdapter_Nintendo::requestShowControllerApplet()
{
    m_showControllerAppletRequested = true;
}

void InputAdapter_Nintendo::showControllerApplet()
{
    SYSTEM_ADAPTER_NINTENDO->hideStartupSplashScreen();

    // $GB: todo deactivate sound?

    nn::hid::ControllerSupportArg args;
    args.playerCountMin = 0;
    args.playerCountMax = 4;
    args.enableTakeOverConnection = true;
    args.enableLeftJustify = false;
    args.enablePermitJoyDual = true;
    args.enableSingleMode = false;
    args.enableIdentificationColor = false;
    args.enableExplainText = false;
    nn::Result res = nn::hid::ShowControllerSupport(args);
    ITF_UNUSED(res);

    ITF_ASSERT(!nn::hid::ResultControllerSupportNotSupportedNpadStyle::Includes(res));

    m_showControllerAppletRequested = false;
}

void InputAdapter_Nintendo::getGamePadPos(u32 _environment, u32 _pad, float* _pos, u32 _numAxes) const
{
    std::fill(_pos, _pos + _numAxes, 0.f);
    getGamePadPos(_environment, getPad(_pad), _pos, _numAxes);
}

void InputAdapter_Nintendo::getGamePadButtons(u32 _environment, u32 _pad, PressStatus* _buttons, u32 _numButtons) const
{
    std::fill(_buttons, _buttons + _numButtons, InputAdapter::Released);
    getGamePadButtons(_environment, getPad(_pad), _buttons, _numButtons);
}

void InputAdapter_Nintendo::getGamePadPos(u32 _environment, InputPad_Nintendo* _padPtr, float* _pos, u32 _numAxes) const
{
    if (_padPtr != nullptr && _padPtr->isConnected())
    {
        float allAxes[JOY_MAX_AXES];
        _padPtr->getGamePadPos(m_environmentInput & _environment, allAxes, _numAxes);
        for (int i = 0; i < _numAxes; i++)
        {
            if (abs(_pos[i]) < abs(allAxes[i]))
                _pos[i] = allAxes[i];
        }
    }
}

void InputAdapter_Nintendo::getGamePadButtons(u32 _environment, InputPad_Nintendo* _padPtr, PressStatus* _buttons, u32 _numButtons) const
{
    if (_padPtr != nullptr && _padPtr->isConnected())
    {
        for (u32 i = 0; i < _numButtons; i++)
        {
            InputAdapter::PressStatus newStatus = _padPtr->getGamePadButton(_environment, m_buttonMode, i);
            if (_buttons[i] == InputAdapter::Pressed || newStatus == InputAdapter::Pressed)
                _buttons[i] = InputAdapter::Pressed;
            else if (_buttons[i] == InputAdapter::Double_Press || newStatus == InputAdapter::Double_Press)
                _buttons[i] = InputAdapter::Double_Press;
            else if ((_buttons[i] == InputAdapter::JustPressed && newStatus == InputAdapter::JustReleased) ||
                (_buttons[i] == InputAdapter::JustReleased && newStatus == InputAdapter::JustPressed))
                _buttons[i] = InputAdapter::Pressed;
            else if (_buttons[i] == InputAdapter::Released)
                _buttons[i] = newStatus;
        }
    }
}

void InputAdapter_Nintendo::startRumble(u32 _pad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
{
    InputPad_Nintendo* pad = getPad(_pad);
    if (pad != nullptr)
    {
        pad->setVibration(_leftMotorSpeed, _rightMotorSpeed, _time);
    }
}

void InputAdapter_Nintendo::stopRumble(u32 _pad)
{
    InputPad_Nintendo* pad = getPad(_pad);
    if (pad != nullptr)
    {
        pad->stopVibration();
    }
}

void InputAdapter_Nintendo::updateControllers()
{
    InputPad_Nintendo* previousActivePad = m_activePad;

    for (InputPad_Nintendo* padPtr : m_pads)
    {
        padPtr->Update();
    }

    m_activePad = getPad(0);

    if (m_activePad != previousActivePad)
    {
        stopRumble(0);
    }
}

void InputAdapter_Nintendo::updateAllInputState()
{
    updateControllers();
    if (m_showControllerAppletRequested && m_enableControllerApplet)
    {
        showControllerApplet();
        updateControllers();
        m_showControllerAppletRequested = false;  // if new wireless controllers concected during applet, don't consider them
    }
    m_enableControllerApplet = true;

    for (u32 padInd = 0; padInd < MAX_PAD_COUNT; padInd++)
    {
        const InputPad_Nintendo* pad = getPad(padInd);

        bbool wasConnected = isPadConnected(padInd);
        bool isConnected;
        PadType padType;
        if (pad != nullptr)
        {
            isConnected = pad->isConnected();
            padType = pad->GetType();
        }
        else
        {
            isConnected = false;
            padType = Pad_Invalid;
        }

        if (wasConnected != isConnected)
        {
            if (isConnected)
            {
                OnControllerConnected(padInd);
            }
            else
            {
                OnControllerDisconnected(padInd);
            }
        }

        setPadConnected(padInd, isConnected);
        setPadType(padInd, padType);
    }

#ifdef INPUT_USE_VKEYBOARD
    VKeyboardUpdate();
#endif  // INPUT_USE_VKEYBOARD
}

#ifdef INPUT_USE_VKEYBOARD
void InputAdapter_Nx::VKeyboardOpen(const VKeyboardOpenParams& _params)
{
    // ITF_PASTA TODO
    ITF_ASSERT(false);

    // ITF_ASSERT_MSG(!m_VKeyboardTask.isValid(), "trying to open virtual keyboard more than once");

    // if (!m_VKeyboardTask.isValid())
    //{
    //     InputAdapter::VKeyboardOpen(_params);

    //    m_VKeyboardTask = newAlloc(mId_System, Task_VKeyboard_NX(_params, VKeyboardGetText()));
    //    m_VKeyboardTask->start();

    //    // set output string to default value
    //    VKeyboardSetText(_params.m_textDefault);
    //}
}

void InputAdapter_Nx::VKeyboardUpdate()
{
    INPUT_IMPLEM(  // ITF_PASTA
        InputAdapter::VKeyboardUpdate();

        if (m_VKeyboardTask.isValid() && m_VKeyboardTask->isFinalized()) {
            // set output string to entered value, if any
            const bbool cancelled = m_VKeyboardTask->hasFailed();

            if (!cancelled)
            {
                VKeyboardSetText(m_VKeyboardTask->getText());
            }

            m_VKeyboardTask.invalidate();

            VKeyboardClose(cancelled);
        })
    INPUT_BOUCHON(

    )
}
#endif  // INPUT_USE_VKEYBOARD


bbool InputAdapter_Nintendo::isHandheld()
{
    return m_pads[0]->isConnected();
}

InputPad_Nintendo* InputAdapter_Nintendo::getPad(u32 _padIndex) const
{
    InputPad_Nintendo* handledPad = m_pads[0];

    if (handledPad == nullptr || !handledPad->isConnected())
    {
        _padIndex++;
    }

    if (_padIndex < m_pads.size())
    {
        return m_pads[_padIndex];
    }
    ITF_ASSERT_MSG(0, "Pad index %d is too high", _padIndex);
    return nullptr;
}

void InputAdapter_Nintendo::swapControllers(u32 index1, u32 index2)
{
    u32 internal1 = index1;
    u32 internal2 = index2;

    InputPad_Nintendo* handledPad = m_pads[0];
    if (handledPad == nullptr || !handledPad->isConnected())
    {
        internal1++;
        internal2++;
    }

    if (internal1 < m_pads.size() && internal2 < m_pads.size())
    {
        std::swap(m_pads[internal1], m_pads[internal2]);
    }
}

}  // namespace ITF
