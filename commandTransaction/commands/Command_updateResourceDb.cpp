#include "precompiled_cmdtr.h"

#ifndef _ITF_TRANSACTION_UPDATERESOURCEBD_H_
#include "commandTransaction\transactions\Transaction_UpdateResourceDb.h"
#endif // _ITF_TRANSACTION_UPDATERESOURCEBD_H_

#ifndef _ITF_COMMAND_UPDATERESOURCEDB_H_
#include "commandTransaction\commands\Command_updateResourceDb.h"
#endif // _ITF_COMMAND_UPDATERESOURCEDB_H_

namespace ITF
{

Command_UpdateResourceDB::Command_UpdateResourceDB(const ITF_VECTOR<String>* _vPaths)
: m_pvPaths(_vPaths), m_pTransactionUpdateDB(NULL),Command(Type::DB_UpdateResource) 
{
   

}

void Command_UpdateResourceDB::onEnter()
{
    m_pTransactionUpdateDB = new Transaction_UpdateResourceDb_Send(m_pvPaths);
}

bbool Command_UpdateResourceDB::onUpdate()
{
    return m_pTransactionUpdateDB->update();
}

void Command_UpdateResourceDB::onExit()
{
    m_bSucceed = btrue;
    SF_DEL(m_pTransactionUpdateDB);
}

}