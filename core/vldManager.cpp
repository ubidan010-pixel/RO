#include "precompiled_core.h"

#ifndef _ITF_VLDMANAGER_H_
#include "core/vldManager.h"
#endif // _ITF_VLDMANAGER_H_


namespace ITF
{
#ifdef ITF_VLD_SUPPORTED
    bbool       vldManager::m_bEnableApplication    = btrue;
    funcVLD     vldManager::m_funcEnable            = NULL;
    funcVLD     vldManager::m_funcDisable           = NULL;
#endif //VLD_SUPPORTED
} // namespace ITF


