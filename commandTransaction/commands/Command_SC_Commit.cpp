#include "precompiled_cmdtr.h"

#ifndef _ITF_COMMAND_SC_COMMIT_H_
    #include "commandTransaction\commands\Command_SC_Commit.h"
#endif // _ITF_COMMAND_SC_COMMIT_H_

namespace ITF
{

Command_SC_Commit::Command_SC_Commit(const ITF_VECTOR<String>* _pPaths, const String& _commiter, u32* _pRevision)
: Command(Type::SC_Commit), m_pPaths(_pPaths), m_Commiter(_commiter), m_pRevision(_pRevision), m_bWaitLastRevision(bfalse)
{
}

void Command_SC_Commit::onEnter()
{
    ITF_ASSERT(SC_ADAPTER != NULL);
    m_RequestID = SC_ADAPTER->commit(*m_pPaths, m_Commiter);
}

bbool Command_SC_Commit::onUpdate()
{
    SCRequestInfo infos;
    if(SC_ADAPTER->pollRequest(m_RequestID, infos))
    {
        m_bSucceed = (infos.getResult() == 0);
        ITF_ASSERT(m_bSucceed);

        // If commit request is over
        if(!m_bWaitLastRevision)
        {
            // If someone is waiting for the last revision value
            // Create an asynchronous request
            if(m_pRevision)
            {
                m_RequestID = SC_ADAPTER->getLastRevision_Prepare();
                m_bWaitLastRevision = btrue;
                return bfalse;
            }
            else
            {
                return btrue;
            }
        }
        else // If getLastRevision request is over
        {
            (*m_pRevision) = SC_ADAPTER->getLastRevision_Finish(infos);
            return btrue;
        }
    }

    return bfalse;
}

void Command_SC_Commit::onExit()
{
}

}