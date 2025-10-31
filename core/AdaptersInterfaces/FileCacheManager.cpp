#include "precompiled_core.h"

#ifdef ITF_SUPPORT_LOGICDATABASE

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILECACHEMANAGER_H_
#include "core/AdaptersInterfaces/FileCacheManager.h"
#endif //_ITF_FILECACHEMANAGER_H_

#ifndef _ITF_FILEREMOTE_H_
#include "core/file/FileRemote.h"
#endif //_ITF_FILEREMOTE_H_


namespace ITF
{

    FileCacheManager::FileCacheManager():m_enableCache(bfalse)
    {
        Synchronize::createCriticalSection(&m_cs);
    }

    FileCacheManager::~FileCacheManager()
    {
        Synchronize::destroyCriticalSection(&m_cs);
        destroyCache();
    }

    void FileCacheManager::closeFile(File* pFile)
    {
        ITF_SET<File*>::iterator iter = m_SetElements.find(pFile);

        if (iter != m_SetElements.end()) //the object is found ;dont do anything
        {
            return;
        }


        //close the real file
        FILEMANAGER->closeFile(pFile);
    }

    u64 FileCacheManager::getLastTimeWriteAccess(const String& _filename)
    {
        if (m_enableCache)
        {
           File* pFile = openFile(_filename);
           closeFile(pFile);

           String lowerKey = _filename;
           lowerKey.toLower();

           csAutoLock cs(m_cs);
           ITF_MAP<String_Uncached,FileRemote*>::iterator iter = m_Elements.find(lowerKey);

           if (iter != m_Elements.end())
           {    
                return ((FileRemote*)pFile)->getLastTimeWriteAccess();
           }
        }

        return FILEMANAGER->getLastTimeWriteAccess(_filename);
    }

    File* FileCacheManager::openFile(const String& _filename)
    {
        if (m_enableCache)
        {
            String lowerKey = _filename;
            lowerKey.toLower();

            csAutoLock cs(m_cs);
            ITF_MAP<String_Uncached,FileRemote*>::iterator iter = m_Elements.find(lowerKey);

            if (iter != m_Elements.end())
            {   
                FileRemote* pFile = (*iter).second;
                pFile->seek(0,0);
                return pFile;
            }

            File* pFile= FILEMANAGER->openFile(_filename,ITF_FILE_ATTR_READ);

            if (pFile)
            {
                u32 fileSize = (u32)pFile->getLength();
                char * readBuffer = newAlloc(mId_Temporary, char[fileSize]);
                ITF_ASSERT_MSG(readBuffer,"not enought memory to allocate the cache file");
                u32 sizeRead = 0;
                pFile->read(readBuffer,fileSize,&sizeRead);

                u64 writeTime = pFile->getLastTimeWriteAccess();

                FILEMANAGER->closeFile(pFile);
                pFile = NULL;

                FileRemote* pFileCached = new FileRemote(fileSize);

                pFileCached->setLastTimeWriteAccess(writeTime);
                pFileCached->writeStream(0,(u8*)readBuffer,fileSize);

                SF_DEL_ARRAY(readBuffer);

                {
                    m_Elements[lowerKey]    = pFileCached;
                    m_SetElements.insert(pFileCached);

                }

                return pFileCached;
            }

        }

      
        return FILEMANAGER->openFile(_filename,ITF_FILE_ATTR_READ);
    }


    void FileCacheManager::destroyCache()
    {
#ifdef ITF_WINDOWS
        for (ITF_SET<File*>::iterator iter = m_SetElements.begin();iter!=m_SetElements.end();++iter)
        {
            delete (*iter);
        }
        m_SetElements.clear();
#endif //ITF_WINDOWS
    }

} //namespace ITF

#endif //ITF_SUPPORT_LOGICDATABASE
