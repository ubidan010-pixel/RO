#include "precompiled_engine.h"

#ifndef _ITF_COMMANDLIST_H_
#include "engine/commandlist/CommandList.h"
#endif //_ITF_COMMANDLIST_H_

namespace ITF
{

CommandList::CommandList()
{
    reset();
}

CommandList::~CommandList() 
{
    reset();
}

bbool CommandList::update()
{
    // If this is the first time "update" is called
    if(-1 == m_currentCommandIndex)
    {
        // Directly return if no command
        if(m_commandList.size() == 0)
        {
            return btrue;
        }

        m_currentCommandIndex = 0;
        m_firstCommandUpdate = btrue;
    }

    // Get the command pointer
    Command *pCmd = m_commandList[m_currentCommandIndex];
    ITF_ASSERT(pCmd != NULL);

    if(m_firstCommandUpdate)
    {
        pCmd->onEnter();
        m_firstCommandUpdate = bfalse;
    }

    if(pCmd->onUpdate())
    {
        pCmd->onExit();

        ITF_ASSERT(pCmd->succeeded());

        // If the next command exists
        if((i32(m_commandList.size()) - 1) > m_currentCommandIndex)
        {
            m_currentCommandIndex++;
            m_firstCommandUpdate = btrue;
        }
        else
        {
            // No more commands
            return btrue;
        }
    }

    // Need more update's calls to proceed remaining commands.
    return bfalse;
}

void CommandList::addCommand(Command *_pCmd)
{
    ITF_ASSERT(_pCmd != NULL);

    _pCmd->setCommandList(this);

    m_commandList.push_back(_pCmd);
}

Command* CommandList::getPreviousCommand(Command::Type::Enum _type, Command *_caller)const
{
    ITF_ASSERT(_caller == m_commandList[m_currentCommandIndex]);

    for(i32 idx = (m_currentCommandIndex - 1); idx >= 0; --idx)
    {
        Command* pCmd = m_commandList[idx];

        ITF_ASSERT(pCmd != NULL);

        if(pCmd->isTypeOf(_type))
        {
            return pCmd;
        }
    }

    return NULL;
}

void CommandList::reset()
{
    m_currentCommandIndex = -1;
    m_firstCommandUpdate = bfalse;

    for(ITF_VECTOR<Command*>::iterator ite = m_commandList.begin(); ite != m_commandList.end(); ++ite)
    {
        Command *pCmd = (*ite);
        SF_DEL(pCmd);
    }
    m_commandList.clear();
}

}