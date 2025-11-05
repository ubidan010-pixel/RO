#pragma once

#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#include "engine/taskmanager/TaskManager.h"

namespace ITF
{

    class TRCManagerAdapter_XBoxSeries : public TRCManagerAdapter
    {
    protected:
        virtual void term() override {}
        virtual bbool _buildAndAddMessage(ErrorContext errorContext) override;
    };

    #define TRC_ADAPTER_XBOXSERIES (static_cast<TRCManagerAdapter_XBoxSeries*>(TRC_ADAPTER))
} // namespace ITF
