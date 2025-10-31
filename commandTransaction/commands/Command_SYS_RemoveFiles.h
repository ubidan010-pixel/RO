#ifndef _ITF_COMMAND_SYS_REMOVE_FILES_H_
#define _ITF_COMMAND_SYS_REMOVE_FILES_H_

#ifndef _ITF_COMMAND_H_   
    #include "engine/commandlist/Command.h"  
#endif

namespace ITF
{

class Command_SYS_RemoveFiles : public Command
{
public:
    Command_SYS_RemoveFiles(ITF_VECTOR<String>*  _pPaths);

    // This function is called when the command starts.
    void    onEnter();

    // This function is called when the command runs.
    bbool   onUpdate();

    // This function is called when the command stops.
    void    onExit();

private:
    ITF_VECTOR<String>* m_pPaths;
    u32                 m_uCurrentIndex;
    u32                 m_OkCount;
};

}

#endif // _ITF_COMMAND_SYS_REMOVE_FILES_H_