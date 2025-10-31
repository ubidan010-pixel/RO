#ifndef _ITF_TRCADAPTER_WIN_H_
#define _ITF_TRCADAPTER_WIN_H_

#pragma once

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

namespace ITF
{
    class TRCManagerAdapter_win : public TRCManagerAdapter
    {
    protected:
        virtual bbool _buildAndAddMessage(ErrorContext errorContext);
    
    public:
        bbool checkSpaceDisk(ErrorContext ErrorContext, u32 bytes);

        TRCManagerAdapter_win() = default;
        ~TRCManagerAdapter_win() = default;
        virtual void init();
        virtual void term(){}
    };


	#define TRC_ADAPTER_WIN		(static_cast< TRCManagerAdapter_win* > (TRC_ADAPTER))

} // namespace ITF

#endif //_ITF_TRCADAPTER_WIN_H_
