#include "precompiled_cmdtr.h"

#ifndef _ITF_COMMAND_SYS_REMOVE_FILES_H_
    #include "commandTransaction\commands\Command_SYS_RemoveFiles.h"
#endif // _ITF_COMMAND_SYS_REMOVE_FILES_H_

#ifndef _ITF_FILESERVER_H_
    #include "engine\file\FileServer.h"
#endif // _ITF_FILESERVER_H_

namespace ITF
{

    Command_SYS_RemoveFiles::Command_SYS_RemoveFiles(ITF_VECTOR<String>* _pPaths)
    : Command(Type::SYS_RemoveFiles), m_pPaths(_pPaths), m_OkCount(0), m_uCurrentIndex(0)
    {
    }

    void Command_SYS_RemoveFiles::onEnter()
    {
        ITF_ASSERT(m_pPaths);
    }

    bbool Command_SYS_RemoveFiles::onUpdate()
    {
        if(m_uCurrentIndex < (*m_pPaths).size())
        {
            String fullPath;
            FILESERVER->getAbsolutePath((*m_pPaths)[m_uCurrentIndex], fullPath);

            bbool succeed = btrue;

            if(File::exists(fullPath))
            {
                succeed = File::DeleteFile(fullPath);
            }

#if ITF_DEBUG_LEVEL > 0
            LOG("\tCommand_RemoveFiles \"%S\"\n", fullPath.cStr());
#endif // ITF_DEBUG_LEVEL > 0

            if(succeed)
            {
                m_OkCount++;
            }

            ITF_ASSERT(succeed);

            ++m_uCurrentIndex;

            return bfalse;
        }
        
        return btrue;
    }

    void Command_SYS_RemoveFiles::onExit()
    {
        if((*m_pPaths).size() == m_OkCount)
        {
            m_bSucceed = btrue;
        }
    }

}