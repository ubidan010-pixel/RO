#ifndef _ITF_FILEMANAGER_ITF_WIN_H_
#define _ITF_FILEMANAGER_ITF_WIN_H_

#pragma once

#ifndef _ITF_FILEMANAGER_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_H_


namespace ITF
{
    
class FileManager_ITF_Win : public FileManager_ITF
{
public:
    bbool fileChooser(bbool _save, String& _defaultAndResult, const wchar_t* _filter = NULL, const String& _workingPath = "")const;
    bbool folderChooser(const String& _title, const String& startPath, String& _result)const;
    
private:
    mutable String m_lastInitialDir;
};


} //namespace ITF


#endif  //_ITF_FILEMANAGER_ITF_WIN_H_
