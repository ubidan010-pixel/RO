#ifndef _ITF_COMMAND_SC_REMOVE_FILES_H_
#define _ITF_COMMAND_SC_REMOVE_FILES_H_

#ifndef _ITF_COMMAND_H_   
    #include "engine/commandlist/Command.h"  
#endif

#ifndef _ITF_SCADAPTER_H_
    #include "engine\AdaptersInterfaces\SCAdapter.h"
#endif //_ITF_SCADAPTER_H_

namespace ITF
{

class Command_SC_RemoveFiles : public Command
{
public:
    Command_SC_RemoveFiles(ITF_VECTOR<String>*  _pPaths);

    // This function is called when the command starts.
    void    onEnter();

    // This function is called when the command runs.
    bbool   onUpdate();

    // This function is called when the command stops.
    void    onExit();

private:
    SCRequestID m_RequestID;
};

}

#endif // _ITF_COMMAND_SC_REMOVE_FILES_H_