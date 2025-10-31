#ifndef _ITF_COMMAND_H_
#define _ITF_COMMAND_H_

namespace ITF
{

class CommandList;

/// Abstract class for a command with states
class Command
{
public:
    /// Used in commandlist process to get a previous command of a custom type
    struct Type
    {
        enum Enum
        {
            SC_AddFile,
            SC_Commit,
            DB_NewResource,
            DB_UpdateResource,
            SYS_RemoveFiles,
            SC_RemoveFiles,
            DB_MarkResourceRemoved,
            Count,
            ENUM_FORCE_SIZE_32(0)
        };
    };

public:

    virtual ~Command() {}
    Command(Type::Enum _type) : m_pCommandList(NULL), m_Type(_type), m_bSucceed(bfalse) {}

    /// Called when the command starts.
    virtual void                onEnter()=0;

    /// Called when the command runs.
    /// @return true if command do not need another update
    virtual bbool               onUpdate()=0;

    /// Called when the command stops.
    virtual void                onExit()=0;

    /// @return btrue if the command ran without error
    ITF_INLINE bbool            succeeded()const;

    /// Set the container list for the command
    /// @param _pCommandList the container list
    ITF_INLINE void             setCommandList(CommandList *_pCommandList);

    /// Get the container list for the command
    /// @return the container list
    ITF_INLINE CommandList*     getCommandList()const;

    /// Test if the command is a specific type
    /// @param _type command type
    /// @return true if the command is same type as parameter
    ITF_INLINE bbool            isTypeOf(const Type::Enum _type)const;

protected:
    bbool           m_bSucceed;

private:
    CommandList*    m_pCommandList;
    Type::Enum      m_Type;
};

}

#include "Command.inl"

#endif // _ITF_COMMAND_H_