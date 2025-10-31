#ifndef _ITF_COMMAND_DB_MARK_RESOURCE_REMOVED_H_
#define _ITF_COMMAND_DB_MARK_RESOURCE_REMOVED_H_

#ifndef _ITF_COMMAND_H_   
    #include "engine/commandlist/Command.h"  
#endif


namespace ITF
{

    class Command_DB_MarkResourceRemoved : public Command
    {

    public:
        Command_DB_MarkResourceRemoved(const ITF_VECTOR<String>* _pPaths);

        // This function is called when the command starts.
        void    onEnter();

        // This function is called when the command runs.
        bbool   onUpdate();

        // This function is called when the command stops.
        void    onExit();

    private:
        const ITF_VECTOR<String>*                   m_pPaths;
    };

}

#endif // _ITF_COMMAND_DB_MARK_RESOURCE_REMOVED_H_