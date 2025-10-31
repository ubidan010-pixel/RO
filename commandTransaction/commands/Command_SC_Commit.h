#ifndef _ITF_COMMAND_SC_COMMIT_H_
#define _ITF_COMMAND_SC_COMMIT_H_

#ifndef _ITF_COMMAND_H_   
    #include "engine/commandlist/Command.h"  
#endif

#ifndef _ITF_SCADAPTER_H_
    #include "engine/AdaptersInterfaces/SCAdapter.h"
#endif // _ITF_SCADAPTER_H_

namespace ITF
{

class Command_SC_Commit : public Command
{
public:
    // Warning : don't use a stack pointer for "_pRevision"
    Command_SC_Commit(const ITF_VECTOR<String>* _pPaths, const String& _commiter, u32 *_pRevision);

    // This function is called when the command starts.
    void    onEnter();

    // This function is called when the command runs.
    bbool   onUpdate();

    // This function is called when the command stops.
    void    onExit();

private:
    SCRequestID                 m_RequestID;
    String                      m_Commiter;
    const ITF_VECTOR<String>*   m_pPaths;
    u32*                        m_pRevision;
    bbool                       m_bWaitLastRevision;
};

}

#endif // _ITF_COMMAND_SC_COMMIT_H_