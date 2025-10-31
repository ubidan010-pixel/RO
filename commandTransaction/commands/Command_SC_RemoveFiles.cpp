#include "precompiled_cmdtr.h"

#ifndef _ITF_COMMAND_SC_REMOVE_FILES_H_
    #include "commandTransaction\commands\Command_SC_RemoveFiles.h"
#endif // _ITF_COMMAND_SC_REMOVE_FILES_H_

namespace ITF
{

    Command_SC_RemoveFiles::Command_SC_RemoveFiles(ITF_VECTOR<String>* _pPaths)
    : Command(Type::SC_RemoveFiles)
    {
        ITF_ASSERT(_pPaths);
        m_RequestID = SC_ADAPTER->remove(*_pPaths);
    }

    void Command_SC_RemoveFiles::onEnter()
    {
    }

    bbool Command_SC_RemoveFiles::onUpdate()
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

    void Command_SC_RemoveFiles::onExit()
    {
    }

}