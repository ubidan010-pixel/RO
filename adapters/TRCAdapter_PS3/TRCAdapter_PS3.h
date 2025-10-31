#ifndef _ITF_TRCADAPTER_PS3_H_
#define _ITF_TRCADAPTER_PS3_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_TASKMANAGER__H_
#include "engine/taskmanager/TaskManager.h"
#endif //_ITF_TASKMANAGER__H_


namespace ITF
{
//#define TRC_USE_SYSTEM_MSG      1

    class TRCManagerAdapter_PS3 : public TRCManagerAdapter
    {
    protected:
        virtual bbool _buildAndAddMessage(ErrorContext errorContext);
        Thread* _thread;
    public:

        typedef void (*OnIOErrorOnMainThreadCB)(u64 _userData);

        static void setOnIoErrorOnMainThreadCB(OnIOErrorOnMainThreadCB _cb, u64 _usrData) { ms_ioErrorOnMainCB = _cb; ms_ioErrorOnMainCBUsrData = _usrData;}

        bbool checkSpaceDisk(ErrorContext ErrorContext, u32 kbytes);
        void updateFakeFreeSpace(i32 spaceToAddInKB);
        bbool checkIOError(ErrorContext errorContext, i32 & _errorCode);
        void checkExitCode(i32& _exitCode);

        // states of the IO thread can be asked there in order to wait for them to finish or not
        enum IOThread
        {
            IO_THREAD_RESOURCE_MGR = 0,
            IO_THREAD_SCENE_MGR,
            IO_THREAD_LOADING_SEQ,
            IO_THREAD_NB_MAX
        };
        bbool isIOThreadBlocked(IOThread _ioThread) const;

        bbool _openUISystemMessage(TRCMessage_Critical* pMessage);
        void onFirstTRCFrame();

        bbool isBootRequirementsFinished();

        TRCManagerAdapter_PS3();
        virtual        ~TRCManagerAdapter_PS3();
        virtual void init();
        virtual void term();
        virtual void update();

        void ReCreateSaveFilesAfterCorruption();
        void checkForBootSpaceDisk();
    private:
        // ----------------------------------------------------------------
        // Boot requirements
        // R224: After application boot, the title immediately checks available space in the HDD; when there is not enough available space 
        // to store all data required for game progress, the title displays an appropriate error message and does not allow game progress
        // 
        enum BootRequirement
        {
            Boot_KeyFileCheck = 0,
            Boot_RewardsSpaceDisk,
            Boot_SaveDataSpaceDisk, 
            Boot_GameDataSpaceDisk,
            Boot_GameSpecificDataSpaceDisk,
            Boot_Requirements_Count    // LAST
        };

        ITF_INLINE void SetBootRequiredChecked(BootRequirement bootRequirement) { _bootRequirementStatus[(u32)bootRequirement] = btrue; }        
        bbool   _bootRequirementStatus[Boot_Requirements_Count];
        bbool   _bootRequirementTested;

        


        enum BootTasks
        {
            BootTask_None = 0,
            BootTask_KeyFileCheck,
            BootTask_Save,
            BootTask_Rewards,
            BootTask_Last
        };
        u32 _currentBootTask;

        static i32  startBootTask(void* pParam);
        static void onBootTaskFinish(void* pParam, i32 _result);
        TaskManager _taskManager;

        volatile bbool m_ioThreadBlocked[IO_THREAD_NB_MAX];

        bbool checkKeyFile();

        bbool _keyFileCheckSuccess;

        static OnIOErrorOnMainThreadCB ms_ioErrorOnMainCB;
        static u64 ms_ioErrorOnMainCBUsrData;
    };

    class TRCMessage_SavCorruptedFile_PS3 : public TRCMessage_TwoButton
    {
    public:
        TRCMessage_SavCorruptedFile_PS3(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
            : TRCMessage_TwoButton(errorContext)
        {}

        void onLeftButtonAction();
        void onRightButtonAction();
    };

	#define TRC_ADAPTER_PS3             (static_cast< TRCManagerAdapter_PS3* > (TRC_ADAPTER))

} // namespace ITF

#endif //_ITF_TRCADAPTER_PS3_H_