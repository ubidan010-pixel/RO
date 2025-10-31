#ifndef _ITF_COMMAND_SC_ADDFILES_H_
#define _ITF_COMMAND_SC_ADDFILES_H_

#ifndef _ITF_COMMAND_H_   
    #include "engine/commandlist/Command.h"  
#endif

#ifndef _ITF_SCADAPTER_H_
    #include "engine/AdaptersInterfaces/SCAdapter.h"
#endif // _ITF_SCADAPTER_H_

namespace ITF
{

class Command_SC_AddFiles : public Command
{
public:
    Command_SC_AddFiles(ITF_VECTOR<String>  _paths);

    // This function is called when the command starts.
    void    onEnter();

    // This function is called when the command runs.
    bbool   onUpdate();

    // This function is called when the command stops.
    void    onExit();

private:
    ITF_VECTOR<String>  m_Paths;
    SCRequestID         m_RequestID;
};

}

#endif // _ITF_COMMAND_SC_ADDFILES_H_