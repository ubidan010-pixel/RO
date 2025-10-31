#ifndef _ITF_FILEMANAGER_ITF_IPAD_H_
#define _ITF_FILEMANAGER_ITF_IPAD_H_

#ifdef ITF_IPAD

#ifndef _ITF_FILEMANAGER_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_H_


namespace ITF
{
    class FileManager_ITF_iPad : public FileManager_ITF
	{
		 bbool           fileExists(const String& _szFileName);
	};
} //namespace ITF

#endif //ITF_IPAD

#endif  //_ITF_FILEMANAGER_ITF_IPAD_H_