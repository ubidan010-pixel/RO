#ifndef _ITF_TRCADAPTER_WII_H_
#define _ITF_TRCADAPTER_WII_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

struct OSThreadQueue;

namespace ITF
{
	#define TRC_ADAPTER_WII		(static_cast< TRCManagerAdapter_WII* > (TRC_ADAPTER))

    class TRCManagerAdapter_WII : public TRCManagerAdapter
    {
    protected:
        bbool _buildAndAddMessage(ErrorContext errorContext);
        
    public:
        TRCManagerAdapter_WII(): TRCManagerAdapter(), m_TRCThreadQueue(NULL) {}
        ~TRCManagerAdapter_WII(){}

        static bbool IOResIsValid(i32 _error);
        
        void	init();
        void    term(){}
        void    update();

        bbool checkSpaceDisk(ErrorContext ErrorContext, u32 fsblock, u32 inode);
        bbool checkIOError(ErrorContext errorContext, i32 & _errorCode);

        bbool  isSaveNotificationEnable() const { return bfalse; }

		static void PowerCallback();
		static void ResetCallback();

		static void RequestExit( ExitValue value );
		static bbool IsShutdownRequested() { return m_exitValue != TRCManagerAdapter::Exit_None; }

		void enableRESETandPOWER()  { ITF::Synchronize::leaveCriticalSection(&m_NANDAccess); }
		void disableRESETandPOWER() { ITF::Synchronize::enterCriticalSection(&m_NANDAccess); }

	private:
		static DWORD Reset_Thread(void *_param)		{return TRC_ADAPTER_WII->ResetThread(_param);}
		static DWORD DVDError_Thread(void *_param)	{return TRC_ADAPTER_WII->DVDErrorThread(_param);}

		DWORD ResetThread(void *_param);
		DWORD DVDErrorThread(void *_param);

		ITF_THREAD_CRITICAL_SECTION m_NANDAccess;

		OSThreadQueue* m_TRCThreadQueue;

		static ExitValue m_exitValue; // Static because it must be valid at the application launch. 
    };

    class TRCMessage_PadDeconnection : public TRCMessage_OneButtonWithCB
    {
    public:
        TRCMessage_PadDeconnection( TRCMessage_Callback_Template* pCallback, TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
            : TRCMessage_OneButtonWithCB(pCallback, errorContext)
        {

        }
        virtual ~TRCMessage_PadDeconnection(){}
        virtual void update();

    };



    class TRCMessage_CorruptedSave_WII : public TRCMessage_TwoButton
    {
    public:
    	TRCMessage_CorruptedSave_WII(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
    		 : TRCMessage_TwoButton(errorContext)
    	{
            
        }
    	virtual ~TRCMessage_CorruptedSave_WII(){}
    	void onLeftButtonAction();
        void onRightButtonAction();
    };
    
    
    class TRCMessage_NoEnoughSpaceOrInode_WII : public TRCMessage_TwoButton
    {
    public:
    	TRCMessage_NoEnoughSpaceOrInode_WII(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
    		 : TRCMessage_TwoButton(errorContext), _lastComponentSelected(NULL)

    	{
            forceName("TRCMessage_Space_WII");
        }
    	virtual ~TRCMessage_NoEnoughSpaceOrInode_WII(){}
    	void onLeftButtonAction();
        void onRightButtonAction();

        virtual void	update();
    private:
        String _workingMessage;
        UIComponent* _lastComponentSelected;
    };
    
    class TRCMessage_ExtensionChange : public TRCMessage_OneButton
    {
    public:
        TRCMessage_ExtensionChange(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte, i32 errorCode = TRCMESSAGE_ERROR_NO)
            : TRCMessage_OneButton(errorContext, errorCode)
        {
        }
        virtual ~TRCMessage_ExtensionChange(){}
        virtual void	update();
    };

    // Debug Task
#ifndef ITF_FINAL
    class TRCTask_DebugExtension : public TRCTask
    {
    public:
    	void run();    
    	void init();
    	
    private:
    	void changeExtension(u32 chan, u32 extension, u32 wpad_dev);
    	bbool extensionStatuts[4][4];
    };
#endif //ITF_FINAL


} // namespace ITF

#endif //_ITF_TRCADAPTER_WII_H_