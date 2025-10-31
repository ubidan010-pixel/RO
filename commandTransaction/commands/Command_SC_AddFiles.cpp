#include "precompiled_cmdtr.h"

#ifndef _ITF_COMMAND_SC_ADDFILES_H_
    #include "commandTransaction\commands\Command_SC_AddFiles.h"
#endif // _ITF_COMMAND_SC_ADDFILES_H_

namespace ITF
{

Command_SC_AddFiles::Command_SC_AddFiles(ITF_VECTOR<String> _paths)
: Command(Type::SC_AddFile), m_Paths(_paths)
{
}

void Command_SC_AddFiles::onEnter()
{
    ITF_ASSERT(SC_ADAPTER != NULL);
    ITF_ASSERT(m_Paths.size() > 0);
    m_RequestID = SC_ADAPTER->add(m_Paths);
}

bbool Command_SC_AddFiles::onUpdate()
{
    SCRequestInfo infos;
    if(SC_ADAPTER->pollRequest(m_RequestID, infos))
    {
        m_bSucceed = (infos.getResult() == 0);
        ITF_ASSERT(m_bSucceed);
        return btrue;
    }

    return bfalse;

}

void Command_SC_AddFiles::onExit()
{
    // TODO release m_RequestID ?
}

}