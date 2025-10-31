#include "precompiled_TRCAdapter_Dummy.h"

#include "adapters/TRCAdapter_Dummy/TRCAdapter_Dummy.h"

namespace ITF
{

    TRCManagerAdapter_Dummy::TRCManagerAdapter_Dummy() = default;
    TRCManagerAdapter_Dummy::~TRCManagerAdapter_Dummy() = default;

    void TRCManagerAdapter_Dummy::term()
    {
        ITF_TRC_NOT_IMPLEMENTED();
    }

    bbool TRCManagerAdapter_Dummy::_buildAndAddMessage(ErrorContext errorContext)
    {
        ITF_TRC_NOT_IMPLEMENTED();
        return bfalse;
    }

} // namespace ITF

