#ifndef _ITF_FILEMANAGER_ITF_CTR_H_
#define _ITF_FILEMANAGER_ITF_CTR_H_

#ifndef _ITF_FILEMANAGER_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_H_


namespace ITF
{
    class FileManager_ITF_CTR : public FileManager_ITF
    {
    public:
        FileManager_ITF_CTR();
        virtual ~FileManager_ITF_CTR();

        bbool           fileExists(const String& _szFileName);
    };
} //namespace ITF

#endif  //_ITF_FILEMANAGER_ITF_CTR_H_