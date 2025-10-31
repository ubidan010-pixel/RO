#ifndef _ITF_FILECACHEMANAGER_H_
#define _ITF_FILECACHEMANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifdef ITF_SUPPORT_LOGICDATABASE

namespace ITF
{
    class FileRemote;
    class FileCacheManager : public TemplateSingleton<FileCacheManager>
    {
    public:


        FileCacheManager();

        ~FileCacheManager();

        File* openFile(const String& _filename);

        u64 getLastTimeWriteAccess(const String& _filename);
        void closeFile(File* pFile);

        void useCache(bbool _bEnableCache)  {m_enableCache = btrue;}
        ITF_INLINE bbool isCacheEnable()    {return m_enableCache;}
        void destroyCache();
        
    protected:
        ITF_MAP<String_Uncached,FileRemote*>        m_Elements;
        ITF_SET<File*>                              m_SetElements;
        bbool                                       m_enableCache;

        ITF_THREAD_CRITICAL_SECTION                 m_cs;
    };

#define FILECACHEMANAGER            FileCacheManager::getptr()

} //namespace ITF


#endif //ITF_SUPPORT_LOGICDATABASE
#endif  //_ITF_FILECACHEMANAGER_H_
