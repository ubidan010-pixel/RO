#include "precompiled_cmdtr.h"

#ifndef _ITF_COMMAND_NEWRESOURCEDB_H_
    #include "commandTransaction\commands\Command_newResourceDb.h"
#endif // _ITF_COMMAND_NEWRESOURCEDB_H_


#ifndef _ITF_TRANSACTION_NEWRESOURCEBD_H_
#include "commandTransaction\transactions\Transaction_NewResourceDb.h"
#endif // _ITF_TRANSACTION_NEWRESOURCEBD_H_


namespace ITF
{

Command_newResourceDB::Command_newResourceDB(const ITF_VECTOR<String>& _paths)
:  m_Paths(_paths),m_pTransactionNewDb(NULL),Command(Type::DB_NewResource) 
{

}

void Command_newResourceDB::onEnter()
{
    m_pTransactionNewDb = new Transaction_NewResourceDb_Send(&m_Paths);
}

bbool Command_newResourceDB::onUpdate()
{
    return m_pTransactionNewDb->update();
}

void Command_newResourceDB::onExit()
{
    m_bSucceed = btrue;
    SF_DEL(m_pTransactionNewDb);
}

}