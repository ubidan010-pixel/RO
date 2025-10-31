#ifndef ITF_TRCADAPTER_PS5_H_
#define ITF_TRCADAPTER_PS5_H_

#pragma once

#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#include "engine/taskmanager/TaskManager.h"

namespace ITF
{

    class TRCManagerAdapter_PS5 : public TRCManagerAdapter
    {
    protected:
        virtual void term() override {}
        virtual bbool _buildAndAddMessage(ErrorContext errorContext) override;
    };

    #define TRC_ADAPTER_PS5 (static_cast<TRCManagerAdapter_PS5*>(TRC_ADAPTER))
} // namespace ITF

#endif // ITF_TRCADAPTER_PS5_H_
