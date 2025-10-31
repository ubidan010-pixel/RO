#include "precompiled_cmdtr.h"

#ifndef _ITF_COMMAND_DB_MARK_RESOURCE_REMOVED_H_
    #include "commandTransaction\commands\Command_DB_markResourceRemoved.h"
#endif // _ITF_COMMAND_DB_MARK_RESOURCE_REMOVED_H_

#ifndef _ITF_STORAGEADAPTER_H_
    #include "engine/AdaptersInterfaces/StorageAdapter.h"
#endif //_ITF_STORAGEADAPTER_H_

namespace ITF
{

    Command_DB_MarkResourceRemoved::Command_DB_MarkResourceRemoved(const ITF_VECTOR<String>* _pPaths)
    : Command(Type::DB_MarkResourceRemoved), m_pPaths(_pPaths)
    {
    }

    void Command_DB_MarkResourceRemoved::onEnter()
    {
    }

    bbool Command_DB_MarkResourceRemoved::onUpdate()
    {
        m_bSucceed = STORAGE_ADAPTER->markResourcesAsRemoved(*m_pPaths);
        ITF_ASSERT(m_bSucceed);
        return btrue;
    }

    void Command_DB_MarkResourceRemoved::onExit()
    {
    }

} // namespace ITF