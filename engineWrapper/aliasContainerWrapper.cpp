#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#include "core/container/extendedMap.h"
#include "core/itfstring.h"
#include "aliasContainerWrapper.h"
#include "utils.h"


aliasContainerWrapper::aliasContainerWrapper()
{
    m_aliasContainer = new ITF::AliasContainer();
}

aliasContainerWrapper::~aliasContainerWrapper()
{
    delete m_aliasContainer;
    m_aliasContainer = NULL;
}

void aliasContainerWrapper::getAllAliases(ICollection<aliasSlotWrapper^> ^ _aliasMap)
{
    _aliasMap->Clear();

    const auto & aliases = m_aliasContainer->getAllAliases();

    for (auto & [strGroup, groupAliases] : aliases)
    {
        System::String^ group = gcnew System::String(strGroup.wcharCStr());

        for (auto & [key, pathAndWildcard] : groupAliases)
        {
            aliasSlotWrapper^ slot = gcnew aliasSlotWrapper();
            slot->group = group;
            slot->key = gcnew System::String(key.wcharCStr());
            slot->path = gcnew System::String(pathAndWildcard.getPath().wcharCStr());
            slot->wildcardPattern = gcnew System::String(pathAndWildcard.getWildCardPattern().wcharCStr());
            _aliasMap->Add(slot);
        }
    }
}

void aliasContainerWrapper::save(String^ _filename)
{
    ITF::String str_filename     = utils::getITFString(_filename);
    m_aliasContainer->save(str_filename);
}


void aliasContainerWrapper::load(String^ _filename)
{
    ITF::String str_filename     = utils::getITFString(_filename);
    m_aliasContainer->load(str_filename);
}

void aliasContainerWrapper::addAlias(String^ _group, String^ _key, String^ _path, String^ _wildcardPattern)
{
    ITF::String str_group = utils::getITFString(_group);
    ITF::String str_key = utils::getITFString(_key);
    ITF::String str_path = utils::getITFString(_path);
    ITF::String str_wildcard = utils::getITFString(_wildcardPattern);

    m_aliasContainer->addSlot(str_key, str_path, str_group, str_wildcard);
}
