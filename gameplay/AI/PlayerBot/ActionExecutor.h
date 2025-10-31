#ifndef _RAY_ACTIONEXECUTOR_H_
#define _RAY_ACTIONEXECUTOR_H_

#ifdef ITF_SUPPORT_BOT_AUTO
namespace ITF
{
    struct BotAction;
    class InputAdapter;

    struct InputEvent
    {
        enum Type
        {
            Type_Press = 0,
            Type_Release,
            Type_Hold,
            Type_Wait
        };

        Type type;
        u32 buttonIndex;
        f32 duration;
        f32 timestamp;

        InputEvent()
            : type(Type_Press)
            , buttonIndex(0)
            , duration(0.0f)
            , timestamp(0.0f)
        {}
    };

    struct InputSequence
    {
        SafeArray<InputEvent> events;
        bool loop;

        InputSequence()
            : loop(false)
        {}
    };

    class ActionExecutor
    {
    public:
        ActionExecutor(InputAdapter* inputAdapter);
        ~ActionExecutor();

        void executeAction(const BotAction& action, f32 deltaTime);

        void pressButton(u32 buttonIndex);
        void releaseButton(u32 buttonIndex);
        void setAxisValue(u32 axisIndex, f32 value);

        void queueInputSequence(const InputSequence& sequence);
        void clearInputQueue();

        void update(f32 deltaTime);

        void setMinButtonHoldTime(f32 time) { m_minButtonHoldTime = time; }
        void setMinButtonReleaseTime(f32 time) { m_minButtonReleaseTime = time; }

    private:
        InputAdapter* m_inputAdapter;
        SafeArray<InputEvent> m_inputQueue;

        static const u32 MAX_BUTTONS = 32;
        bool m_buttonStates[MAX_BUTTONS];
        f32 m_buttonHoldTimers[MAX_BUTTONS];

        static const u32 MAX_AXES = 8;
        f32 m_axisStates[MAX_AXES];

        f32 m_minButtonHoldTime;
        f32 m_minButtonReleaseTime;

        void mapActionToInputs(const BotAction& action);
        void processInputQueue(f32 deltaTime);
        void updateButtonTimers(f32 deltaTime);
    };
}
#endif
#endif
