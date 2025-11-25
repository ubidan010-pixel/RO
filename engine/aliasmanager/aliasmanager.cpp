#include "precompiled_engine.h"

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

namespace ITF
{

    void DeprecatedAliasSlot::serialize(ArchiveMemory& _archive)
    {
        m_key.serialize(_archive);
        m_path.serialize(_archive);
    }

    void AliasSlot::serialize(ArchiveMemory& _archive)
    {
        m_path.serialize(_archive);
        m_wildCardPattern.serialize(_archive);
        if (m_wildCardPattern.isEmpty())
        {
            m_wildCardPattern = "*.*";
        }
    }

    Vector<String> AliasSlot::getWildCardList() const
    {
        Vector<String> wildcards;

        m_wildCardPattern.tokenize(
            [&](const u16* _start, u32 len)
            {
                
                wildcards.push_back(String{ _start, len });
            },
            static_cast<u16>(L';'));
        return wildcards;
    }

    void AliasContainer::addSlot(const String& _key, const String& _path, const String& _group, const String& _wildcard)
    {
        String key =  _key;
        key.toLower();
        String path = _path;
        path.toLower();

        m_groupAliases[_group][key] = AliasSlot(path, _wildcard);
    }

#ifdef ITF_WINDOWS
    void AliasContainer::save(const String& _filename)
    {
        ArchiveMemory arch;

        serialize(arch);

        File* writeFile = FILEMANAGER->openFile(_filename, ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        if (writeFile)
        {
            writeFile->write(arch.getData(),arch.getSize());
            FILEMANAGER->closeFile(writeFile);
            writeFile = NULL;
        }
    }
#endif //ITF_WINDOWS

    void AliasContainer::load(const String& _filename)
    {
        m_groupAliases.clear();

        File* pFile = FILEMANAGER->openFile(_filename,ITF_FILE_ATTR_READ);
        ITF_ASSERT_MSG(pFile,"the alias file is missing %s", StringToUTF8(_filename).get());
        if (pFile)
        {
            u32 length = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary, u8[length]);

            bbool rdValid = pFile->read(pBuffer, length);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            if ( rdValid )
            {
                ArchiveMemory rd(pBuffer, length);
                serialize(rd);
            #ifdef SUPPORT_DEPCOLLECTOR
                m_listFileAlias.insert(_filename);
            #endif //SUPPORT_DEPCOLLECTOR
            }
            SF_DEL_ARRAY(pBuffer);
        }
    }

    void AliasContainer::serialize(ArchiveMemory& _archive)
    {
        u32 version = VERSION_GROUP_SUPPORT;

        _archive.serialize(version);

        if (version >= VERSION_GROUP_SUPPORT)
        {
            m_groupAliases.serialize(_archive);
        }
        else
        {
            ITF_ASSERT(_archive.isReading()); // Old format is only read.
            ITF_MAP<Key, DeprecatedAliasSlot> oldFormat;
            oldFormat.serialize(_archive);

            m_groupAliases[""].clear();

            for (auto& oldAlias : oldFormat)
            {
                const String& key = oldAlias.first;
                const DeprecatedAliasSlot& slot = oldAlias.second;
                addSlot(key, slot.getPath());
            }
        }
    }

    const String& AliasContainer::getPath(const String& _key, const String& _group)
    {
        auto groupIter = m_groupAliases.find(_group);
        if (groupIter != m_groupAliases.end())
        {
            AliasToPath& keyToSlot = groupIter->second;
            String lowerKey = _key;
            lowerKey.toLower();
            auto slotIter = keyToSlot.find(lowerKey);
            if (slotIter != keyToSlot.end())
            {
                return slotIter->second.getPath();
            }
        }

        if (_group.isEmpty())
        {
            ITF_ASSERT_MSG(0, "Path not found for key %s", StringToUTF8(_key).get());
        }
        else
        {
            ITF_ASSERT_MSG(0, "Path not found for key %s on group %s", StringToUTF8(_key).get(), StringToUTF8(_group).get());
        }
        return String::emptyString;
    }

#ifdef SUPPORT_DEPCOLLECTOR
    void AliasContainer::getDependencies(DepCollector& _depCollector, const String& _group) const
    {   
        for (const String & file : m_listFileAlias)
            _depCollector.add(file);

        auto iterGroup = m_groupAliases.find(_group);
        if (iterGroup == m_groupAliases.end())
        {
            return;
        }

        const AliasToPath& aliasSlots = iterGroup->second;

        for (auto& aliasSlot : aliasSlots)
        {
            String folder;
            String path = aliasSlot.second.getPath();
            FILEMANAGER->TranslatePath(folder, path);
            if (Directory::exists(folder))
            {
                if (!path.endsWith("/") && !path.endsWith("\\"))
                    path += "/";

                auto wildcards = aliasSlot.second.getWildCardList();

                Vector<String> files;
                for (const String& wildcard : wildcards)
                {
                    Directory::searchFilesRelative(files, folder, wildcard, btrue);
                }

                for (const String & file : files)
                {
                    _depCollector.add(path+file);
                }
            }
            else
            {
                _depCollector.add(path);
            }
        }
    }
#endif //SUPPORT_DEPCOLLECTOR

    void AliasManager::load(const String& _filename)
    {
        m_aliasContainer.load(_filename);
    }

    void AliasManager::loadDefault()
    {
        m_aliasContainer.load("common.alias");
    }
}
