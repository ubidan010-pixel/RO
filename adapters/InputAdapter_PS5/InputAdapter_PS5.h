#ifndef ITF_INPUTADAPTER_PS5_H_
#define ITF_INPUTADAPTER_PS5_H_

#pragma once

#include <pad.h>

#include "engine/AdaptersInterfaces/InputAdapter.h"

#include "InputJoy_PS5.h"

#ifdef INPUT_USE_VKEYBOARD
#include "core/UnicodeTools.h"
#include <libime.h>
#endif // INPUT_USE_VKEYBOARD


namespace ITF
{

    class InputAdapter_PS5 : public InputAdapter
    {
    public:
        InputAdapter_PS5();
        ~InputAdapter_PS5();


        virtual u32 getGamePadCount() override;
        virtual void getGamePadPos(u32 _environment, u32 _pad, float* _pos, u32 _numAxes) const override;
        virtual void getGamePadButtons(u32 _environment, u32 _pad, PressStatus* _buttons, u32 _numButtons) const override;
        virtual void startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed) override;
        virtual void stopRumble(u32 _numPad) override;

        SceUserServiceUserId getUserServiceIdFromPadIndex(u32 _pad) const;

        void updateAllInputState() override;

        bbool isConnected(u8 _pad) const;

        u32 getMaxPadCount() const { return SCE_USER_SERVICE_MAX_LOGIN_USERS; }

        void getPadAcc( u32 _numPad, Vec3d & _acc);
        Vec3d getPadAngles(u32 _numPad);
        void resetOrientation(u32 _numPad);

        void getLowLevelAccels(i32 _numPad, Vector<Vec3d>& _accels, bbool _invertX, bbool _invertY, bbool _invertZ, f32 _maxLimit) const;

        InputJoy_PS5::JoyType getJoyType(u32 _numPad);


        void loginUser(i32 _idx, i32 _userID);
        void logoutUser(i32 _idx, i32 _userID);

    protected:
        void checkNewConnections();

    private:

        struct JoyPadPort
        {
            // JoyPadPort use case :
            // first open a port by calling open with a user id
            // -> the resulting object will be opened and connected or not.
            // checkConnection can be call regularly to deal with connection/deconnection
            // getJoyPad is giving the associated joypad interface of the connected pad.

            bbool open(SceUserServiceUserId _userID, i32 _id ); // call ignored if already opened
            bbool checkConnection(); // recheck connexion. return false if not opened or not connected...
            void close();

            bbool isOpened() const;
            bbool isConnected() const;

            InputJoy_PS5::JoyType getType() const;
            i32 getPort(InputJoy_PS5::JoyType _type) const; // return sce id of the open port if positive, the sce error if negative, -1 for unknown JoyType
            InputJoy_PS5* getJoyPad() const; // nullptr if not connected or connected to an unknown device

            ~JoyPadPort();

        private:
            bbool createJoyPad();
            void destroyJoyPad();

            i32 m_dsPort = -1;
            bbool m_dsConnected = bfalse;
            i32 m_id = -1;
            InputJoy_PS5* m_joyConnected = nullptr;
        };

        u32 getConnectedPadIndex (u32 _padIndex) const;

        SceUserServiceLoginUserIdList m_usersIDs;

        InputJoy_PS5* m_Joys[SCE_USER_SERVICE_MAX_LOGIN_USERS];

        JoyPadPort m_joyPadPort[SCE_USER_SERVICE_MAX_LOGIN_USERS];

    };

    #define INPUT_ADAPTER_PS5 (static_cast<InputAdapter_PS5*>(INPUT_ADAPTER))

} // namespace ITF

#endif //ITF_INPUTADAPTER_PS5_H_
