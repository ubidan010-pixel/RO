#ifndef _ITF_TESTMANAGER_H_
#define _ITF_TESTMANAGER_H_

namespace ITF
{
    class StateTest 
    {

    public:

        enum StateTestStatus
        {
            onENTER     = 0,
            onUPDATE    = 1,
            onEXIT      = 2,
            ENUM_FORCE_SIZE_32(0)
        };

        StateTest():m_State(onENTER)  {};
        virtual ~StateTest()  {};
        virtual void onEnter() = 0;
        virtual void onExit() = 0;
        virtual void update(f32 _dt) = 0;

    protected:
        StateTestStatus m_State;
    };


    //TELEPORTATION TEST and move to the next checkpoint and back to first..
    class TeleportStateTest :public  StateTest
    {
        virtual ~TeleportStateTest() {};
        virtual void onEnter();
        virtual void onExit();
        virtual void update(f32 _dt);
        
    protected:
        Vec3d   m_Direction;
        Vec3d   m_StartingPos;
        Vec3d   m_EndingPos;
        f32     m_fTimeToReach;
        Vec3d   m_DirFrame;
        ObjectRef m_ActorID;

    };

    class CameraRunStateTest :public  StateTest
    {
    public:
        CameraRunStateTest();
        virtual         ~CameraRunStateTest() {};
        virtual void    onEnter();
        virtual void    onExit();
        virtual void    update(f32 _dt);
        static  void    startNewTest( bbool loop = true, bbool waitAtEndOfWorld = true );
        static  bbool   isRunning() {return m_isRunning;}
        static  bbool   isAborted() {return m_isAborted;}
        static  void    stopTest();
        
    protected:
        bbool   m_bActive;
        AABB    m_aabb;
        float   m_startZ;
        float   m_curFrameX, m_curFrameY, m_curFrameZ, m_frameIncrX, m_frameIncrY , m_Timer;
        static  bbool m_isRunning;
        f32     m_innerTimer, m_x, m_y;
        static bbool    m_loop;
        static bbool    m_isAborted;

    public:
        static float    m_camSpeedPercent;

    private:
        void    build();
    };

#ifndef ITF_FINAL
    class TestManager
    {
    public:

        TestManager();
        ~TestManager();

        void start(const String& _szName);

        void update(f32 _elapsed);
        void state_teleport();

        void setTest(StateTest* _test) 
        {
            SF_DEL(m_pStateTest);
            m_pStateTest = _test; 
            if (m_pStateTest) 
                m_isRunning = btrue;
        }

    protected:

        StateTest* m_pStateTest;
        bbool m_isRunning;
    };
#endif // ITF_FINAL

}

#endif // _ITF_TESTMANAGER_H_