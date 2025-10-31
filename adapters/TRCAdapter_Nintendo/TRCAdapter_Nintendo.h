#pragma once

#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#include "engine/taskmanager/TaskManager.h"

namespace ITF
{

    class TRCManagerAdapter_Nintendo : public TRCManagerAdapter
    {
    protected:
        virtual void term() override {}
        virtual bbool _buildAndAddMessage(ErrorContext errorContext) override;
    };

    #define TRC_ADAPTER_NINTENDO (static_cast<TRCManagerAdapter_Nintendo*>(TRC_ADAPTER))

} // namespace ITF
