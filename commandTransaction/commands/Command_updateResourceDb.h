#ifndef _ITF_COMMAND_UPDATERESOURCEDB_H_
#define _ITF_COMMAND_UPDATERESOURCEDB_H_

#ifndef _ITF_COMMAND_H_   
    #include "engine/commandlist/Command.h"  
#endif


namespace ITF
{
class Transaction_UpdateResourceDb_Send;

class Command_UpdateResourceDB : public Command
{

public:
    Command_UpdateResourceDB(const ITF_VECTOR<String>* _paths);

    // This function is called when the command starts.
    void    onEnter();

    // This function is called when the command runs.
    bbool   onUpdate();

    // This function is called when the command stops.
    void    onExit();

    

private:
    const ITF_VECTOR<String>*                   m_pvPaths;
    Transaction_UpdateResourceDb_Send*          m_pTransactionUpdateDB;
};

}

#endif // _ITF_COMMAND_UPDATERESOURCEDB_H_