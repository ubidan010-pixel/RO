#ifndef _ITF_COMMAND_NEWRESOURCEDB_H_
#define _ITF_COMMAND_NEWRESOURCEDB_H_

#ifndef _ITF_COMMAND_H_   
    #include "engine/commandlist/Command.h"  
#endif


namespace ITF
{
class Transaction_NewResourceDb_Send;

class Command_newResourceDB : public Command
{

public:
    Command_newResourceDB(const ITF_VECTOR<String>&  _paths);

    // This function is called when the command starts.
    void    onEnter();

    // This function is called when the command runs.
    bbool   onUpdate();

    // This function is called when the command stops.
    void    onExit();

    

private:
    ITF_VECTOR<String>  m_Paths;

    Transaction_NewResourceDb_Send* m_pTransactionNewDb;
};

}

#endif // _ITF_COMMAND_NEWRESOURCEDB_H_