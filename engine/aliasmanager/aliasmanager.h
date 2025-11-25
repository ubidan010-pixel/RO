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
    // DeprecatedAliasSlot is old structure kept for backward compatibility with old alias files (version < AliasContainer::VERSION_GROUP_SUPPORT)
    class DeprecatedAliasSlot
    {
    public:
        DeprecatedAliasSlot() = default;
        explicit DeprecatedAliasSlot(const String& _key, const String& _path)
            : m_key(_key)
            , m_path(_path)
        {}

        void serialize(ArchiveMemory& _archive);
        const String& getPath() const { return m_path; }

    protected:
        String m_key; //debug purpose could be clear in final version
        String m_path;
    };


    class AliasSlot
    {
    public:
        AliasSlot() = default;
        explicit AliasSlot(const String& _path, const String& _wildcardPattern)
            : m_path(_path)
            , m_wildCardPattern(_wildcardPattern)
        {
        }

        void serialize(ArchiveMemory& _archive);

        const String& getPath() const { return m_path; }

        // A wildCardPattern is a pattern like "*.png;*.jpg"
        const String& getWildCardPattern() const { return m_wildCardPattern; }

        // Splitted wild card pattern into a list of wild cards (eg: "*.png;*.jpg" => ["*.png", "*.jpg"])
        Vector<String> getWildCardList() const;

    protected:
        String m_path;
        String m_wildCardPattern = "*.*";
    };


    class AliasContainer
    {
    public:

        // A group support has been added in version 1
        // A group allows for instance to have different alias for different platform (eg: PC, Xbox, PS4, etc)
        // The default group is the empty string ""
        enum : u32 { VERSION_GROUP_SUPPORT = 1 };

        void addSlot(const String& _key, const String& _path, const String & _group = "", const String& _wildcardPattern = "*.*");
        void serialize(ArchiveMemory& _archive);
    #ifdef ITF_WINDOWS
        void save(const String& _filename);
    #endif //ITF_WINDOWS

        void load(const String& _filename);

        using Group = String;
        using Key = String;

        using AliasToPath = Map<Key, AliasSlot>;
        using GroupToAliases = Map<Group, AliasToPath>;

        const GroupToAliases& getAllAliases() const { return m_groupAliases; }

        const String& getPath(const String& _key, const String &_group = "");

    #ifdef SUPPORT_DEPCOLLECTOR
        void getDependencies(DepCollector& _depCollector, const String& _group = "") const;
    #endif //SUPPORT_DEPCOLLECTOR

    protected:

    #ifdef SUPPORT_DEPCOLLECTOR
        ITF_SET<String> m_listFileAlias;
    #endif  //SUPPORT_DEPCOLLECTOR

        GroupToAliases m_groupAliases;
    };

    class AliasManager : public TemplateSingleton<AliasManager>
    {
    public:
 
        void load(const String& _filename);
        void loadDefault();
        const String& getPath(const String& _key, const String & group = "") { return m_aliasContainer.getPath(_key, group); }
        const AliasContainer& getContainer() const { return m_aliasContainer; }

    protected:
        AliasContainer m_aliasContainer;
    };

    #define ALIASMANAGER AliasManager::getptr()
    #define GETPATH_ALIAS(name_)  ALIASMANAGER->getPath(name_)
    #define GETPATH_GROUP_ALIAS(name_, group_)  ALIASMANAGER->getPath((name_), (group_))
}

#endif //_ITF_ALIASMANAGER_H_
