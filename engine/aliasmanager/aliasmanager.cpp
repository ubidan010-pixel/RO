#include "precompiled_engine.h"

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace ITF
{

///--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void aliasSlot::serialize(ArchiveMemory& _archive)
    {
        m_key.serialize(_archive);
        m_path.serialize(_archive);
    }
    
    
///--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void aliasContainer::addSlot(const String& _key,const String& _path)
    {
        String key =  _key;
        key.toLower();
        String path = _path;
        path.toLower();

        m_mSlot[key] = aliasSlot(key,path);
     
    }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef ITF_WINDOWS
    void aliasContainer::save(const String& _filename)
    {
        ArchiveMemory arch;

        serialize(arch);

        File* writeFile = FILEMANAGER->openFile(_filename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        if (writeFile)
        {
            writeFile->write(arch.getData(),arch.getSize());
            FILEMANAGER->closeFile(writeFile);
            writeFile = NULL;
        }

    }   
#endif //ITF_WINDOWS
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void aliasContainer::load(const String& _filename)
    {
        m_mSlot.clear();

        File* pFile = FILEMANAGER->openFile(_filename,ITF_FILE_ATTR_READ);
        ITF_ASSERT_MSG(pFile,"the alias file is missing %s", StringToUTF8(_filename).get());
        if (pFile)
        {
            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            bbool rdValid = pFile->read(pBuffer,lenght);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            if ( rdValid )
            {
                ArchiveMemory rd(pBuffer,lenght);
                serialize(rd);
#ifdef SUPPORT_DEPCOLLECTOR
                m_listFileAlias.insert(_filename);
#endif //SUPPORT_DEPCOLLECTOR
            }

            SF_DEL_ARRAY(pBuffer);
        }

    }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void aliasContainer::serialize(ArchiveMemory& _archive)
    {
        u32 version = 0;

        _archive.serialize(version);
        m_mSlot.serialize(_archive);
    }

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    const String& aliasContainer::getPath(const String& _key)
    {
        String lowerKey = _key;
        lowerKey.toLower();
        ITF_MAP<String,aliasSlot> ::const_iterator iter = m_mSlot.find(lowerKey);
        if (iter != m_mSlot.end())
        {
            return (*iter).second.getPath();
        }

        ITF_ASSERT_MSG(0,"Path not found for key : %s", StringToUTF8(lowerKey).get());
        return String::emptyString;
    }

    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef SUPPORT_DEPCOLLECTOR
    void aliasContainer::getDependencies(DepCollector& _depCollector) const
    {   
        for (ITF_SET<String> ::const_iterator iter = m_listFileAlias.begin();iter!=m_listFileAlias.end();iter++)
            _depCollector.add(*iter);

        ITF_MAP<String,aliasSlot> ::const_iterator iter = m_mSlot.begin();
        for (iter;iter!=m_mSlot.end();iter++)
        {
            String Folder;
            const String& path = (*iter).second.getPath();
            FILEMANAGER->TranslatePath(Folder,path);
            if (Directory::exists(Folder))
            {
                ITF_VECTOR<String> vFiles;
                Directory::searchFilesRelative(vFiles,Folder,"*.*",btrue);

                for ( ITF_VECTOR<String>::const_iterator iterFile = vFiles.begin();iterFile!=vFiles.end();iterFile++)
                {
                    _depCollector.add(path+(*iterFile));
                }
            }
            else
            {
                _depCollector.add(path);
            }
            
        }
    }
#endif //SUPPORT_DEPCOLLECTOR

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void AliasManager::load(const String& _filename)
    {
        m_aliasContainer.load(_filename);
    }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    void AliasManager::loadDefault()
    {
        m_aliasContainer.load("common.alias");
    }


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

}


///--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
