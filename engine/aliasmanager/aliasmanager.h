#ifndef _ITF_ALIASMANAGER_H_
#define _ITF_ALIASMANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_


#if defined (ITF_WINDOWS) && !defined (ITF_CONSOLE)
#define SUPPORT_DEPCOLLECTOR
#endif

#ifdef SUPPORT_DEPCOLLECTOR
#ifndef _ITF_DEP_COLLECTOR_H_
#include "engine/dependencies/DepCollector.h"
#endif //_ITF_DEP_COLLECTOR_H_
#endif //SUPPORT_DEPCOLLECTOR

namespace ITF
{

///--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class aliasSlot
{
public:

    aliasSlot() {};
    aliasSlot(const String& _key,const String& _path):m_key(_key),m_path(_path) {}
    void serialize(ArchiveMemory& _archive);

    const String& getPath() const {return m_path;}

protected:

    String m_key;//debug purpose could be clear in final version
    String m_path;

};

///--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class aliasContainer
{
public:
    
    void addSlot(const String& _key,const String& _path);
    void serialize(ArchiveMemory& _archive);
#ifdef ITF_WINDOWS
    void save(const String& _filename);
#endif //ITF_WINDOWS

    void load(const String& _filename);
    const ITF_MAP<String,aliasSlot>& getAllAlias() {return m_mSlot;};

#ifdef SUPPORT_DEPCOLLECTOR
    void getDependencies(DepCollector& _depCollector) const;
#endif //SUPPORT_DEPCOLLECTOR
    const String& getPath(const String& _key);

protected:

#ifdef SUPPORT_DEPCOLLECTOR
    ITF_SET<String>        m_listFileAlias;
#endif  //SUPPORT_DEPCOLLECTOR
    ITF_MAP<String,aliasSlot> m_mSlot;

};

///--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class AliasManager : public TemplateSingleton<AliasManager>
{
public:
 
    void load(const String& _filename);
    void loadDefault();
    const String& getPath(const String& _key)   {return m_aliasContainer.getPath(_key);}
    const aliasContainer& getContainer()        {return m_aliasContainer;}

protected:
    aliasContainer m_aliasContainer;
};

///--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define ALIASMANAGER AliasManager::getptr()
#define GETPATH_ALIAS(_name)  ALIASMANAGER->getPath(_name)
}
#endif //_ITF_ALIASMANAGER_H_
