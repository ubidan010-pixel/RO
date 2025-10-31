#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#include "core/container/extendedMap.h"
#include "core/itfstring.h"
#include "aliasContainerWrapper.h"
#include "utils.h"


aliasContainerWrapper::aliasContainerWrapper()
{
    m_aliasContainer = new ITF::aliasContainer();
}

aliasContainerWrapper::~aliasContainerWrapper()
{
    delete m_aliasContainer;
    m_aliasContainer = NULL;
}

void aliasContainerWrapper::getAllAlias(Hashtable^ _aliasMap)
{
    ITF_MAP<ITF::String,ITF::aliasSlot> mAlias = m_aliasContainer->getAllAlias();
    _aliasMap->Clear();

    ITF_MAP<ITF::String,ITF::aliasSlot> ::iterator iter;
    for (iter= mAlias.begin();iter!= mAlias.end();iter++)
    {
        String^ key =  gcnew String((wchar_t*)(*iter).first.cStr());
        const ITF::aliasSlot& slot = (*iter).second;
        const ITF::String& slotPath = slot.getPath();
        String^ path =  gcnew String((wchar_t*)slotPath.cStr());
        _aliasMap[key] = path;
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

void aliasContainerWrapper::addAlias(String^ _key,String^ _path)
{
    ITF::String str_key     = utils::getITFString(_key);
    ITF::String str_path    = utils::getITFString(_path);
    m_aliasContainer->addSlot(str_key,str_path);
}
