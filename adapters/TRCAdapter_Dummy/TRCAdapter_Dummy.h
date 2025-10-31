#pragma once

#ifdef ITF_DUMMY_TRC_MANAGER

#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"

#ifdef ITF_DUMMY_ADAPTERS_ASSERT_ON_NOT_IMPLEMENTED
    #define ITF_TRC_NOT_IMPLEMENTED() ITF_ASSERT_MSG(0, "TRC not implemented")
#else
    #define ITF_TRC_NOT_IMPLEMENTED() do {} while(0)
#endif

namespace ITF
{
    class TRCManagerAdapter_Dummy : public TRCManagerAdapter
    {
    public:
        TRCManagerAdapter_Dummy();
        virtual ~TRCManagerAdapter_Dummy() override;
        virtual void term() override;
        virtual bbool _buildAndAddMessage(ErrorContext errorContext) override;
    };

    #define TRC_ADAPTER_DUMMY (static_cast<TRCManagerAdapter_Dummy*>(TRC_ADAPTER))

} // namespace ITF

#endif // ITF_DUMMY_TRC_MANAGER
