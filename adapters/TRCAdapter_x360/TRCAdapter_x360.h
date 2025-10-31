#ifndef _ITF_TRCADAPTER_X360_H_
#define _ITF_TRCADAPTER_X360_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

namespace ITF
{
    class TRCManagerAdapter_x360 : public TRCManagerAdapter
    {
    protected:
        bbool _buildAndAddMessage(ErrorContext errorContext);
    
    public:
        TRCManagerAdapter_x360(): TRCManagerAdapter() {}
        ~TRCManagerAdapter_x360(){}
        
        void	init();
        void    term(){}

        
    };


    class TRCMessage_DeconnectionDuringGamePlay_X360 : public TRCMessage_OneButtonWithCB
    {
    public:
        TRCMessage_DeconnectionDuringGamePlay_X360(TRCMessage_Callback_Template* pCallback, 
            TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
            : TRCMessage_OneButtonWithCB(pCallback, errorContext)
        {}

        virtual ~TRCMessage_DeconnectionDuringGamePlay_X360(){}
        virtual bbool	start();
    };


    class TRCMessage_NoAvailableDevice_X360 : public TRCMessage_TwoButton
    {
    public:
        TRCMessage_NoAvailableDevice_X360(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
            : TRCMessage_TwoButton(errorContext)
        {}

        void onLeftButtonAction();
        void onRightButtonAction();
    };


    class TRCMessage_UserNotSignedIn_X360 : public TRCMessage_TwoButton
    {
    public:
        TRCMessage_UserNotSignedIn_X360(u32 currentPlayerIndex, TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
            : _currentPlayerIndex(currentPlayerIndex), TRCMessage_TwoButton(errorContext)
        {}

        virtual void update();
    private:
        u32 _currentPlayerIndex;
    };


    class TRCMessage_SavDeviceNoMoreAvailableForSaving_X360 : public TRCMessage_TwoButton
    {
    public:
        TRCMessage_SavDeviceNoMoreAvailableForSaving_X360(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
            : TRCMessage_TwoButton(errorContext)
        {}

        void onLeftButtonAction();
        void onRightButtonAction();
    };

    class TRCMessage_SavOverWriteSaving_X360 : public TRCMessage_TwoButton
    {
    public:
        TRCMessage_SavOverWriteSaving_X360(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
            : TRCMessage_TwoButton(errorContext)
        {}

        void onLeftButtonAction();
        void onRightButtonAction();
    };

    class TRCMessage_SavErrorDuringLoad_X360 : public TRCMessage_TwoButton
    {
    public:
        TRCMessage_SavErrorDuringLoad_X360(TRCManagerAdapter::ErrorContext errorContext = TRCManagerAdapter::GenericContexte)
            : TRCMessage_TwoButton(errorContext)
        {}

        void onLeftButtonAction();
        void onRightButtonAction();
    };
	#define TRC_ADAPTER_X360		(static_cast< TRCManagerAdapter_x360* > (TRC_ADAPTER))

} // namespace ITF

#endif //_ITF_TRCADAPTER_X360_H_