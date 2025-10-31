#ifndef _ITF_COMMANDLIST_H_
#define _ITF_COMMANDLIST_H_

#ifndef _ITF_COMMAND_H_   
#include "engine/commandlist/Command.h"
#endif //_ITF_COMMAND_H_   

namespace ITF
{
    class CommandList
    {
    public:
        CommandList();
        virtual ~CommandList();

        /// Sequentially proceed all commands.
        /// @return btrue if the commandList is fully processed.
        bbool update(void);
        
        /// Add a new command.
        /// @param _pCmd the command to be added
        void addCommand(Command * _pCmd);

        /// Get the first previous command of a specific type
        /// @param _type type of the searched command
        /// @param _caller caller object
        /// @return a pointer to the first previous command of type given
        Command* getPreviousCommand(Command::Type::Enum _type, Command *_caller)const;

        /// @brief  Resets the CommandList. 
        void reset();

    private:
        bbool                    m_firstCommandUpdate;
        i32                     m_currentCommandIndex;
        ITF_VECTOR<Command*>    m_commandList;
    };
}

#endif // _ITF_COMMANDLIST_H_