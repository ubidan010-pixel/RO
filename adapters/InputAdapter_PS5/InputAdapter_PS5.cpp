#include "precompiled_inputadapter_PS5.h"

#include "adapters/InputAdapter_PS5/InputAdapter_PS5.h"


#include "adapters/SystemAdapter_PS5/SystemAdapter_PS5.h"

#include "InputJoy_DualSense_PS5.h"

#include "core/math/vec2d.h"

#include "core/System/Thread.h"
#include "core/utility.h"

#ifndef _ITF_INPUT_MANAGER_H_
#include "engine/input/InputManager.h"
#endif

#ifndef _ITF_CONTROLLER_INPUT_SOURCE_H_
#include "engine/input/ControllerInputSource.h"
#endif

#ifndef _ITF_INPUT_MAPPING_DEFAULTS_H_
#include "engine/input/InputMappingDefaults.h"
#endif

#include <libsysmodule.h>
#include <user_service.h>
#include <algorithm>

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    // joypad port management class
    
    InputAdapter_PS5::JoyPadPort::~JoyPadPort()
    {
        close();
    }

    bbool InputAdapter_PS5::JoyPadPort::open(SceUserServiceUserId _userID, i32 _id )
    {
        if(!isOpened())
        {
            m_id = _id;
            m_dsPort = scePadOpen(_userID, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);
            ITF_ASSERT_SCE_RESULT_NAMED(m_dsPort, "scePadOpen");

            if (isOpened())
                checkConnection();
        }
        return isOpened();
    }

    bbool InputAdapter_PS5::JoyPadPort::checkConnection()
    {
        if(isOpened())
        {
            bbool wasConnected = isConnected();

            if (m_dsPort>=0)
            {
                ScePadControllerInformation controllerInfo;
                ITF_VERIFY_SCE_CALL(scePadGetControllerInformation(m_dsPort, &controllerInfo));
                m_dsConnected = controllerInfo.connected;
            }
            else
                m_dsConnected = bfalse;

             // create or delete the joypad
            if (wasConnected && !isConnected())
            {
                destroyJoyPad();
            }
            if (!wasConnected && isConnected())
            {
                createJoyPad();
            }
        }
        else
        {
            m_dsConnected = bfalse;
        }

        return isConnected();
    }

    bbool InputAdapter_PS5::JoyPadPort::createJoyPad()
    {
        ITF_ASSERT(isConnected());
        InputJoy_PS5::JoyType chosenType = InputJoy_PS5::JOY_TYPE_NONE;

        if (m_dsConnected)
            chosenType = InputJoy_PS5::JOY_TYPE_DUALSENSE;
        
        // create device object
        i32 joyPort = -1;
        if (chosenType == InputJoy_PS5::JOY_TYPE_DUALSENSE)
        {
            m_joyConnected = newAlloc(MemoryId::mId_GameplayInputCriteria, InputJoy_DualSense);
            joyPort = m_dsPort;
        }

        if (m_joyConnected)
        {
            m_joyConnected->init(joyPort, m_id);
            m_joyConnected->updateInputState();
            return btrue;
        }
        return bfalse;
    }

    void InputAdapter_PS5::JoyPadPort::destroyJoyPad()
    {
        ITF_ASSERT(m_joyConnected);
        m_joyConnected->done();
        SF_DEL(m_joyConnected);
    }

    void InputAdapter_PS5::JoyPadPort::close()
    {
        if (isConnected())
            destroyJoyPad();

        if (m_dsPort>=0)
        {
            ITF_VERIFY_SCE_CALL(scePadClose(m_dsPort));
        }
        m_dsPort = -1;
        m_dsConnected = bfalse;
    }

    InputJoy_PS5::JoyType InputAdapter_PS5::JoyPadPort::getType() const
    {
        if (isConnected())
        {
            return getJoyPad()->getType();
        }
        return InputJoy_PS5::JOY_TYPE_NONE;
    }

    i32 InputAdapter_PS5::JoyPadPort::getPort(InputJoy_PS5::JoyType _type) const
    {
        if (_type == InputJoy_PS5::JOY_TYPE_DUALSENSE)
            return m_dsPort;
        return -1;
    }

    bbool InputAdapter_PS5::JoyPadPort::isOpened() const
    {
        return m_dsPort >= 0;
    }

    bbool InputAdapter_PS5::JoyPadPort::isConnected() const
    {
        return m_dsConnected;
    }

    InputJoy_PS5* InputAdapter_PS5::JoyPadPort::getJoyPad() const
    {
        ITF_ASSERT((m_joyConnected!=nullptr) == isConnected());
        return m_joyConnected;
    }

    InputAdapter_PS5::InputAdapter_PS5()
    {
        fill(m_usersIDs.userId, SCE_USER_SERVICE_USER_ID_INVALID);

        ITF_VERIFY_SCE_CALL(scePadInit());
        InitializeActionStrings();
        InitializeInputManager();
        InputAdapter::LoadPlayerControlSettings();
    }

    /// Destructor.
    InputAdapter_PS5::~InputAdapter_PS5()
    {
        for(ux i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++)
        {
            m_joyPadPort[i].close();
        }

        ITF_VERIFY_SCE_CALL(sceUserServiceTerminate());
    }

    void InputAdapter_PS5::getGamePadPos(u32 _environment,u32 _pad, float *_pos, u32 _numAxes) const
    {
        if (!isConnected(_pad))
            return;

        InputJoy_PS5 * joyPad = m_joyPadPort[_pad].getJoyPad();

        if( (m_environmentInput & _environment) != 0 ) //This environment is enable ,fill the buffer
        {
            for (u32 i = 0 ; i < _numAxes ; i++)
                _pos[i] = joyPad->getAxe(i);
        }
        else
        {
            for (u32 i = 0 ; i < _numAxes ; i++)
                _pos[i] = 0.f;
        }
    }

    void InputAdapter_PS5::getGamePadButtons( u32 _environment ,u32 _pad, PressStatus* _buttons, u32 _numButtons ) const
    {
        ITF_ASSERT(_numButtons <= JOY_MAX_BUT);

        if ((m_environmentInput & _environment) != 0 && _pad < getMaxPadCount()) //This environment is enable ,fill the buffer
        {
            if(m_joyPadPort[_pad].isConnected())
            {
                InputJoy_PS5* joyPad = m_joyPadPort[_pad].getJoyPad();

                for (u32 i = 0; i < _numButtons; i++)
                    _buttons[i] = joyPad->getButton(i);
                return;
             }
        }
        
        for (u32 i = 0; i < _numButtons; i++)
            _buttons[i] = Released;
    }

    u32 InputAdapter_PS5::getGamePadCount()
    {
        return SCE_USER_SERVICE_MAX_LOGIN_USERS;
    }

    SceUserServiceUserId InputAdapter_PS5::getUserServiceIdFromPadIndex( u32 _pad ) const
    {
        ITF_ASSERT_MSG( _pad >= 0 && _pad < getMaxPadCount(), "Invalid pad index!");
        return m_usersIDs.userId[_pad];
    }

    void InputAdapter_PS5::updateAllInputState()
    {
        InputAdapter::updateAllInputState();
    }

    void InputAdapter_PS5::UpdatePads()
    {
        checkNewConnections();

        for (u32 pad = 0; pad < SCE_USER_SERVICE_MAX_LOGIN_USERS; ++pad)
        {
            bool isEffectivelyConnected = false;
            if (m_joyPadPort[pad].isConnected())
            {
                JoyPadPort& joyPadPort = m_joyPadPort[pad];
                InputJoy_PS5* joyPad = joyPadPort.getJoyPad();
                if (joyPad)
                {
                    joyPad->updateInputState();
                    m_Joys[pad] = joyPad;
                    isEffectivelyConnected = joyPad->isConnected();

                    if (isEffectivelyConnected)
                    {
                        PadType padType = Pad_PS5;
                        setPadType(pad, padType);
                        resetOrientation(pad);
                    }
                }

                if (!isEffectivelyConnected)
                {
                    joyPadPort.checkConnection();
                }
            }

            setPadConnected(pad, isEffectivelyConnected);
        }

        SyncInputManagerControllers();
    }

    void InputAdapter_PS5::startRumble( u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed )
    {
        if (!isConnected(_numPad))
            return;
        m_joyPadPort[_numPad].getJoyPad()->padVibration(_leftMotorSpeed, _rightMotorSpeed, _time);
    }

    void InputAdapter_PS5::stopRumble( u32 _numPad )
    {
        if (!isConnected(_numPad))
            return;
        m_joyPadPort[_numPad].getJoyPad()->padVibration(0.0f,0.0f,0.0);
    }

    bbool InputAdapter_PS5::isConnected(u8 _pad) const
    {
        ITF_ASSERT(_pad < SCE_USER_SERVICE_MAX_LOGIN_USERS);
        if(_pad >= SCE_USER_SERVICE_MAX_LOGIN_USERS)
            return bfalse;
        return m_joyPadPort[_pad].isConnected();
    }

    void InputAdapter_PS5::getPadAcc( u32 _numPad, Vec3d & _acc)
    {
        if (!isConnected(_numPad))
        {
            _acc.set(0.f, 0.f, 0.f);
            return;
        }

        ITF_ASSERT(m_joyPadPort[_numPad].getJoyPad());
        m_joyPadPort[_numPad].getJoyPad()->getAcceleration(_acc);
    }

    Vec3d InputAdapter_PS5::getPadAngles(u32 _numPad)
    {
        if (!isConnected(_numPad))
            return Vec3d::Zero;

        ITF_ASSERT(m_joyPadPort[_numPad].getJoyPad());
        return m_joyPadPort[_numPad].getJoyPad()->getAngles();
    }

    void InputAdapter_PS5::resetOrientation(u32 _numPad)
    {
        if(isConnected(_numPad))
        {
            ITF_ASSERT(m_joyPadPort[_numPad].getJoyPad());
            m_joyPadPort[_numPad].getJoyPad()->resetOrientation();
        }
    }

    void InputAdapter_PS5::getLowLevelAccels(i32 _numPad, ITF_VECTOR<Vec3d>& _accels, bbool _invertX, bbool _invertY, bbool _invertZ, f32 _maxLimit) const
    {
        if (!isConnected(_numPad))
        {
            Vec3d acc;
            acc.set(0.f, 0.f, 0.f);
            _accels.push_back(acc);
            return;
        }
        ITF_ASSERT(m_joyPadPort[_numPad].getJoyPad());
        m_joyPadPort[_numPad].getJoyPad()->getLowLevelAccels(_accels, _invertX, _invertY, _invertZ, _maxLimit);
    }

    InputJoy_PS5::JoyType InputAdapter_PS5::getJoyType(u32 _numPad)
    {
        if (isConnected(_numPad))
            return m_joyPadPort[_numPad].getType();

        return InputJoy_PS5::JOY_TYPE_NONE;
    }

    void InputAdapter_PS5::loginUser(i32 _idx, i32 _userID)
    {
        ITF_ASSERT(_idx < SCE_USER_SERVICE_MAX_LOGIN_USERS);
        if (_idx >= SCE_USER_SERVICE_MAX_LOGIN_USERS)
            return;
        // remember userID
        m_usersIDs.userId[_idx] = _userID;
        // $GB: Todo (for sound sent to pad ?)
        // SOUND_ADAPTER_PS5->loginUser(_idx, _userID);
    }

    void InputAdapter_PS5::logoutUser(i32 _idx, i32 _userID)
    {
        ITF_ASSERT(_idx < SCE_USER_SERVICE_MAX_LOGIN_USERS);
        if (_idx >= SCE_USER_SERVICE_MAX_LOGIN_USERS || _idx >= ITF_ARRAY_SIZE(m_joyPadPort))
            return;
        m_usersIDs.userId[_idx] = SCE_USER_SERVICE_USER_ID_INVALID;
        m_joyPadPort[_idx].close();
        // $GB: Todo (for sound sent to pad ?)
        // SOUND_ADAPTER_ORBIS->logoutUser(_idx);
    }

    void InputAdapter_PS5::checkNewConnections()
    {
        for(ux i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++)
        {
            if(m_usersIDs.userId[i] == SCE_USER_SERVICE_USER_ID_INVALID)
                continue;

            JoyPadPort & padPort = m_joyPadPort[i];

            if (!padPort.isOpened())
                padPort.open(m_usersIDs.userId[i], i );

            if (!padPort.isConnected())
                padPort.checkConnection();
        }
    }

    u32 InputAdapter_PS5::getConnectedPadIndex(u32 _padIndex) const
    {
        return 0;
    }

    void InputAdapter_PS5::SyncInputManagerControllers()
    {
        if (!m_inputManagerInitialized || !m_inputManager || !m_inputManager->IsInitialized())
            return;

        InputManager* inputManager = m_inputManager;

        for (u32 padIndex = 0; padIndex < SCE_USER_SERVICE_MAX_LOGIN_USERS; ++padIndex)
        {
            bool padConnected = isConnected(static_cast<u8>(padIndex));

            if (padConnected)
            {
                ControllerInputSource* controllerSource = GetOrCreateControllerSource(padIndex);
                if (!controllerSource)
                    continue;
                controllerSource->SetConnected(true);

                InputAdapter::PressStatus buttons[JOY_MAX_BUT]{};
                float axes[JOY_MAX_AXES]{};
                getGamePadButtons(EnvironmentAll, padIndex, buttons, JOY_MAX_BUT);
                getGamePadPos(EnvironmentAll, padIndex, axes, JOY_MAX_AXES);

                for (u32 button = 0; button < JOY_MAX_BUT; ++button)
                {
                    InputAdapter::PressStatus status = buttons[button];
                    bool pressed = (status == Pressed || status == JustPressed || status == Double_Press);
                    controllerSource->SetButtonState(button, pressed);
                }

                for (u32 axis = 0; axis < JOY_MAX_AXES; ++axis)
                {
                    controllerSource->SetAxisState(axis, axes[axis]);
                }
            }
            else
            {
                IInputSource* existing = inputManager->GetInputSource(padIndex);
                if (existing && existing->GetInputType() == PhysicalInput::ControllerButton)
                {
                    static_cast<ControllerInputSource*>(existing)->SetConnected(false);
                }
            }
        }
    }

    ControllerInputSource* InputAdapter_PS5::GetOrCreateControllerSource(u32 padIndex)
    {
        if (!m_inputManager || !m_inputManager->IsInitialized())
            return nullptr;

        IInputSource* existing = m_inputManager->GetInputSource(padIndex);
        if (existing && existing->GetInputType() == PhysicalInput::ControllerButton)
        {
            return static_cast<ControllerInputSource*>(existing);
        }

        auto newSource = std::make_unique<ControllerInputSource>(padIndex);
        ControllerInputSource* controllerSource = newSource.get();
        m_inputManager->RegisterInputSource(std::move(newSource));
        InitializeDefaultControllerMappings(m_inputManager->GetInputMapping(), padIndex);
        return controllerSource;
    }

} // namespace ITF
